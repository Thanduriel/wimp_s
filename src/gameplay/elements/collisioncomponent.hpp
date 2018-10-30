#pragma once

#include "gameplay/core/component.hpp"
#include "ei/3dtypes.hpp"
#include "ei/vector.hpp"
#include "utils/meshloader.hpp"

namespace clunk{
	class Sample;
}

namespace Game {

	struct HitInfo
	{
		ei::Vec3 normal;
		ei::Vec3 position;
	};

	// Convex polyhedron to check intersection with
	struct BoundingMesh : public Utils::MeshInfo
	{
		BoundingMesh() = default;
		// create a bounding mesh from a simple box.
		BoundingMesh(const ei::Box& _box);
		BoundingMesh(const std::string& _fileName);

		std::vector<ei::Vec3> vertices;
		std::vector<ei::Plane> faces;
		std::vector<ei::Vec3> supportVectors; //< center of the face
		std::vector<ei::Triangle> triangles;

		void Load(std::ifstream& _stream, size_t _numVertices);

		bool Intersects(const BoundingMesh& _other, const ei::Mat4x4& _transform, HitInfo& _info) const;
		bool Intersects(const ei::Sphere& _other, const ei::Mat4x4& _transform, HitInfo& _info) const;
	};

	typedef uint32_t CollisionFlags;

	// While the collision component itself does not change the Actor
	// the solver needs to alter the Actors state through the component.
	class CollisionComponent : public ActorComponent
	{
	public:
		CollisionComponent(Actor& _actor, float _boundingRadius, const ei::Box& _boundingBox, uint32_t _type);
		// Create a simple CollisionComponent that uses only the bounding sphere.
		CollisionComponent(Actor& _actor, float _boundingRadius, uint32_t _type);
		// Create a CollisionComponent from a BoundingMesh
		CollisionComponent(Actor& _actor, const std::string& _name, uint32_t _type);
		CollisionComponent(Actor& _actor, const CollisionComponent& _other);

		void ProcessComponent(float _deltaTime) override;

		bool Check(const CollisionComponent& _other, HitInfo& _info) const;
		// ray cast with the bounding sphere
		bool RayCastFast(const ei::Ray& _ray, float& _distance) const;
		// ray cast with the bounding mesh
		bool RayCast(const ei::Ray& _ray, float& _distance) const;

		// Overwrites bounding radius and depended internal structures.
		// Use only on simple collision components.
		void SetBoundingRadius(float _radius);
		float GetBoundingRadius() const { return m_boundingRadius; }
		float GetBoundingRadiusSq() const { return m_boundingRadiusSq; }
		const BoundingMesh& GetBoundingMesh() const { return m_boundingMesh; }
		float GetVolume() const { return m_volume; }

		// Restricts collisions and raycasts with this component.
		// A CollisionComponent with the Any bit set will be considered for intersection.
		// Only if both Objects are solid physical resolve is done.
		struct Type{
			enum Value
			{
				Any = 1,
				Solid = 2,
				Dynamic = 4,
				Ship = 8,
				Projectile = 16,
				Zone = 32, // trigger volumes that do not cause physical reactions
				Hostile = 64
			};
		};
		CollisionFlags GetType() const { return m_type; }
		void SetType(CollisionFlags _type) { m_type = _type; }

		ei::Mat3x3 ComputeInertiaTensor(float _mass) const;

		enum struct Material
		{
			None,
			Metal,
			Rock,
			COUNT
		};
		Material GetMaterial() const { return m_material; }
		void SetMaterial(Material _material) { m_material = _material; }

		const clunk::Sample* GetMaterialSound(const CollisionComponent& _other);

	private:
		float m_volume;
		float m_boundingRadius;
		float m_boundingRadiusSq;
		ei::Box m_AABBOrigin; // size of the axis aligned bounding box
		ei::Box m_AABB; // actual box in world space
		float m_minOfAllMin; // smallest x value of all CollisionComponents that have a larger x max then this.
		uint32_t m_type;
		Material m_material = Material::None;
		const BoundingMesh& m_boundingMesh;
		bool m_isSimple = false; // this component can be approximated by its bounding sphere

		friend class SceneGraph; // the SceneGraph handles all intersection queries
	};
}