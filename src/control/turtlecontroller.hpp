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
		TurtleController(Game::Ship& _ship, Game::Actor::Handle _target, GameStates::MainHud& _hud);

		void ProcessComponent(float _deltaTime, const Game::SceneGraph& _sceneGraph) override;

		void ManageDistanceToTarget();

		void ManageShooting();

		void EvadeObstacles();

		float GetMaxDistance() const { return m_maxDistance; }
		void SetMaxDistance(const float _distance) { m_maxDistance = _distance; };

		float GetMinDistance() const { return m_minDistance; }
		void SetMinDistance(const float _distance) { m_minDistance = _distance; };
	private:
		Game::Actor::Handle m_target;
		float m_maxDistance;
		float m_minDistance;
		bool m_lookForTarget;
		float m_followTimeCounter;
		float m_maxFollowTime;
		float m_evading;
		float m_evasionTime;
	};
}