#include "mainstate.hpp"
#include "../graphic/mesh.hpp"
#include "../graphic/core/device.hpp"
#include "../graphic/resources.hpp"
#include "gameplay/core/model.hpp"
#include "control/camera.hpp"
#include "control/playercontroller.hpp"
#include "control/input.hpp"

#include "graphic/interface/pixelcoords.hpp"
#include "gameplay/elements/grid.hpp"

namespace GameStates {

	using namespace ei;
	using namespace Graphic;
	using namespace Control;
	using namespace Game;

	// test stuff
	Game::Grid* grid;
	Game::Model* model;
	Game::Model* model2;

	MainState::MainState()
		: m_starbackground(2000, 20000.f, 14000)
	{
		grid = new Game::Grid(ei::Vec3(0.f), ei::qidentity(), Utils::Color32F(0.f, 1.f, 0.f, 0.6f), 2.f, 2.f);
		model = new Model(Vec3(0.f), qidentity());
		m_playerController = new Control::PlayerController(*model, *grid);
	//	Control::g_camera.Attach(*Control::g_player);

		model2 = new Model(Vec3(1.f,0.f,1.f), qidentity());

		auto& label = m_hud.GetDebugLabel();
		label.SetText("Let there be strings!");

		auto& btn0 = m_hud.CreateScreenElement<Graphic::Button>("menuBtn", Vec2(0.f, 0.f), Vec2(0.f),
			DefinitionPoint::BotRight, ScreenPosition::Anchor(DefinitionPoint::BotRight, &m_hud), "button");
		btn0.Scale(Vec2(0.65f));

		ScreenOverlay* el = &m_hud.CreateScreenElement<ScreenTexture>("simpleWindow", PixelCoord(50, 100));
		el->Scale(Vec2(0.33f));
	}

	MainState::~MainState()
	{
		delete m_playerController;

		// test stuff
		delete model;
		delete model2;
		delete grid;
	}

	void MainState::Process(float _deltaTime)
	{
		Control::g_camera.Process(_deltaTime);
		m_playerController->Process(_deltaTime);
		model->Process(_deltaTime);
	}

	void MainState::Draw(float _deltaTime)
	{
		m_starbackground.Draw();

		Graphic::Device::SetEffect(Graphic::Resources::GetEffect(Graphic::Effects::MESH));
		model->Draw();
		model2->Draw();
		grid->Draw();

		m_hud.GetDebugLabel().SetText(std::to_string(_deltaTime));
		m_hud.Draw(_deltaTime);
	}

	// ******************************************************* //
	void MainState::MouseMove(float _dx, float _dy)  
	{ 
		m_hud.MouseMove(_dx, _dy);

		m_playerController->MouseMove(_dx, _dy);
	}
	void MainState::Scroll(float _dx, float _dy)  
	{ 
		if (m_hud.Scroll(_dx, _dy)) return;

		m_playerController->Scroll(_dx, _dy);
	}
	void MainState::KeyDown(int _key, int _modifiers)  
	{ 
		if(m_hud.KeyDown(_key, _modifiers)) return; 
	}
	void MainState::KeyRelease(int _key)  
	{ 
		if(m_hud.KeyUp(_key, 0)) return; 
	}
	void MainState::KeyClick(int _key)  
	{ 
//		if(m_hud.KeyClick(_key)) return; 
		m_playerController->KeyClick(_key);
	}
	void MainState::KeyDoubleClick(int _key)  
	{ 
//		if (m_hud.KeyDoubleClick(_key)) return;
	}
}