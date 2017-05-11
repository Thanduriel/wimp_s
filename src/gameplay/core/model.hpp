#pragma once

#include "actor.hpp"
#include "graphic/mesh.hpp"

namespace Game {

	class Model : public DynamicActor, Graphic::Mesh
	{
	public:
		using DynamicActor::DynamicActor;

		void Draw() override;
	};
}