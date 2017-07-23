#pragma once

#include "gameplay/core/component.hpp"
#include "ei/3dtypes.hpp"
#include "ei/vector.hpp"
#include "utils/meshloader.hpp"

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

	// While the collision component itself does not change the Actor
	// the solver needs to alter the Actors state through the component.
	class CollisionComponent : public ActorComponent
	{
	public:
		CollisionComponent(Actor& _actor, float _boundingRadius, const ei::Box& _boundingBox);
		// Create a simple CollisionComponent that uses only the bounding sphere.
		CollisionComponent(Actor& _actor, float _boundingRadius);
		// Create a CollisionComponent from a BoundingMesh
		CollisionComponent(Actor& _actor, const std::string& _name);
		CollisionComponent(Actor& _actor, const CollisionComponent& _other);

		bool Check(const CollisionComponent& _other, HitInfo& _info) const;
		// ray cast with the bounding sphere
		bool RayCastFast(const ei::Ray& _ray, float& _distance) const;
		// ray cast with the bounding mesh
		bool RayCast(const ei::Ray& _ray, float& _distance) const;

		float GetBoundingRadius() const { return m_boundingRadius; }
		float GetBoundingRadiusSq() const { return m_boundingRadiusSq; }
		const BoundingMesh& GetBoundingMesh() const { return m_boundingMesh; }
		float GetVolume() const { return m_volume; }

		// Restricts which sets of components can collide with each other.
		// x indicates that the types can collide.
		//     Any NP
		// Any  x  x
		// NP   x  -
		enum struct Type 
		{
			Any,
			NonPlayer
		};
		Type GetType() const { return m_type; }
		void SetType(Type _type) { m_type = _type; }

		ei::Mat3x3 ComputeInertiaTensor(float _mass) const;
	private:
		float m_volume;
		float m_boundingRadius;
		float m_boundingRadiusSq;
		Type m_type;
		const BoundingMesh& m_boundingMesh;
		bool m_isSimple = false; // this component can be approximated by its bounding sphere
	};
}