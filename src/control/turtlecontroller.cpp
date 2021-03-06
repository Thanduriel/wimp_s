#include "turtlecontroller.hpp"
#include "gameplay/elements/ship.hpp"
#include "gamestates/huds/mainhud.hpp"
#include "math/extensions.hpp"

namespace Control
{
	using namespace ei;

	TurtleController::TurtleController(Game::Ship& _ship, Game::Actor::ConstHandle _target, GameStates::MainHud& _hud, const std::string& _name)
		: Controller(_ship, _hud)
	{
		m_target = _target;
		m_minDistance = 50.0f;
		m_maxDistance = 400.0f;
		m_maxFollowTime = 5.0f;
		m_evasionTime = 5.0f;
		GetShip().SetSpeed(0.0f);
	//	GetShip().SetAngularAcceleration(0.5f);
		GetShip().SetWeaponTarget(**_target);
		m_hud.AddIndicator(this->GetShip(), _name);
	}

	void TurtleController::ProcessComponent(float _deltaTime, const Game::SceneGraph& _sceneGraph)
	{
		//Do the AI stuff in here
		if (*m_target)
		{
			ManageDistanceToTarget();

			ManageShooting();

		//	EvadeObstacles();
		}
	}

	void TurtleController::ManageDistanceToTarget()
	{
		const Game::Ship* target = static_cast<const Game::Ship*>(&**m_target);

		GetShip().SetTargetAngularVelocity(Vec3(0.0f, 0.0f, 0.0f));
		Vec3 delta = target->GetPosition() - GetShip().GetPosition();
		const float dist = len(delta);
		if (dist < m_minDistance)
			GetShip().SetSpeed(-10.0f);
		else if (dist > 1000.f)
		{
			RotateTowards(Vec3(0.f));
			GetShip().SetSpeed(30.f);
		}
		else if (dist > m_maxDistance)
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
		const Game::Ship* target = static_cast<const Game::Ship*>(&**m_target);

		Vec3 delta = target->GetPosition() - GetShip().GetPosition();
		if (len(delta) < m_maxDistance)
		{
			Vec3 forward = GetShip().GetRotationMatrix() * Vec3(0.0f, 0.0f, 1.0f);

		//	if (dot(delta, forward) > 0.0f)
			{
				float angle = acosf(clamp(dot(normalize(delta), forward), -1.0f, 1.0f));
				if (angle < ei::PI / 2.0f)
				{
					Ray ray(m_ship.GetPosition(), normalize(delta));
					ray.origin = m_ship.GetInverseTransformation() * ray.origin;
					ray.direction = m_ship.GetInverseRotationMatrix() * ray.direction;

					// put origin outside of the sphere and invert direction
					// so that the intersection test finds the right point
					ray.origin += ray.direction * 100000.f;
					ray.direction *= -1.f;
					m_ship.AdjustWeaponOrientation(ray);
					GetShip().Fire();
				}
			}
		}
	}
}