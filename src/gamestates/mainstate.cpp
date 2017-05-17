#include "mainstate.hpp"
#include "../graphic/mesh.hpp"
#include "../graphic/core/device.hpp"
#include "../graphic/resources.hpp"
#include "gameplay/core/model.hpp"
#include "control/camera.hpp"
#include "control/playercontroller.hpp"

namespace GameStates {

	using namespace ei;

	MainState::MainState()
	{
	}

	MainState::~MainState()
	{
	}

	void MainState::Process(float _deltaTime)
	{
		Control::g_camera.Process(_deltaTime);
		Control::g_player.Process(_deltaTime);
	}

	void MainState::Draw(float _deltaTime)
	{
		Graphic::Device::SetEffect(Graphic::Resources::GetEffect(Graphic::Effects::MESH));
		//Control::g_player.Draw();
	}
}