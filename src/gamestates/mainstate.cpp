#include "mainstate.hpp"
#include "../graphic/mesh.hpp"
#include "../graphic/core/device.hpp"
#include "../graphic/resources.hpp"
#include "gameplay/core/model.hpp"
#include "control/camera.hpp"
#include "control/playercontroller.hpp"
#include "graphic/interface/pixelcoords.hpp"

namespace GameStates {

	using namespace ei;
	using namespace Graphic;

	MainState::MainState()
	{
		Control::g_player = new Control::PlayerController(Vec3(0.f), qidentity());
		
		auto& label = m_hud.GetDebugLabel();
		label.SetText("Let there be strings!");

		ScreenOverlay* el = &m_hud.CreateScreenElement<ScreenTexture>("simpleWindow", PixelCoord(50, 100));
		el->Scale(Vec2(0.33f));
	}

	MainState::~MainState()
	{
	}

	void MainState::Process(float _deltaTime)
	{
		Control::g_camera.Process(_deltaTime);
		Control::g_player->Process(_deltaTime);
	}

	void MainState::Draw(float _deltaTime)
	{
		Graphic::Device::SetEffect(Graphic::Resources::GetEffect(Graphic::Effects::MESH));
		Control::g_player->Draw();

		m_hud.Draw(_deltaTime);
	}
}