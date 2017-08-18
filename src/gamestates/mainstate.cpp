#include "mainstate.hpp"
#include "inventorystate.hpp"
#include "pausestate.hpp"
#include "../graphic/mesh.hpp"
#include "gameplay/core/model.hpp"
#include "gameplay/elements/ship.hpp"
#include "control/camera.hpp"
#include "control/playercontroller.hpp"
#include "control/waspcontroller.hpp"
#include "control/kamikazecontroller.hpp"
#include "control/turtlecontroller.hpp"
#include "control/input.hpp"
#include "graphic/effects/lightsystem.hpp"
#include "control/controller.hpp"
// test related
#include "graphic/interface/pixelcoords.hpp"
#include "gameplay/elements/grid.hpp"
#include "gameplay/elements/blackhole.hpp"
#include "gameplay/elements/light.hpp"
#include "generators/random.hpp"
#include "gameplay/elements/shipsystems/projectile.hpp"
#include "gameplay/events/event.hpp"
#include "gameplay/events/conditions.hpp"
#include "gameplay/events/01_intro/act01.hpp"

namespace GameStates {

	using namespace ei;
	using namespace std;
	using namespace Graphic;
	using namespace Control;
	using namespace Game;

	Game::Ship* ship2;

	MainState::MainState()
		: m_starbackground(2000, 20000.f, 14000),
		m_gameTimeControl{1.f}
	{
		Controller::SetSceneGraph(m_sceneGraph);

		// setup player controller
		Ship* ship = new Ship("TestShip", Vec3(0.f));
		m_sceneGraph.Add(*ship);
		m_playerController = new Control::PlayerController(*ship, m_hud, m_gameTimeControl);
		m_sceneGraph.RegisterComponent(*m_playerController);
		Control::g_camera.Attach(*ship);

		Acts::Act01 act1(*ship, m_hud);

		// test actors
		ship = new Ship("TestShip", Vec3(50.f,0.f,100.f));
		m_sceneGraph.Add(*ship);
		m_sceneGraph.RegisterComponent(*new Control::TurtleController(*ship, m_playerController->GetShip().GetHandle(), m_hud));
		ship2 = ship;

	}

	MainState::~MainState()
	{
	}

	void MainState::Process(float _deltaTime)
	{
	//	ei::Ray ray(Vec3(m_playerController->GetShip().GetPosition()),
	//		normalize(m_playerController->GetShip().GetRotationMatrix() * Vec3(0.f, 0.f, 1.f)));
	//	ray.origin += ray.direction * (m_playerController->GetShip().GetCollisionComponent().GetBoundingRadius()+1.f);

	//	ship2->SetPosition(m_playerController->GetShip().GetPosition());
	//	ship2->SetRotation(m_playerController->GetShip().GetRotation());

		m_sceneGraph.Process(m_gameTimeControl.m_timeScale * _deltaTime, _deltaTime);

		Control::g_camera.Process(_deltaTime);
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
		if (InputManager::IsVirtualKey(_key, Control::VirtualKey::INVENTORY))
		{
			// Change to inventory state
			m_newState = new GameStates::InventoryState(m_playerController->GetShip());
		}

		if (InputManager::IsVirtualKey(_key, Control::VirtualKey::PAUSE))
		{
			m_newState = new GameStates::PauseState(*this);
		}
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