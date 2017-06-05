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
#include "gameplay/elements/blackhole.hpp"

namespace GameStates {

	using namespace ei;
	using namespace std;
	using namespace Graphic;
	using namespace Control;
	using namespace Game;

	//Actor vector (lol rhyme)
	vector<unique_ptr<Actor>> actors;

	MainState::MainState()
		: m_starbackground(2000, 20000.f, 14000)
	{
		unique_ptr<BlackHole> blackHole(new BlackHole(ei::Vec3(-5.f), ei::qidentity()));
		unique_ptr<Grid> grid(new Game::Grid(ei::Vec3(0.f), ei::qidentity(), Utils::Color32F(0.f, 1.f, 0.f, 0.6f), 2.f, 2.f));
		unique_ptr<Model> model(new Model("models/spaceship.fbx", Vec3(0.f), qidentity()));
		m_playerController = new Control::PlayerController(*model, *grid, *blackHole);
	//	Control::g_camera.Attach(*Control::g_player);

		unique_ptr<Model> model2(new Model("models/spaceship.fbx", Vec3(1.f, 0.f, 1.f), qidentity()));
		model2->SetAngularVelocity(Vec3(1.f));
		model2->SetVelocity(Vec3(1.f, 0.f, 1.f));

		auto& label = m_hud.GetDebugLabel();
		label.SetText("Let there be strings!");

		auto& btn0 = m_hud.CreateScreenElement<Graphic::Button>("menuBtn", Vec2(0.f, 0.f), Vec2(0.f),
			DefinitionPoint::BotRight, ScreenPosition::Anchor(DefinitionPoint::BotRight, &m_hud), "button");
		btn0.Scale(Vec2(0.65f));

		ScreenOverlay* el = &m_hud.CreateScreenElement<ScreenTexture>("simpleWindow", PixelCoord(50, 100));
		el->Scale(Vec2(0.33f));

		actors.push_back(move(model));
		actors.push_back(move(model2));
		actors.push_back(move(grid));
		actors.push_back(move(blackHole));
	}

	MainState::~MainState()
	{
		delete m_playerController;
	}

	void MainState::Process(float _deltaTime)
	{
		Control::g_camera.Process(_deltaTime);
		m_playerController->Process(_deltaTime);
		for (int i = 0; i < actors.size(); i++)
			actors[i]->Process(_deltaTime);
	}

	void MainState::Draw(float _deltaTime)
	{
		m_starbackground.Draw();

		Graphic::Device::SetEffect(Graphic::Resources::GetEffect(Graphic::Effects::MESH));

		for (int i = 0; i < actors.size(); i++)
			actors[i]->Draw();

		// render the framebuffer to the hardwarebackbuffer
		Texture& tex = *Device::GetCurrentFramebufferBinding()->GetColorAttachments().begin()->pTexture;
		Device::BindFramebuffer(nullptr);
		Device::SetEffect(Graphic::Resources::GetEffect(Effects::SCREEN_OUTPUT));
		Device::SetTexture(tex, 0);
		Device::DrawFullscreen();

		// the hud should be drawn last
		m_hud.GetDebugLabel().SetText("fps: <c 0 255 100><<" + std::to_string(_deltaTime) + "</c>");
		m_hud.Draw(_deltaTime);
	}

	void MainState::Dispose()
	{
		for (int i = 0; i < actors.size(); i++)
		{
			if (actors[i]->IsDestroyed())
			{
				swap(actors[i], actors.back());
				actors.resize(actors.size() - 1);
			}
		}
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