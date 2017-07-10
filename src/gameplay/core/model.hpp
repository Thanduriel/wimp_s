#pragma once

#include "actor.hpp"
#include "component.hpp"
#include "graphic/mesh.hpp"
#include "gameplay/elements/collisioncomponent.hpp"

namespace Game {

	class GeometryComponent : public ConstActorComponent
	{
	public:
		GeometryComponent(const Actor& _actor, const std::string& _pFile);
		GeometryComponent(const Actor& _actor, const GeometryComponent& _orig);

		void Draw();

		const Graphic::Mesh& GetMesh() const { return *m_mesh; }
	private:
		const Graphic::Mesh* m_mesh;
	};

	/* Model *********************************************
	 * A movable object with an textured mesh attached to it.
	 */
	class Model : public DynamicActor
	{
	public:
		Model(const std::string& _pFile, const ei::Vec3&_position, const ei::Quaternion&_rotation);
		Model(const Model& _orig);

		void RegisterComponents(class SceneGraph& _sceneGraph) override;
		GeometryComponent& GetGeometryComponent() { return m_geometryComponent; }
		CollisionComponent& GetCollisionComponent() { return m_collisionComponent; }
		const CollisionComponent& GetCollisionComponent() const { return m_collisionComponent; }
	protected:
		GeometryComponent m_geometryComponent;
		CollisionComponent m_collisionComponent;
	};
}