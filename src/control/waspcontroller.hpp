#pragma once
#include "gameplay/core/actor.hpp"
#include "controller.hpp"

namespace Control
{
	class WaspController : public Controller
	{
	public:
		WaspController(Game::Ship& _ship, Game::Actor::ConstHandle _target, GameStates::MainHud& _hud);

		void ProcessComponent(float _deltaTime, const Game::SceneGraph& _sceneGraph) override;

		void ManageDistanceToTarget();

		void EvadeShipBehind(float _deltaTime);

		void ManageShooting();

		float GetMaxDistance() const { return m_maxDistance; }
		void SetMaxDistance(const float _distance) { m_maxDistance = _distance; };

		float GetMinDistance() const { return m_minDistance; }
		void SetMinDistance(const float _distance) { m_minDistance = _distance; };
	};
}