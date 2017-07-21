#include "enemyship.hpp"

namespace Game
{
	EnemyShip::EnemyShip(const string& _pFile, const Vec3& _position, Ship& _target)
		: Ship(_pFile, _position),
		m_target(&_target),
		m_minDistance(50.0f),
		m_maxDistance(200.0f),
		m_lookForTarget(false)
	{
		SetSpeed(50.0f);
	}

	void EnemyShip::Process(float _deltaTime)
	{
		//Do the AI stuff in here
		ManageDistanceToTarget();

		Ship::Process(_deltaTime);
	}

	void EnemyShip::ManageDistanceToTarget()
	{
		SetTargetAngularVelocity(Vec3(0.0f, 0.0f, 0.0f));
		Vec3 delta = m_target->GetPosition() - GetPosition();
		Ray trajectoryForward = Ray(GetPosition(), normalize(GetRotationMatrix() * Vec3(0.0f, 0.0f, -1.0f)));
		if (len(delta) < m_minDistance)
		{
			//TODO: Check how this actually works
			float distance = 0.0f;
			if (m_target->GetCollisionComponent().RayCastFast(trajectoryForward, distance))
			{
				//Evade ship by pulling up
				SetTargetAngularVelocity(GetRotationMatrix() * Vec3(2.0f, 0.0f, 0.0f));
			}
		}
		else if (len(delta) > m_maxDistance)
		{
			m_lookForTarget = true;
		}

		if (m_lookForTarget)
		{
			Ray trajectoryForward = Ray(GetPosition(), normalize(GetRotationMatrix() * Vec3(0.0f, 0.0f, -1.0f)));
			float distance = 0.0;
			if (m_target->GetCollisionComponent().RayCastFast(trajectoryForward, distance))
				m_lookForTarget = false;

			//Turn towards target ship
			Vec3 forward = GetRotationMatrix() * Vec3(0.0f, 0.0f, -1.0f);
			Vec3 rotationVector = normalize(cross(delta, forward));
			SetTargetAngularVelocity(rotationVector);
		}
	}
}