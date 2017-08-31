#pragma once
#include "gameplay/core/actor.hpp"
#include "controller.hpp"

namespace Game {
	class Ship;
}

namespace Control
{
	class TurtleController : public Controller
	{
	public:
		TurtleController(Game::Ship& _ship, Game::Actor::ConstHandle _target, GameStates::MainHud& _hud, const std::string& _name = "Turtle Ship");

		void ProcessComponent(float _deltaTime, const Game::SceneGraph& _sceneGraph) override;

		void ManageDistanceToTarget();

		void ManageShooting();
	};
}