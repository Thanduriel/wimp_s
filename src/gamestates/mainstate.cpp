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
#include "transitionstate.hpp"
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
	Game::Model* boundingMeshTest = nullptr;

	MainState::MainState(Game::Ship& _player)
		: m_starbackground(2000, 20000.f, 14000),
		m_gameTimeControl{1.f},
		m_playerShip(_player)
	{
		Controller::SetSceneGraph(m_sceneGraph);

		// setup player controller
		m_sceneGraph.Add(_player);
		m_playerController = new Control::PlayerController(_player, m_hud, m_gameTimeControl);
		m_sceneGraph.RegisterComponent(*m_playerController);

		//register weapons
		for (auto item : _player.GetInventory())
		{
			m_sceneGraph.Add(*static_cast<Weapon*>(item));
		}

		//boundingMeshTest = new Model("default_shipbm", Vec3(), qidentity());
		//boundingMeshTest->GetCollisionComponent().SetType(0x0); // no collision
	//	m_sceneGraph.Add(*boundingMeshTest);

		// test actors
/*		ship2 = new Ship("WaspShip", Vec3(50.f,0.f,500.f));
		m_sceneGraph.Add(*ship2);
		m_sceneGraph.RegisterComponent(*new Control::WaspController(*ship2, m_playerController->GetShip().GetHandle(), m_hud));*/
	}

	MainState::~MainState()
	{
		if (boundingMeshTest) delete boundingMeshTest;
		ParticleSystems::Manager::ClearAll();
	}

	void MainState::Process(float _deltaTime)
	{
		m_sceneGraph.Process(m_gameTimeControl.m_timeScale * _deltaTime, _deltaTime);
		//boundingMeshTest->SetPosition(m_playerController->GetShip().GetPosition());
		//boundingMeshTest->SetRotation(m_playerController->GetShip().GetRotation());

		Control::g_camera.Process(_deltaTime);

		if (m_map->IsFinished())
		{
			m_isFinished = true;
			RemovePlayer();
			m_newState = new TransitionState(m_map->GetNextLevel(), &m_playerShip);

			// clear all particles
			Graphic::ParticleSystems::Manager::ClearAll();
		}

		if (m_playerController)
		{
			if (m_playerShip.IsDestroyed())
			{
				m_playerController = nullptr;
				m_sceneGraph.Remove(m_playerShip);
				g_camera.FixRotation(g_camera.GetRotation(), g_camera.GetPosition());
				m_timeLeft = 7.f;
			}
		}
		else
		{
			m_timeLeft -= _deltaTime;
			if (m_timeLeft < 0.f)
			{
				m_isFinished = true;
				delete &m_playerShip;
			}
		}

		m_hud.Process(_deltaTime); // use real game time
	}

	void MainState::Draw(float _deltaTime)
	{
		m_starbackground.Draw();

		m_sceneGraph.Draw();
		// the hud should be drawn last
		m_hud.GetDebugLabel().SetText("ft: <c 0 255 100>" + std::to_string(_deltaTime) + "</c>"
							+ "\nnumP" + std::to_string(ParticleSystems::Manager::GetNumParticlesTotal()));
		if(m_playerController) m_hud.Draw(_deltaTime);
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

		if(m_playerController) m_playerController->MouseMove(_dx, _dy);
	}
	void MainState::Scroll(float _dx, float _dy)
	{ 
		if (m_hud.Scroll(_dx, _dy)) return;

		if (m_playerController) m_playerController->Scroll(_dx, _dy);
	}
	void MainState::KeyDown(int _key, int _modifiers)  
	{ 
		if(m_hud.KeyDown(_key, _modifiers)) return; 

		if (m_playerController) m_playerController->KeyDown(_key, _modifiers);
	}
	void MainState::KeyRelease(int _key)  
	{
		// cant manually change the state when game over
		if (m_playerController)
		{
			if (InputManager::IsVirtualKey(_key, Control::VirtualKey::INVENTORY))
			{
				// Change to inventory state
				m_newState = new GameStates::InventoryState(m_playerController->GetShip());
			}
			else if (InputManager::IsVirtualKey(_key, Control::VirtualKey::PAUSE))
			{
				m_newState = new GameStates::PauseState(*this);
			}
		}

		if(m_hud.KeyUp(_key, 0)) return; 
	}
	void MainState::KeyClick(int _key)  
	{ 
//		if(m_hud.KeyClick(_key)) return; 
		if (m_playerController) m_playerController->KeyClick(_key);
	}
	void MainState::KeyDoubleClick(int _key)  
	{ 
//		if (m_hud.KeyDoubleClick(_key)) return;
	}

	// ******************************************************* //
	Ship& MainState::CreatePlayerShip()
	{
		return *new Ship("PlayerShip", Vec3(0.f));
	}

	void MainState::SetMap(Game::Map& _map)
	{
		m_map.reset(&_map);
	}

	void MainState::RemovePlayer()
	{
		m_sceneGraph.Remove(m_playerShip);
		for (auto item : m_playerShip.GetInventory())
		{
			m_sceneGraph.Remove(*static_cast<Weapon*>(item));
		}
	}
}