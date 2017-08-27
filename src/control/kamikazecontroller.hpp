#pragma once
#include "gameplay/core/actor.hpp"
#include "controller.hpp"

namespace Game {
	class Ship;
}

namespace Control
{
	class KamikazeController : public Controller
	{
	public:
		KamikazeController(Game::Ship& _ship, Game::Actor::Handle _target, GameStates::MainHud& _hud);

		void ProcessComponent(float _deltaTime, const Game::SceneGraph& _sceneGraph) override;

		void ManageDistanceToTarget();

		void EvadeShipBehind(float _deltaTime);

		void ManageShooting();
	};
}