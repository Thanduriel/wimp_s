#pragma once
#include "gameplay/core/actor.hpp"

namespace Game {
	class Ship;
}

namespace Control
{
	class Controller
	{
	public:
		Controller(Game::Ship& _ship);

		virtual void Process(float _deltaTime) abstract;

		Game::Ship& GetShip() const { return m_ship; };

		static void SetSceneGraph(Game::SceneGraph& _graph) { s_sceneGraph = &_graph; }
	private:
		Game::Ship& m_ship;

	protected:
		static Game::SceneGraph* s_sceneGraph;
	};
}