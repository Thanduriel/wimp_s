#pragma once
#include "gameplay/core/actor.hpp"
#include "gameplay/elements/ship.hpp"

namespace Game {
	class Ship;
}

namespace GameStates {
	class MainHud;
}

namespace Control
{
	class Controller : public Game::DynamicComponent<Game::SceneComponent>
	{
	public:
		Controller(Game::Ship& _ship, GameStates::MainHud& _hud);

		Game::Ship& GetShip() const { return m_ship; };

		static void SetSceneGraph(Game::SceneGraph& _graph) { s_sceneGraph = &_graph; }

		void RotateTowards(ei::Vec3 _pos);

		void Fire();
	protected:
		Game::Ship& m_ship;
		static Game::SceneGraph* s_sceneGraph;

		GameStates::MainHud& m_hud;
	};
}