#include "turtlecontroller.hpp"
#include "gameplay/elements/ship.hpp"
#include "gamestates/huds/mainhud.hpp"

namespace Control
{
	using namespace ei;

	TurtleController::TurtleController(Game::Ship& _ship, Game::Actor::Handle _target, GameStates::MainHud& _hud)
		: Controller(_ship, _hud),
		m_target(_target),
		m_minDistance(50.0f),
		m_maxDistance(200.0f),
		m_lookForTarget(false),
		m_followTimeCounter(0.0f),
		m_maxFollowTime(5.0f),
		m_evasionTime(5.0f),
		m_evading(-1.0f)
	{
		GetShip().SetSpeed(0.0f);
		GetShip().SetAngularAcceleration(0.5f);
		m_hud.AddIndicator(this->GetShip(), "Turtle Ship");
	}

	void TurtleController::ProcessComponent(float _deltaTime, const Game::SceneGraph& _sceneGraph)
	{
		//Do the AI stuff in here
		if (*m_target)
		{
			ManageDistanceToTarget();

			ManageShooting();
		}
	}

	void TurtleController::ManageDistanceToTarget()
	{
		Game::Ship* target = static_cast<Game::Ship*>(&**m_target);

		GetShip().SetTargetAngularVelocity(Vec3(0.0f, 0.0f, 0.0f));
		Vec3 delta = target->GetPosition() - GetShip().GetPosition();
		if (len(delta) < m_minDistance)
			GetShip().SetSpeed(-10.0f);
		else if (len(delta) > m_maxDistance)
			GetShip().SetSpeed(10.0f);
		else
			GetShip().SetSpeed(0.0f);

		//Stop rotating when pointing at the target to avoid wobbling
		Ray trajectoryForward = Ray(GetShip().GetPosition(), normalize(GetShip().GetRotationMatrix() * Vec3(0.0f, 0.0f, 1.0f)));
		float distance;
		if (target->GetCollisionComponent().RayCast(trajectoryForward, distance))
			GetShip().SetAngularVelocity(Vec3(0.0f));

		//Turn towards target ship
		RotateTowards(target->GetPosition());
	}

	void TurtleController::ManageShooting()
	{
		Game::Ship* target = static_cast<Game::Ship*>(&**m_target);

		Vec3 delta = target->GetPosition() - GetShip().GetPosition();
		if (len(delta) < m_maxDistance)
		{
			Vec3 forward = GetShip().GetRotationMatrix() * Vec3(0.0f, 0.0f, 1.0f);
			float distance;
			if (dot(delta, forward) > 0.0f)
			{
				float angle = acosf(clamp(dot(normalize(delta), forward), -1.0f, 1.0f));
				if (angle < ei::PI / 4.0f)
					GetShip().Fire();
			}
		}
	}
}