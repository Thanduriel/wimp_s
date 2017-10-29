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

		//@return the current angle
		float RotateTowards(ei::Vec3 _pos);
		void FlyTo(const ei::Vec3& _pos, float _speed);
		void EvadeObstacles();

		float GetMaxDistance() const { return m_maxDistance; }
		void SetMaxDistance(const float _distance) { m_maxDistance = _distance; };

		float GetMinDistance() const { return m_minDistance; }
		void SetMinDistance(const float _distance) { m_minDistance = _distance; };

	protected:
		Game::Ship& m_ship;
		static Game::SceneGraph* s_sceneGraph;

		float m_minDistance;
		float m_maxDistance;
		bool m_lookForTarget;
		float m_followTimeCounter;
		float m_maxFollowTime;
		float m_evading;
		float m_evasionTime;
		Game::Actor::ConstHandle m_target;

		GameStates::MainHud& m_hud;
	};

	enum struct AIControllerType
	{
		Wasp,
		Kamikaze,
		Turtle
	};
}