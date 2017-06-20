#include "ship.hpp"

namespace Game
{
	using namespace ei;
	using namespace std;

	Ship::Ship(const string& _pFile, const Vec3& _position)
		: Model(_pFile, _position, qidentity()),
		m_thrust(10.0f),
		m_weight(1.0f),
		m_speed(1.0f),
		m_maxSpeed(10.0f)
	{

	}

	void Ship::Process(float _deltaTime)
	{
		m_speed = ei::min(m_maxSpeed, m_speed);
		Vec3 forward = ei::rotation(GetRotation()) * Vec3(0.0f, 0.0f, 1.0f);
		float forwardVelocity = 0.0f;
		if (len(GetVelocity()) > 0.0f)
			forwardVelocity = dot(forward, normalize(GetVelocity())) * len(GetVelocity());
		if (forwardVelocity < m_speed)
		{
			// since we have newtons second axiom: F = m * a => a = F / m
			float acceleration = m_thrust / m_weight;
			// and since delta_v = a * delta_t
			float deltaSpeed = acceleration * _deltaTime;
			// Apply the velocity to the player ship in the current direction
			Vec3 newVel = (len(GetVelocity()) + deltaSpeed) * forward;
			SetVelocity(newVel);
		}
		Model::Process(_deltaTime);
	}
}