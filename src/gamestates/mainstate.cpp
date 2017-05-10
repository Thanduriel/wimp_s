#include "mainstate.hpp"
#include "../graphic/mesh.hpp"
#include "../graphic/core/device.hpp"
#include "../graphic/resources.hpp"
#include "gameplay/core/model.hpp"
#include "control/camera.hpp"

namespace GameStates {

	void MainState::Process(float _deltaTime)
	{
	//	Control::g_camera.SetAngularVelocity(ei::Vec3(1.f));
		Control::g_camera.Translate(ei::Vec3(0.f, 0.f, _deltaTime));
		Control::g_camera.Process(_deltaTime);
	}

	void MainState::Draw(float _deltaTime)
	{
		static Game::Model model;

		Graphic::Device::SetEffect(Graphic::Resources::GetEffect(Graphic::Effects::MESH));
		model.Draw();
	}
}