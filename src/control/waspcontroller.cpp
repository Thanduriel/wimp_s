#include "waspcontroller.hpp"
#include "gameplay/elements/ship.hpp"
#include "gamestates/huds/mainhud.hpp"

namespace Control
{
	using namespace ei;

	WaspController::WaspController(Game::Ship& _ship, Game::Actor::ConstHandle _target, GameStates::MainHud& _hud, const std::string& _name)
		: Controller(_ship, _hud)
	{
		m_target = _target;
		m_minDistance = 75.0f;
		m_maxDistance = _ship.GetMaxSpeed() * 4.5f;
		m_maxFollowTime = 5.0f;
		m_evasionTime = 5.0f;
		GetShip().SetSpeed(120.0f);
	//	GetShip().SetAngularAcceleration(2.0f);
		GetShip().SetWeaponTarget(**_target);
		m_hud.AddIndicator(this->GetShip(), _name);
	}

	void WaspController::ProcessComponent(float _deltaTime, const Game::SceneGraph& _sceneGraph)
	{
		//Do the AI stuff in here
		if (*m_target)
		{
			ManageDistanceToTarget();

			EvadeShipBehind(_deltaTime);

			ManageShooting();

			EvadeObstacles();
		}
	}

	void WaspController::ManageDistanceToTarget()
	{
		const Game::Ship* target = static_cast<const Game::Ship*>(&**m_target);

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
			RotateTowards(target->GetPosition());
		}
	}

	void WaspController::EvadeShipBehind(float _deltaTime)
	{
		const Game::Ship* target = static_cast<const Game::Ship*>(&**m_target);

		Vec3 delta = target->GetPosition() - m_ship.GetPosition();
		Vec3 forward = normalize(m_ship.GetRotationMatrix() * Vec3(0.0f, 0.0f, 1.0f));

		if (m_evading > 0.0f)
		{
			RotateTowards(target->GetPosition() + delta * 10.0f);
			m_evading -= _deltaTime;
		}
		else
		{
			//Is the enemy behind me and do the trajectories align?
			if (dot(delta, forward) < 0.0f && dot(target->GetVelocity(), GetShip().GetVelocity()) > 0.0f)
			{
				//Check if ship is in a cone of sight of a 45 degree angle and also flies in the same direction
				float angle = acosf(clamp(dot(normalize(-delta), forward), -1.0f, 1.0f));
				float velocityAngle = acosf(clamp(dot(normalize(target->GetVelocity()), normalize(GetShip().GetVelocity())), -1.0f, 1.0f));
				if (angle < ei::PI / 4.0f && velocityAngle < ei::PI / 4.0f)
				{
					//Ship seems to look at us
					m_followTimeCounter += _deltaTime;
				}
				else
					m_followTimeCounter = 0.0f;

				if (m_followTimeCounter > m_maxFollowTime)
				{
					//Start evasion maneuver
					m_evading = m_evasionTime;
					m_followTimeCounter = 0.0f;
				}
			}
		}
	}

	void WaspController::ManageShooting()
	{
		const Game::Ship* target = static_cast<const Game::Ship*>(&**m_target);

		Vec3 delta = target->GetPosition() - GetShip().GetPosition();
		Vec3 forward = GetShip().GetRotationMatrix() * Vec3(0.0f, 0.0f, 1.0f);

		if (lensq(delta) < 500.f*500.f && dot(delta, forward) > 0.0f)
		{
			float angle = acosf(clamp(dot(normalize(delta), forward), -1.0f, 1.0f));
			if (angle < ei::PI / 4.0f)
				GetShip().Fire();
		}
	}
}