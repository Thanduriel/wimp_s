#include "mainstate.hpp"
#include "../graphic/mesh.hpp"
#include "../graphic/core/device.hpp"
#include "../graphic/resources.hpp"

namespace GameStates {

	void MainState::Process(float _deltaTime)
	{

	}

	void MainState::Draw(float _deltaTime)
	{
		static Graphic::Mesh mesh;

		Graphic::Device::SetEffect(Graphic::Resources::GetEffect(Graphic::Effects::MESH));
		mesh.Draw();
	}
}