#include "ship.hpp"
#include "../scenegraph.hpp"
#include "shipsystems/weapon.hpp"

namespace Game
{
	using namespace ei;
	using namespace std;

	Ship::Ship(const string& _pFile, const Vec3& _position)
		: Model(_pFile, _position, qidentity()),
		m_thrust(10.0f),
		m_weight(1.0f),
		m_speed(1.0f),
		m_minSpeed(0.0f),
		m_maxSpeed(10.0f),
		m_minSprayRadius(0.0f),
		m_maxSprayRadius(1.0f),
		m_sprayRadius(0.0f),
		m_weaponSockets{ SocketComponent(THISACTOR, Vec3(1.f,0.f,0.f)), SocketComponent(THISACTOR, Vec3(-1.f,0.f,0.f)) },
		m_actorFactory(THISACTOR)
	{
		Weapon& weapon = m_actorFactory.Make<Weapon>(Vec3());
		m_weaponSockets[0].Attach(weapon);
	}

	float Ship::GetCurrentSpeed() const
	{
		Vec3 forward = ei::rotation(GetRotation()) * Vec3(0.0f, 0.0f, 1.0f);
		float forwardVelocity = 0.0f;
		if (len(GetVelocity()) > 0.0f)
			forwardVelocity = dot(forward, normalize(GetVelocity())) * len(GetVelocity());
		return forwardVelocity;
	}

	void Ship::UpdateSpeed(float currentSpeed, float _deltaTime)
	{
		Vec3 forward = ei::rotation(GetRotation()) * Vec3(0.0f, 0.0f, 1.0f);
		if (currentSpeed < m_speed)
		{
			// since we have newtons second axiom: F = m * a => a = F / m
			float acceleration = m_thrust / m_weight;
			// and since delta_v = a * delta_t
			float deltaSpeed = acceleration * _deltaTime;
			// Apply the velocity to the player ship in the current direction
			// min() to prevent ship from freaking out at low framerates
			Vec3 newVel = ei::min(len(GetVelocity()) + deltaSpeed, m_speed) * forward;
			SetVelocity(newVel);
		}
		else if (m_speed < currentSpeed)
		{
			// same as above, but decelerate
			float deceleration = -m_thrust / m_weight;
			float deltaSpeed = deceleration * _deltaTime;
			// max() to prevent ship from freaking out at low framerates
			Vec3 newVel = ei::max(len(GetVelocity()) + deltaSpeed, m_speed) * forward;
			SetVelocity(newVel);
		}
	}

	void Ship::UpdateSprayRadius(float currentSpeed)
	{
		// Scale spray radius from min to max over speed 0 to max speed
		float factor = (currentSpeed - m_minSpeed) / (m_maxSpeed - m_minSpeed);
		m_sprayRadius = ei::max(ei::min(m_minSprayRadius + (m_maxSprayRadius - m_minSprayRadius) * factor, m_maxSprayRadius), m_minSprayRadius);
	}

	void Ship::Process(float _deltaTime)
	{
		// Get current speed here for performance reasons
		float currentSpeed = GetCurrentSpeed();

		// Update the speed
		UpdateSpeed(currentSpeed, _deltaTime);

		// Update the spray radius
		UpdateSprayRadius(currentSpeed);

		Model::Process(_deltaTime);
	}

	void Ship::RegisterComponents(SceneGraph& _sceneGraph)
	{
		_sceneGraph.RegisterComponent(m_actorFactory);

		Model::RegisterComponents(_sceneGraph);
		for (auto& socket : m_weaponSockets)
			_sceneGraph.RegisterComponent(socket);
	}

	void Ship::Fire()
	{
		for (auto& sockets : m_weaponSockets)
		{
			Weapon* weapon = static_cast<Weapon*>(sockets.GetActor());
			if (!weapon) continue;

			weapon->Fire();
		}
	}
}