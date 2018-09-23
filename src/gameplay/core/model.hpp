#pragma once

#include "actor.hpp"
#include "component.hpp"
#include "graphic/mesh.hpp"
#include "gameplay/elements/collisioncomponent.hpp"

namespace Game {

	class GeometryComponent : public ConstActorComponent
	{
	public:
		GeometryComponent(const Actor& _actor, const std::string& _pFile, const std::string& _textureName = "");
		GeometryComponent(const Actor& _actor, const GeometryComponent& _orig);

		void Draw() const;

		const Graphic::Mesh& GetMesh() const { return *m_mesh; }
	private:
		const Graphic::Mesh* m_mesh;
	};

	namespace Details {

		class ModelComponentImpl : public GeometryComponent, public CollisionComponent
		{
		public:
			// different order as both have the same signature
			ModelComponentImpl(const std::string& _meshName, Actor& _actor, const std::string& _textureName = "");
			ModelComponentImpl(Actor& _actor, const std::string& _meshName, const std::string& _boundingMeshName);
			ModelComponentImpl(Actor& _actor, const ModelComponentImpl& _other);
		private:
		};
	}

	typedef CompositeComponent<Details::ModelComponentImpl, GeometryComponent, CollisionComponent> ModelComponent;

	/* Model *********************************************
	 * A movable object with an textured mesh attached to it.
	 */
	class Model : public DynamicActor
	{
	public:
		// @param _pFile is the name of the mesh file without ending
		Model(const std::string& _pFile, const ei::Vec3&_position, const ei::Quaternion&_rotation, float _mass = 1.f, const std::string& _textureName = "");
		Model(const std::string& _meshName, const std::string& _boundingMeshName, const ei::Vec3&_position, const ei::Quaternion& _rotation, float _mass = 1.f);
		Model(const Model& _orig);

		void RegisterComponents(class SceneGraph& _sceneGraph) override;
		GeometryComponent& GetGeometryComponent() { return m_component; }
		CollisionComponent& GetCollisionComponent() { return m_component; }
		const CollisionComponent& GetCollisionComponent() const { return m_component; }
	protected:
		ModelComponent m_component;
	};
}