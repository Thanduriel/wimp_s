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
	class Controller
	{
	public:
		Controller(Game::Ship& _ship, GameStates::MainHud& _hud);
		virtual ~Controller() {};

		virtual void Process(float _deltaTime) abstract;

		Game::Ship& GetShip() const { return m_ship; };

		static void SetSceneGraph(Game::SceneGraph& _graph) { s_sceneGraph = &_graph; }

		void RotateTowards(ei::Vec3 _pos);

	protected:
		Game::Ship& m_ship;
		static Game::SceneGraph* s_sceneGraph;

		GameStates::MainHud& m_hud;
	};
}