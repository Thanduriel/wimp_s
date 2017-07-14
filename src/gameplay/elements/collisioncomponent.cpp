#include "collisioncomponent.hpp"
#include "math/extensions.hpp"
#include "ei/3dintersection.hpp"
#include <functional>

namespace Game {

	using namespace ei;

	BoundingMesh::BoundingMesh(const std::string& _fileName)
	{
		Utils::MeshLoader loader;
		std::string texName;
		loader.Load(_fileName, *this, texName, 
			std::bind(&BoundingMesh::Load, this, std::placeholders::_1, std::placeholders::_2));
	}

	void BoundingMesh::Load(std::ifstream& _stream, size_t _numVertices)
	{
		size_t numFaces;
		Utils::MeshLoader::Read(_stream, numFaces);
		vertices.resize(_numVertices);
		_stream.read(reinterpret_cast<char*>(&vertices.front()), _numVertices * sizeof(Vec3));
		faces.reserve(numFaces);
		supportVectors.reserve(numFaces);
		triangles.reserve(numFaces);
		for (size_t i = 0; i < numFaces; ++i)
		{
			IVec3 indices;
			Utils::MeshLoader::Read(_stream, indices);
		}
	}

	// *************************************************************** //
	bool BoundingMesh::Intersects(const BoundingMesh& _other, const Mat4x4& _transform, HitInfo& _info) const
	{
		// transform one mesh
		std::vector<Vec3> othVertices(_other.vertices.size());
		for (size_t i = 0; i < othVertices.size(); ++i)
			othVertices[i] = Vec3(_transform * Vec4(_other.vertices[i], 1.f));

		std::vector<Plane> othFaces;
		othFaces.reserve(_other.faces.size());
		for (size_t i = 0; i < _other.faces.size(); ++i)
		{
			// should already be normalized, but the transformation takes away precision
			othFaces.emplace_back(normalize(Vec3(_transform * Vec4(_other.faces[i].n,0.f))),
				_transform * _other.supportVectors[i]);
		}

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
			if (isSeperated) return false;
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
	CollisionComponent::CollisionComponent(Actor& _actor, float _boundingRadius, 
		const ei::Box& _boundingBox)
		: ActorComponent(_actor),
		m_boundingRadius(_boundingRadius),
		m_boundingRadiusSq(_boundingRadius*_boundingRadius),
		m_type(Type::Any)
	{
		ei::Vec3 v = _boundingBox.max - _boundingBox.min;
		m_volume = v.x * v.y * v.z;

		// create bounding mesh
		// todo generate this from mesh
		m_boundingMesh.vertices.reserve(8);
		const Vec3& min = _boundingBox.min;
		const Vec3& max = _boundingBox.max;
		Vec3 center = (min + max) * 0.5f;
		m_boundingMesh.vertices.push_back(min);
		m_boundingMesh.vertices.push_back(Vec3(min.x, min.y, max.z));
		m_boundingMesh.vertices.push_back(Vec3(max.x, min.y, min.z));
		m_boundingMesh.vertices.push_back(Vec3(min.x, max.y, min.z));
		m_boundingMesh.vertices.push_back(max);
		m_boundingMesh.vertices.push_back(Vec3(max.x, max.y, min.z));
		m_boundingMesh.vertices.push_back(Vec3(min.x, max.y, max.z));
		m_boundingMesh.vertices.push_back(Vec3(max.x, min.y, max.z));

		m_boundingMesh.faces.reserve(6);
		m_boundingMesh.supportVectors.reserve(6);
		m_boundingMesh.supportVectors.push_back(Vec3(center.x, center.y, min.z));
		m_boundingMesh.faces.emplace_back(Vec3(0.f, 0.f, -1.f), m_boundingMesh.supportVectors.back());
		
		m_boundingMesh.supportVectors.push_back(Vec3(center.x, center.y, max.z));
		m_boundingMesh.faces.emplace_back(Vec3(0.f, 0.f, 1.f), m_boundingMesh.supportVectors.back());

		m_boundingMesh.supportVectors.push_back(Vec3(min.x, center.y, center.z));
		m_boundingMesh.faces.emplace_back(Vec3(-1.f, 0.f, 0.f), m_boundingMesh.supportVectors.back());

		m_boundingMesh.supportVectors.push_back(Vec3(max.x, center.y, center.z));
		m_boundingMesh.faces.emplace_back(Vec3(1.f, 0.f, 0.f), m_boundingMesh.supportVectors.back());

		m_boundingMesh.supportVectors.push_back(Vec3(center.x, min.y, center.z));
		m_boundingMesh.faces.emplace_back(Vec3(0.f, -1.f, 0.f), m_boundingMesh.supportVectors.back());

		m_boundingMesh.supportVectors.push_back(Vec3(center.x, max.y, center.z));
		m_boundingMesh.faces.emplace_back(Vec3(0.f, 1.f, 0.f), m_boundingMesh.supportVectors.back());

		const Vec3 minxy = Vec3(min.x, min.y, max.z);
		const Vec3 minxz = Vec3(min.x, max.y, min.z);
		const Vec3 minyz = Vec3(max.x, min.y, min.z);
		const Vec3 maxxy = Vec3(max.x, max.y, min.z);
		const Vec3 maxxz = Vec3(max.x, min.y, max.z);
		const Vec3 maxyz = Vec3(min.x, max.y, max.z);
		m_boundingMesh.triangles.emplace_back(min, minyz, maxxy);
		m_boundingMesh.triangles.emplace_back(min, maxxy, minxz);
		m_boundingMesh.triangles.emplace_back(min, minxy, maxyz);
		m_boundingMesh.triangles.emplace_back(min, minxz, maxyz);
		m_boundingMesh.triangles.emplace_back(min, minyz, maxxz);
		m_boundingMesh.triangles.emplace_back(min, minxy, maxxz);

		m_boundingMesh.triangles.emplace_back(max, maxyz, minxy);
		m_boundingMesh.triangles.emplace_back(max, minxy, maxxz);
		m_boundingMesh.triangles.emplace_back(max, maxxy, minyz);
		m_boundingMesh.triangles.emplace_back(max, maxxz, minyz);
		m_boundingMesh.triangles.emplace_back(max, maxyz, minxz);
		m_boundingMesh.triangles.emplace_back(max, maxxy, minxz);
	}

	// *************************************************************** //
	CollisionComponent::CollisionComponent(Actor& _actor, const CollisionComponent& _orig)
		: ActorComponent(_actor),
		m_boundingRadius(_orig.m_boundingRadius),
		m_boundingRadiusSq(_orig.m_boundingRadiusSq),
		m_type(Type::Any),
		m_volume(_orig.m_volume),
		m_boundingMesh(_orig.m_boundingMesh)
	{}

	// *************************************************************** //
	bool CollisionComponent::Check(const CollisionComponent& _other, HitInfo& _info)
	{
		Mat4x4 transform = m_actor.GetInverseTransformation() * _other.m_actor.GetTransformation();

		bool b = m_boundingMesh.Intersects(_other.m_boundingMesh, transform, _info);

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
		Ray ray(m_actor.GetInverseTransformation() * _ray.origin, normalize(m_actor.GetInverseRotationMatrix() * _ray.direction));

		for (auto& triangle : m_boundingMesh.triangles)
		{
			if (intersects(triangle, ray, _distance))
				return true;
		}

		return false;
	}
}