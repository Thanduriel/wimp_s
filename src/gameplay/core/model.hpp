#pragma once

#include "actor.hpp"
#include "graphic/mesh.hpp"

namespace Game {

	using namespace ei;

	/* Model *********************************************
	 * A movable object with an textured mesh attached to it.
	 */
	class Model : public DynamicActor, Graphic::Mesh
	{
	public:

		
		Model(const std::string& _pFile, const Vec3 &_position, const Quaternion &_rotation) : DynamicActor(_position, _rotation), Mesh(_pFile) {};
		

		void Draw() override;
	};
}