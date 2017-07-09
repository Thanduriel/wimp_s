#pragma once

#include "gameplay/core/component.hpp"
#include "ei/3dtypes.hpp"
#include "ei/vector.hpp"

namespace Game {

	struct HitInfo
	{
		ei::Vec3 normal;
		ei::Vec3 position;
	};

	// Convex polyhedron to check intersection with
	class BoundingMesh
	{
	public:
		std::vector<ei::Vec3> vertices;
		std::vector<ei::Plane> faces;
		std::vector<ei::Vec3> supportVectors; //< center of the face

		bool Intersects(const BoundingMesh& _other, const ei::Mat4x4& _transform, HitInfo& _info) const;
	};

	// While the collision component itself does not change the Actor
	// the solver needs to alter the Actors state through the component.
	class CollisionComponent : public ActorComponent
	{
	public:
		CollisionComponent(Actor& _actor, float _boundingRadius, const ei::Box& _boundingBox);
		CollisionComponent(Actor& _actor, const CollisionComponent& _other);

		bool Check(const CollisionComponent& _other, HitInfo& _info);

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
	private:
		float m_volume;
		float m_boundingRadius;
		float m_boundingRadiusSq;
		Type m_type;
		BoundingMesh m_boundingMesh;
	};
}