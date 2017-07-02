#include "mainstate.hpp"
#include "../graphic/mesh.hpp"
#include "gameplay/core/model.hpp"
#include "gameplay/elements/ship.hpp"
#include "control/camera.hpp"
#include "control/playercontroller.hpp"
#include "control/input.hpp"
#include "graphic/effects/lightsystem.hpp"

#include "graphic/interface/pixelcoords.hpp"
#include "gameplay/elements/grid.hpp"
#include "gameplay/elements/blackhole.hpp"
#include "gameplay/elements/light.hpp"
#include "gameplay/elements/particlesystemcomponent.hpp"
#include "generators/random.hpp"
#include "gameplay/elements/shipsystems/projectile.hpp"

namespace GameStates {

	using namespace ei;
	using namespace std;
	using namespace Graphic;
	using namespace Control;
	using namespace Game;

	Game::PointLight* pointLight;
	Game::BlackHole* blackHole;
	Game::ParticleSystemActor<Graphic::ParticleSystems::BASIC_SYSTEM>* particleSystem;

	MainState::MainState()
		: m_starbackground(2000, 20000.f, 14000),
		m_gameTimeControl{1.f}
	{
		// setup player controller
		blackHole = new BlackHole(ei::Vec3(-15.f), 5.f);
		m_sceneGraph.Add(*blackHole);
		Grid* grid = new Game::Grid(ei::Vec3(0.f), 
			Utils::Color32F(0.f, 1.f, 0.f, 0.6f), 2.f, 2.f,
			50.f, GridComponent::TransitionInfo(4800.f, 0.25f));
		m_sceneGraph.Add(*grid);
		Ship* ship = new Ship("spaceship", Vec3(0.f), CollisionComponent::Type::NonPlayer);
		m_sceneGraph.Add(*ship);
		m_playerController = new Control::PlayerController(*ship, *grid, *blackHole, m_hud, m_gameTimeControl);
		Control::g_camera.Attach(*ship);

		ship = new Ship("spaceship", Vec3(50.f,0.f,100.f));
		m_sceneGraph.Add(*ship);

		// some test actors
		pointLight = new PointLight(Vec3(0.f), Vec3(0.f), 5.f, Utils::Color8U(255_uc, 255_uc, 0_uc));
		m_sceneGraph.Add(*pointLight);
	}

	MainState::~MainState()
	{
		delete m_playerController;
	}

	void MainState::Process(float _deltaTime)
	{
		m_sceneGraph.Process(m_gameTimeControl.m_timeScale * _deltaTime, _deltaTime);
		m_playerController->Process(_deltaTime);
		Control::g_camera.Process(_deltaTime);

		static float t = 0.f;
		t += _deltaTime;
		
		Vec4 pos = m_playerController->GetShip().GetTransformation() * Vec4(0.5f, 0.f, 0.f, 1.f);
	//	pointLight->SetPosition(m_playerController->GetShip().GetPosition() + Vec3(0.f,1.f,0.f));
		Vec3 p = ei::Vec3(cos(t), 0.f, sin(t));
		pointLight->SetPosition(p);
	}

	void MainState::Draw(float _deltaTime)
	{
		m_starbackground.Draw();

		m_sceneGraph.Draw();

		// the hud should be drawn last
		m_hud.GetDebugLabel().SetText("ft: <c 0 255 100>" + std::to_string(_deltaTime) + "</c>"
							+ "\nnumP" + std::to_string(ParticleSystems::Manager::GetNumParticlesTotal()));
		m_hud.Draw(_deltaTime);
	}

	void MainState::Dispose()
	{
		m_sceneGraph.CleanUp();
		m_sceneGraph.AddActors();
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

		m_playerController->KeyDown(_key, _modifiers);
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