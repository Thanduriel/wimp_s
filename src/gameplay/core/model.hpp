#pragma once

#include "actor.hpp"
#include "component.hpp"
#include "graphic/mesh.hpp"

namespace Game {

	class GeometryComponent : public ConstActorComponent
	{
	public:
		GeometryComponent(const Actor& _actor, const std::string& _pFile);

		void Draw();

	private:
		const Graphic::Mesh* m_mesh;
	};

	/* Model *********************************************
	 * A movable object with an textured mesh attached to it.
	 */
	class Model : public DynamicActor, public GeometryComponent
	{
	public:
		Model(const std::string& _pFile, const ei::Vec3&_position, const ei::Quaternion&_rotation);

		void RegisterComponents(class SceneGraph& _sceneGraph) override;
	};
}