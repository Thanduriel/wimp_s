#include "ship.hpp"
#include "../scenegraph.hpp"
#include "shipsystems/weapon.hpp"
#include "generators/weapongen.hpp"

namespace Game
{
	using namespace ei;
	using namespace std;

	Ship::Ship(const string& _pFile, const Vec3& _position, CollisionComponent::Type _collisionType)
		: Model(_pFile, _pFile + "bm", _position, qidentity()),
		m_thrust(50.0f),
		m_speed(1.0f),
		m_minSpeed(0.0f),
		m_maxSpeed(100.0f),
		m_minSprayRadius(0.0f),
		m_maxSprayRadius(1.0f),
		m_sprayRadius(0.0f),
		m_angularAcceleration(1.0f),
		m_targetAngularVelocity(0.f),
		m_weaponSockets{ {{THISACTOR, Vec3(3.f,1.5f,0.f)}, {THISACTOR, Vec3(-3.f,1.5f,0.f)} } },
		m_staticLights{ {{THISACTOR, Vec3(3.f, 0.f, -6.f), 5.f, Utils::Color8U(0.f,1.f,0.f)},
		{THISACTOR, Vec3(-3.f, 0.f, -6.f), 5.f, Utils::Color8U(0.f,1.f,0.f)} } }
	{
		m_health = 100;
		m_canTakeDamage = true;
		m_mass = 1.f;
		GetCollisionComponent().SetType(_collisionType);

		// temporary weapon setup
		Weapon& weapon1 = FactoryActor::GetThreadLocalInstance().Make<Weapon>();
		Weapon& weapon2 = FactoryActor::GetThreadLocalInstance().Make<Weapon>();
		m_weaponSockets[0].Attach(weapon1);
		m_weaponSockets[1].Attach(weapon2);
	}

	float Ship::GetCurrentSpeed() const
	{
		Vec3 forward = ei::rotation(GetRotation()) * Vec3(0.0f, 0.0f, 1.0f);
		float forwardVelocity = 0.0f;
		float l = len(GetVelocity());
		if (l > 0.0f)
			forwardVelocity = dot(forward, normalize(GetVelocity())) * l;
		return forwardVelocity;
	}

	void Ship::UpdateSpeed(float currentSpeed, float _deltaTime)
	{
		Vec3 forward = ei::rotation(GetRotation()) * Vec3(0.0f, 0.0f, 1.0f);
		if (currentSpeed < m_speed)
		{
			// since we have newtons second axiom: F = m * a => a = F / m
			float acceleration = m_thrust / m_mass;
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
			float deceleration = -m_thrust / m_mass;
			float deltaSpeed = deceleration * _deltaTime;
			// max() to prevent ship from freaking out at low framerates
			Vec3 newVel = ei::max(len(GetVelocity()) + deltaSpeed, m_speed) * forward;
			SetVelocity(newVel);
		}
	}

	void Ship::UpdateAngularVelocity(float _deltaTime)
	{
		Vec3 newAngularVelocity(GetAngularVelocity());
		for (int i = 0; i < 3; i++)
		{
			if (GetAngularVelocity()[i] < m_targetAngularVelocity[i])
				newAngularVelocity[i] = ei::min(lerp(newAngularVelocity[i], m_targetAngularVelocity[i], m_angularAcceleration * _deltaTime), m_targetAngularVelocity[i]);
			else
				newAngularVelocity[i] = ei::max(lerp(newAngularVelocity[i], m_targetAngularVelocity[i], m_angularAcceleration * _deltaTime), m_targetAngularVelocity[i]);
		}
		SetAngularVelocity(newAngularVelocity);
	}

	void Ship::Process(float _deltaTime)
	{
		// Get current speed here for performance reasons
		float currentSpeed = GetCurrentSpeed();

		// Update the speed
		UpdateSpeed(currentSpeed, _deltaTime);

		// Update angular velocity
		UpdateAngularVelocity(_deltaTime);

		AdjustWeaponOrientation(Vec3(0.f, 0.f, 1.f));

		Model::Process(_deltaTime);
	}

	void Ship::RegisterComponents(SceneGraph& _sceneGraph)
	{
		Model::RegisterComponents(_sceneGraph);
		for (auto& socket : m_weaponSockets)
			_sceneGraph.RegisterComponent(socket);

		for (auto& light : m_staticLights)
			_sceneGraph.RegisterComponent(light);
	}

	// ***************************************************************************** //
	void Ship::Fire()
	{
		float speed = ei::len(m_velocity);
		for (auto& sockets : m_weaponSockets)
		{
			Weapon* weapon = static_cast<Weapon*>(sockets.GetAttached());
			if (!weapon) continue;

			weapon->SetBeginSpeed(speed);
			weapon->Fire();
		}
	}

	void Ship::AdjustWeaponOrientation(const ei::Vec3& _targetDir)
	{
		for (auto& socket : m_weaponSockets)
		{
			Weapon* weapon = static_cast<Weapon*>(socket.GetAttached());
			if (!weapon) continue;
			Vec3 targetPos = _targetDir * weapon->GetRange();

			Vec3 targetDir = targetPos - socket.GetPosition();

			socket.SetRotation(normalize(targetDir));
		}
	}

	void Ship::SetWeapon(int _slot, Weapon& _weapon)
	{
		m_weaponSockets[_slot].GetAttached()->Destroy();
		m_weaponSockets[_slot].Attach(_weapon);
		FactoryActor::GetThreadLocalInstance().Add(_weapon);
	}
}