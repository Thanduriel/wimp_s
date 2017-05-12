#pragma once

#include "actor.hpp"
#include "graphic/mesh.hpp"

namespace Game {

	/* Model *********************************************
	 * A movable object with an textured mesh attached to it.
	 */
	class Model : public DynamicActor, Graphic::Mesh
	{
	public:
		// just for testing
		using DynamicActor::DynamicActor;

		void Draw() override;
	};
}