#include "waspcontroller.hpp"
#include "gameplay/elements/ship.hpp"
#include "gamestates/huds/mainhud.hpp"

namespace Control
{
	using namespace ei;

	WaspController::WaspController(Game::Ship& _ship, Game::Actor::Handle _target, GameStates::MainHud& _hud)
		: Controller(_ship, _hud),
		m_target(_target),
		m_minDistance(75.0f),
		m_maxDistance(500.0f),
		m_lookForTarget(false)
	{
		GetShip().SetSpeed(50.0f);
		m_hud.AddIndicator();
	}

	void WaspController::Process(float _deltaTime)
	{
		//Do the AI stuff in here
		if (*m_target)
		{
			ManageDistanceToTarget();

			ManageShooting();
		}
	}

	void WaspController::ManageDistanceToTarget()
	{
		Game::Ship* target = static_cast<Game::Ship*>(&**m_target);


		GetShip().SetTargetAngularVelocity(Vec3(0.0f, 0.0f, 0.0f));
		Vec3 delta = target->GetPosition() - GetShip().GetPosition();
		Ray trajectoryForward = Ray(GetShip().GetPosition(), normalize(GetShip().GetRotationMatrix() * Vec3(0.0f, 0.0f, 1.0f)));
		if (len(delta) < m_minDistance)
		{
			m_lookForTarget = false;
			float distance = 0.0f;
			if (target->GetCollisionComponent().RayCastFast(trajectoryForward, distance))
			{
				//Evade ship by pulling up
				GetShip().SetTargetAngularVelocity(GetShip().GetRotationMatrix() * Vec3(1.0f, 0.0f, 0.0f));
			}
		}
		else if (len(delta) > m_maxDistance)
		{
			m_lookForTarget = true;
		}

		if (m_lookForTarget)
		{
			//Stop rotating when pointing at the target to avoid wobbling
			Ray trajectoryForward = Ray(GetShip().GetPosition(), normalize(GetShip().GetRotationMatrix() * Vec3(0.0f, 0.0f, 1.0f)));
			float distance;
			if (target->GetCollisionComponent().RayCast(trajectoryForward, distance))
				GetShip().SetAngularVelocity(Vec3(0.0f));

			//Turn towards target ship
			Vec3 forward = normalize(GetShip().GetRotationMatrix() * Vec3(0.0f, 0.0f, 1.0f));
			//Clamp the dot product to avoid getting a domain error
			float angle = acosf(clamp(dot(normalize(delta), forward), -1.0f, 1.0f));
			float factor = clamp(angle, 0.0f, ei::PI) / (ei::PI);
			Vec3 rotationVector = normalize(cross(forward, delta)) * factor;
			GetShip().SetTargetAngularVelocity(rotationVector);
		}
	}

	void WaspController::ManageShooting()
	{
		Game::Ship* target = static_cast<Game::Ship*>(&**m_target);

		Ray ray = Ray(GetShip().GetPosition(), normalize(GetShip().GetRotationMatrix() * Vec3(0.0f, 0.0f, 1.0f)));
		float distance;
		if (target->GetCollisionComponent().RayCast(ray, distance))
			GetShip().Fire();
	}
}