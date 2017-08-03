#include "ship.hpp"
#include "../scenegraph.hpp"
#include "shipsystems/weapon.hpp"
#include "generators/weapongen.hpp"
#include "gameplay/content.hpp"
#include "ei/3dintersection.hpp"

namespace Game
{
	using namespace ei;
	using namespace std;

	//delegate constructor with the node to reduce amount of indirections of the file access
	Ship::Ship(const string& _pFile, const Vec3& _position)
		: Ship(Content::GetShipData()[_pFile], _position)
	{}

	Ship::Ship(const Jo::Files::MetaFileWrapper::Node& _node, const Vec3& _position)
		: Model(_node["Mesh"s], _node["BoundingMesh"s], _position, qidentity()),
		m_thrust(50.0f),
		m_speed(1.0f),
		m_minSpeed(0.0f),
		m_maxSpeed(100.0f),
		m_minSprayRadius(0.0f),
		m_maxSprayRadius(1.0f),
		m_sprayRadius(0.0f),
		m_angularAcceleration(1.0f),
		m_targetAngularVelocity(0.f),
		m_maxEnergy(10.f),
		m_energy(m_maxEnergy),
		m_energyRecharge(1.5f),
		m_drivePositions(_node["DriveSockets"s].Size()),
		m_weaponSockets(_node["WeaponSockets"s].Size()),
		m_thrustParticles(m_drivePositions.capacity()),
		m_thrustLights(m_drivePositions.capacity()),
		m_particleSpawnCount(0.f)
	{
		auto& weaponsNode = _node["WeaponSockets"s];
		for (int i = 0; i < m_weaponSockets.capacity(); ++i)
			m_weaponSockets.emplace(THISACTOR, GetGeometryComponent().GetMesh().GetSocket(weaponsNode[i]));

		auto& drivesNode = _node["DriveSockets"s];
		for (int i = 0; i < m_drivePositions.capacity(); ++i)
		{
			m_drivePositions.emplace(GetGeometryComponent().GetMesh().GetSocket(drivesNode[i]));
			m_thrustParticles.emplace(THISACTOR, m_drivePositions[i]);
			m_thrustLights.emplace(THISACTOR, m_drivePositions[i], 2.f, Utils::Color8U(0.f, 1.f, 0.f));
		}
		m_health = _node["BaseHealth"s].Get(42.f);
		m_maxHealth = m_health;

		// todo: make depended on acceleration
		static thread_local Generators::RandomGenerator rng(0x614AA);
		for(int i = 0; i < m_drivePositions.size(); ++i)
			m_thrustParticles[i].SetEmitter(200.f, [=]() {return rng.Direction() * 0.1f; },
				[]() {return Vec3(0.f, 0.f, -1.f); },
				[=]() {return rng.Uniform(1.f, 3.f); },
				[]() {return Utils::Color8U(0.5f, 0.2f, 0.7f, 1.f).RGBA(); },
				[]() {return 0.1f; });

		m_canTakeDamage = true;
		m_mass = 1.f;
		GetCollisionComponent().SetType(CollisionComponent::Type::Any | CollisionComponent::Type::Solid 
			| CollisionComponent::Type::Ship | CollisionComponent::Type::Dynamic);

		// temporary weapon setup
		Weapon& weapon1 = FactoryActor::GetThreadLocalInstance().Make<Weapon>();
		Weapon& weapon2 = FactoryActor::GetThreadLocalInstance().Make<Weapon>();
		m_weaponSockets[0].Attach(weapon1);
		m_weaponSockets[2].Attach(weapon2);
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
		m_energy = ei::min(m_energy + _deltaTime * m_energyRecharge, m_maxEnergy);
		// Get current speed here for performance reasons
		float currentSpeed = GetCurrentSpeed();

		// Update the speed
		UpdateSpeed(currentSpeed, _deltaTime);

		// Update angular velocity
		UpdateAngularVelocity(_deltaTime);

	//	AdjustWeaponOrientation(Vec3(0.f, 0.f, 1.f));

		Model::Process(_deltaTime);
	}

	void Ship::RegisterComponents(SceneGraph& _sceneGraph)
	{
		Model::RegisterComponents(_sceneGraph);
		for(auto& socket : m_weaponSockets)
			_sceneGraph.RegisterComponent(socket);

		for (auto& light : m_thrustLights)
			_sceneGraph.RegisterComponent(light);

		for(auto& ps : m_thrustParticles)
			_sceneGraph.RegisterComponent(ps);
	}

	// ***************************************************************************** //
	void Ship::Fire()
	{
		float speed = ei::len(m_velocity);
		for (auto& sockets : m_weaponSockets)
		{
			Weapon* weapon = static_cast<Weapon*>(sockets.GetAttached());
			if (!weapon) continue;

			m_energy -= weapon->Fire(speed, m_energy);
		}
	}

	void Ship::AdjustWeaponOrientation(const ei::Ray& _targetRay)
	{
		for (auto& socket : m_weaponSockets)
		{
			Weapon* weapon = static_cast<Weapon*>(socket.GetAttached());
			if (!weapon) continue;

			Vec3 pos = socket.GetPosition();
			float d;
			// this should always be true since d >> d(ray.position,socket.GetPosition());
			bool b = ei::intersects(_targetRay, ei::Sphere(pos, weapon->GetRange()), d);
			Vec3 targetPos = _targetRay.origin + _targetRay.direction * d;

			Vec3 targetDir = targetPos - pos;

			socket.SetRotation(normalize(targetDir));
		}
	}

	void Ship::SetWeaponTarget(const Actor& _actor)
	{
		for (auto& socket : m_weaponSockets)
		{
			Weapon* weapon = static_cast<Weapon*>(socket.GetAttached());
			if (weapon)
				weapon->SetTarget(_actor);
		}
	}

	void Ship::SetWeapon(int _slot, Weapon& _weapon)
	{
		m_weaponSockets[_slot].GetAttached()->Destroy();
		m_weaponSockets[_slot].Attach(_weapon);
		FactoryActor::GetThreadLocalInstance().Add(_weapon);
	}
}