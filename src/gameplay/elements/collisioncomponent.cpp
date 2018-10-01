#include "collisioncomponent.hpp"
#include "math/extensions.hpp"
#include "ei/3dintersection.hpp"
#include "gameplay/content.hpp"
#include <functional>
#include <array>

namespace Game {

	using namespace ei;

	BoundingMesh::BoundingMesh(const std::string& _fileName)
	{
		Utils::MeshLoader loader;
		std::string texName;
		loader.Load(_fileName, *this, texName, 
			std::bind(&BoundingMesh::Load, this, std::placeholders::_1, std::placeholders::_2));
	}

	BoundingMesh::BoundingMesh(const ei::Box& _box)
	{
		// create bounding mesh
		vertices.reserve(8);
		const Vec3& min = _box.min;
		const Vec3& max = _box.max;
		const Vec3 center = (min + max) * 0.5f;
		vertices.push_back(min);
		vertices.push_back(Vec3(min.x, min.y, max.z));
		vertices.push_back(Vec3(max.x, min.y, min.z));
		vertices.push_back(Vec3(min.x, max.y, min.z));
		vertices.push_back(max);
		vertices.push_back(Vec3(max.x, max.y, min.z));
		vertices.push_back(Vec3(min.x, max.y, max.z));
		vertices.push_back(Vec3(max.x, min.y, max.z));

		faces.reserve(6);
		supportVectors.reserve(6);
		supportVectors.push_back(Vec3(center.x, center.y, min.z));
		faces.emplace_back(Vec3(0.f, 0.f, -1.f), supportVectors.back());

		supportVectors.push_back(Vec3(center.x, center.y, max.z));
		faces.emplace_back(Vec3(0.f, 0.f, 1.f), supportVectors.back());

		supportVectors.push_back(Vec3(min.x, center.y, center.z));
		faces.emplace_back(Vec3(-1.f, 0.f, 0.f), supportVectors.back());

		supportVectors.push_back(Vec3(max.x, center.y, center.z));
		faces.emplace_back(Vec3(1.f, 0.f, 0.f), supportVectors.back());

		supportVectors.push_back(Vec3(center.x, min.y, center.z));
		faces.emplace_back(Vec3(0.f, -1.f, 0.f), supportVectors.back());

		supportVectors.push_back(Vec3(center.x, max.y, center.z));
		faces.emplace_back(Vec3(0.f, 1.f, 0.f), supportVectors.back());

		const Vec3 minxy = Vec3(min.x, min.y, max.z);
		const Vec3 minxz = Vec3(min.x, max.y, min.z);
		const Vec3 minyz = Vec3(max.x, min.y, min.z);
		const Vec3 maxxy = Vec3(max.x, max.y, min.z);
		const Vec3 maxxz = Vec3(max.x, min.y, max.z);
		const Vec3 maxyz = Vec3(min.x, max.y, max.z);
		triangles.emplace_back(min, minyz, maxxy);
		triangles.emplace_back(min, maxxy, minxz);
		triangles.emplace_back(min, minxy, maxyz);
		triangles.emplace_back(min, minxz, maxyz);
		triangles.emplace_back(min, minyz, maxxz);
		triangles.emplace_back(min, minxy, maxxz);

		triangles.emplace_back(max, maxyz, minxy);
		triangles.emplace_back(max, minxy, maxxz);
		triangles.emplace_back(max, maxxy, minyz);
		triangles.emplace_back(max, maxxz, minyz);
		triangles.emplace_back(max, maxyz, minxz);
		triangles.emplace_back(max, maxxy, minxz);
	}

	// *************************************************************** //
	void BoundingMesh::Load(std::ifstream& _stream, size_t _numVertices)
	{
		uint32_t numFaces;
		Utils::MeshLoader::Read(_stream, numFaces);
		vertices.resize(_numVertices);
		_stream.read(reinterpret_cast<char*>(&vertices.front()), _numVertices * sizeof(Vec3));

		faces.reserve(numFaces);
		supportVectors.reserve(numFaces);
		triangles.reserve(numFaces);
		for (uint32_t i = 0; i < numFaces; ++i)
		{
			IVec3 face;
			Utils::MeshLoader::Read(_stream, face);
			triangles.emplace_back(vertices[face.x], vertices[face.y], vertices[face.z]);
			supportVectors.emplace_back(1.f / 3.f * (vertices[face.x] + vertices[face.y] + vertices[face.z]));
			faces.emplace_back(vertices[face.x], vertices[face.y], vertices[face.z]);

			// seems like the coordinate system is not compatible so transform
			faces.back().n *= -1.f;
			faces.back().d *= -1.f;
		}
	}

	// *************************************************************** //
	bool BoundingMesh::Intersects(const BoundingMesh& _other, const Mat4x4& _transform, HitInfo& _info) const
	{
		// transform one mesh
		std::vector<Vec3> othVertices(_other.vertices.size());
		for (size_t i = 0; i < othVertices.size(); ++i)
			othVertices[i] = Vec3(_transform * Vec4(_other.vertices[i], 1.f));

	//	std::vector<int> slfInds(faces.size());
	//	std::vector<int> othInds(_other.faces.size());

		// check planes of the first mesh against all points of the other
		// all vertices of the first mesh are in the same half space <=0,
		// since it is convex.
		for (const Plane& plane : faces)
		{
			bool isSeperated = true;
			for (const Vec3& p : othVertices)
			{
				if (dot(plane.n, p) + plane.d <= 0.f)
				{
					isSeperated = false;
					break;
				}
			}
			if (isSeperated) 
				return false;
		}

		std::vector<Plane> othFaces;
		othFaces.reserve(_other.faces.size());
		for (size_t i = 0; i < _other.faces.size(); ++i)
		{
			// should already be normalized, but the transformation takes away precision
			othFaces.emplace_back(normalize(Vec3(_transform * Vec4(_other.faces[i].n, 0.f))),
				_transform * _other.supportVectors[i]);
		}

		// other way around
		for (const Plane& plane : othFaces)
		{
			bool isSeperated = true;
			for (const Vec3& p : vertices)
			{
				if (dot(plane.n, p) + plane.d <= 0.f)
				{
					isSeperated = false;
					break;
				}
			}
			if (isSeperated) return false;
		}

		const Plane* closestPlane = nullptr;
		float closestAvgD = -std::numeric_limits<float>::max();
		Vec3 hitPos;
		bool flip = true;

		// find hit normal and position
		for (const Plane& plane : faces)
		{
			float avgDist = 0.f;
			int c = 0;
			Vec3 pos{};

			for (const Vec3& p : othVertices)
			{
				float f = dot(plane.n, p) + plane.d;
				if (f < 0.f)
				{
					avgDist += f;
					++c;
					pos += p;
				}
			}
			// since d is negative the largest one is close to zero
			if (avgDist > closestAvgD)
			{
				closestPlane = &plane;
				closestAvgD = avgDist;
				hitPos = pos / c;
			}
		}

		// other way around
		for (const Plane& plane : othFaces)
		{
			float avgDist = 0.f;
			int c = 0;
			Vec3 pos{};

			for (const Vec3& p : vertices)
			{
				float f = dot(plane.n, p) + plane.d;
				if (f < 0.f)
				{
					avgDist += f;
					++c;
					pos += p;
				}
			}
			// since d is negative the largest one is close to zero
			if (avgDist > closestAvgD)
			{
				closestPlane = &plane;
				closestAvgD = avgDist;
				hitPos = pos / c;
				flip = false;
			}
		}

		_info.normal = (flip ? -1.f : 1.f) * closestPlane->n;
		_info.position = hitPos;
		return true;
	}

	// *************************************************************** //
	bool BoundingMesh::Intersects(const ei::Sphere& _other, const ei::Mat4x4& _transform, HitInfo& _info) const
	{
		Vec3 center = _transform * _other.center;

		std::vector<Vec3> hits;

		for (const Plane& plane : faces)
		{
			if (dot(plane.n, center) + plane.d - _other.radius >= 0.f)
				return false;
		}

		_info.normal = normalize(Vec3(0.f) - center);
		_info.position = _info.normal * _other.radius;
		return true;
	}

	// *************************************************************** //
	CollisionComponent::CollisionComponent(Actor& _actor, float _boundingRadius, 
		const ei::Box& _boundingBox, uint32_t _type)
		: ActorComponent(_actor),
		m_boundingRadius(_boundingRadius),
		m_boundingRadiusSq(_boundingRadius*_boundingRadius),
		m_AABBOrigin(_boundingBox),
		m_AABB(m_AABBOrigin),
		m_type(_type),
		m_boundingMesh(Content::GetBoundingMesh(std::make_pair(_boundingBox.min,_boundingBox.max)))
	{
		ei::Vec3 v = _boundingBox.max - _boundingBox.min;
		m_volume = v.x * v.y * v.z;
	}

	CollisionComponent::CollisionComponent(Actor& _actor, float _boundingRadius, uint32_t _type)
		: ActorComponent(_actor),
		m_boundingRadius(_boundingRadius),
		m_boundingRadiusSq(_boundingRadius*_boundingRadius),
		m_AABBOrigin(Vec3(-_boundingRadius), Vec3(_boundingRadius)),
		m_AABB(m_AABBOrigin),
		m_type(_type),
		m_boundingMesh(Content::GetBoundingMesh()),
		m_isSimple(true)
	{

	}

	// *************************************************************** //
	CollisionComponent::CollisionComponent(Actor& _actor, const std::string& _name, uint32_t _type)
		:ActorComponent(_actor),
		m_boundingMesh(Content::GetBoundingMesh(_name)),
		m_type(_type)
	{
		ei::Vec3 v = m_boundingMesh.upperBound - m_boundingMesh.lowerBound;
		m_volume = v.x * v.y * v.z;
		m_boundingRadius = m_boundingMesh.boundingRadius;
		m_boundingRadiusSq = m_boundingMesh.boundingRadius * m_boundingMesh.boundingRadius;

		m_AABBOrigin.min = m_boundingMesh.lowerBound;
		m_AABBOrigin.max = m_boundingMesh.upperBound;
		m_AABB = m_AABBOrigin;
	}

	// *************************************************************** //
	CollisionComponent::CollisionComponent(Actor& _actor, const CollisionComponent& _orig)
		: ActorComponent(_actor),
		m_boundingRadius(_orig.m_boundingRadius),
		m_boundingRadiusSq(_orig.m_boundingRadiusSq),
		m_AABBOrigin(_orig.m_AABBOrigin),
		m_AABB(_orig.m_AABB),
		m_type(_orig.m_type),
		m_volume(_orig.m_volume),
		m_boundingMesh(_orig.m_boundingMesh),
		m_isSimple(_orig.m_isSimple)
	{}

	// *************************************************************** //
	void CollisionComponent::SetBoundingRadius(float _radius)
	{
		m_boundingRadius = _radius;
		m_boundingRadiusSq = _radius * _radius;
		m_AABBOrigin = Box(Vec3(-_radius), Vec3(_radius));
		m_AABB = m_AABBOrigin;
	}

	// *************************************************************** //
	void CollisionComponent::ProcessComponent(float _deltaTime)
	{
		OBox box(m_actor.GetRotation(), m_AABBOrigin);
		m_AABB = Box(box);
		m_AABB.min += m_actor.GetPosition();
		m_AABB.max += m_actor.GetPosition();
	}


	// *************************************************************** //
	bool CollisionComponent::Check(const CollisionComponent& _other, HitInfo& _info) const
	{
		bool b;
		
		if (m_isSimple)
		{
			// this will cause the normal to be flipped
			// todo: fix this
			if (!_other.m_isSimple) return _other.Check(*this, _info);
			// both are spheres, already done in pre pass
			// todo: calc hit info and perform check when pre pass uses different technique
			else return true;
		}
		else
		{
			Mat4x4 transform = m_actor.GetInverseTransformation() * _other.m_actor.GetTransformation();
			if (_other.m_isSimple) b = m_boundingMesh.Intersects(ei::Sphere(Vec3(0.f), _other.GetBoundingRadius()), transform, _info);
			else b = m_boundingMesh.Intersects(_other.m_boundingMesh, transform, _info);
		}

		// transform _info to global space
		_info.normal = Vec3(m_actor.GetTransformation() * Vec4(_info.normal, 0.f));
		_info.position = m_actor.GetTransformation() * _info.position;

		return b;
	}

	// *************************************************************** //
	bool CollisionComponent::RayCastFast(const ei::Ray& _ray, float& _distance) const
	{
		Sphere sphere(m_actor.GetPosition(), m_boundingRadius);

		return intersects(sphere, _ray, _distance);
	}

	bool CollisionComponent::RayCast(const ei::Ray& _ray, float& _distance) const
	{
		if (m_isSimple) return RayCastFast(_ray, _distance);

		Ray ray(m_actor.GetInverseTransformation() * _ray.origin, normalize(m_actor.GetInverseRotationMatrix() * _ray.direction));

		for (auto& triangle : m_boundingMesh.triangles)
		{
			if (intersects(triangle, ray, _distance))
				return true;
		}

		return false;
	}

	// *************************************************************** //
	Mat3x3 CollisionComponent::ComputeInertiaTensor(float _mass) const
	{
		// calculate inertia tensor
		const ei::Vec3& min = m_boundingMesh.lowerBound;
		const ei::Vec3& max = m_boundingMesh.upperBound;
		float h = max.y - min.y; h *= h;
		float d = max.z - min.z; d *= d;
		float w = max.x - min.x; w *= w;
		ei::Mat3x3 inertiaTensor = ei::identity3x3();
		inertiaTensor.m00 = 1.f / 12.f * _mass * (h + d);
		inertiaTensor.m11 = 1.f / 12.f * _mass * (w + d);
		inertiaTensor.m22 = 1.f / 12.f * _mass * (w + h);

		return inertiaTensor;
	}

	// *************************************************************** //
#define SOUND(name) &Content::GetSound(name)

	const clunk::Sample* CollisionComponent::GetMaterialSound(const CollisionComponent& _other)
	{
		static const std::array< const clunk::Sample*, static_cast<size_t>(CollisionComponent::Material::COUNT)
			* static_cast<size_t>(CollisionComponent::Material::COUNT)> SOUNDS =
		{
			//			none, metal, rock
			/*none*/	nullptr, nullptr, nullptr,
			/*metal*/	nullptr, SOUND("collision01"), SOUND("collision01"),
			/*rock*/	nullptr, SOUND("collision01"), SOUND("collision01")
		};

		return SOUNDS[static_cast<size_t>(m_material)
			+ static_cast<size_t>(_other.m_material) * static_cast<size_t>(CollisionComponent::Material::COUNT)];
	}
}