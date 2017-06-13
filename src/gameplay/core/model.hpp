#pragma once

#include "actor.hpp"
#include "../elements/component.hpp"
#include "graphic/mesh.hpp"

namespace Game {

	class GeometryComponent : ActorComponent, Graphic::Mesh
	{
	public:
		GeometryComponent(Actor& _actor, const std::string& _pFile);

		void Draw();
	};

	/* Model *********************************************
	 * A movable object with an textured mesh attached to it.
	 */
	class Model : public DynamicActor, public GeometryComponent
	{
	public:
		Model(const std::string& _pFile, const ei::Vec3&_position, const ei::Quaternion&_rotation);
	};
}