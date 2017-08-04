#include "enemyship.hpp"

namespace Game
{
	EnemyShip::EnemyShip(const string& _pFile, const Vec3& _position, Actor::Handle _target)
		: Ship(_pFile, _position),
		m_target(_target),
		m_minDistance(75.0f),
		m_maxDistance(500.0f),
		m_lookForTarget(false)
	{
		SetSpeed(50.0f);
	}

	void EnemyShip::Process(float _deltaTime)
	{
		//Do the AI stuff in here
		if (*m_target)
		{
			ManageDistanceToTarget();

			ManageShooting();
		}

		Ship::Process(_deltaTime);
	}

	void EnemyShip::ManageDistanceToTarget()
	{
		Ship* target = static_cast<Ship*>(&**m_target);


		SetTargetAngularVelocity(Vec3(0.0f, 0.0f, 0.0f));
		Vec3 delta = target->GetPosition() - GetPosition();
		Ray trajectoryForward = Ray(GetPosition(), normalize(GetRotationMatrix() * Vec3(0.0f, 0.0f, 1.0f)));
		if (len(delta) < m_minDistance)
		{
			m_lookForTarget = false;
			float distance = 0.0f;
			if (target->GetCollisionComponent().RayCastFast(trajectoryForward, distance))
			{
				//Evade ship by pulling up
				SetTargetAngularVelocity(GetRotationMatrix() * Vec3(1.0f, 0.0f, 0.0f));
			}
		}
		else if (len(delta) > m_maxDistance)
		{
			m_lookForTarget = true;
		}

		if (m_lookForTarget)
		{
			//Stop rotating when pointing at the target to avoid wobbling
			Ray trajectoryForward = Ray(GetPosition(), normalize(GetRotationMatrix() * Vec3(0.0f, 0.0f, 1.0f)));
			float distance;
			if (target->GetCollisionComponent().RayCast(trajectoryForward, distance))
				SetAngularVelocity(Vec3(0.0f));

			//Turn towards target ship
			Vec3 forward = normalize(GetRotationMatrix() * Vec3(0.0f, 0.0f, 1.0f));
			//Clamp the dot product to avoid getting a domain error
			float angle = acosf(clamp(dot(normalize(delta), forward), -1.0f, 1.0f));
			float factor = clamp(angle, 0.0f, ei::PI) / (ei::PI);
			Vec3 rotationVector = normalize(cross(forward, delta)) * factor;
			SetTargetAngularVelocity(rotationVector);
		}
	}

	void EnemyShip::ManageShooting()
	{
		Ship* target = static_cast<Ship*>(&**m_target);

		Ray ray = Ray(GetPosition(), normalize(GetRotationMatrix() * Vec3(0.0f, 0.0f, 1.0f)));
		float distance;
		if (target->GetCollisionComponent().RayCast(ray, distance))
			Fire();
	}
}