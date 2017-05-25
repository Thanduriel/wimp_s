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
		: m_starbackground(2000, 20000.f, 14000)
	{
		Control::g_player = new Control::PlayerController(Vec3(0.f), qidentity());
	//	Control::g_camera.Attach(*Control::g_player);

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
		m_starbackground.Draw();

		Graphic::Device::SetEffect(Graphic::Resources::GetEffect(Graphic::Effects::MESH));
		Control::g_player->Draw();

		m_hud.GetDebugLabel().SetText(std::to_string(_deltaTime));
		m_hud.Draw(_deltaTime);
	}

	// ******************************************************* //
	void MainState::MouseMove(float _dx, float _dy)  
	{ 
		m_hud.MouseMove(_dx, _dy);

		Control::g_player->MouseMove(_dx, _dy);
	}
	void MainState::Scroll(float _dx, float _dy)  
	{ 
		if (m_hud.Scroll(_dx, _dy)) return;
	}
	void MainState::KeyDown(int _key, int _modifiers)  
	{ 
		if(m_hud.KeyDown(_key, _modifiers)) return; 
	}
	void MainState::KeyRelease(int _key)  
	{ 
//		if(m_hud.KeyRelease(_key)) return; 
	}
	void MainState::KeyClick(int _key)  
	{ 
//		if(m_hud.KeyClick(_key)) return; 
	}
	void MainState::KeyDoubleClick(int _key)  
	{ 
//		if (m_hud.KeyDoubleClick(_key)) return;
	}
}