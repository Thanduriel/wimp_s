#include "ship.hpp"
#include "../scenegraph.hpp"
#include "shipsystems/weapon.hpp"
#include "generators/weapongen.hpp"
#include "gameplay/content.hpp"
#include "ei/3dintersection.hpp"
#include "shipsystems/specialmove.hpp"
#include "crate.hpp"
#include "explosion.hpp"
#include "clunk/clunk.h"

namespace Game
{
	using namespace ei;
	using namespace std;

	//delegate constructor with the node to reduce amount of indirections of the file access
	Ship::Ship(const string& _pFile, const Vec3& _position)
		: Ship(Content::GetShipData()[_pFile], _position)
	{}

	// use default implementation in the source file to allow SpecialMove to be predeclared
	Ship::~Ship() = default;

	Ship::Ship(const Jo::Files::MetaFileWrapper::Node& _node, const Vec3& _position)
		: Model(_node["Mesh"s], _node["BoundingMesh"s], _position, qidentity(), _node["Mass"s].Get(1000.f)),
		m_thrust(_node["Thrust"s].Get(50000.f)),
		m_speed(0.0f),
		m_minSpeed(_node["MinSpeed"s].Get(-10.f)),
		m_maxSpeed(_node["MaxSpeed"s].Get(100.f)),
		m_minSprayRadius(0.0f),
		m_maxSprayRadius(1.0f),
		m_sprayRadius(0.0f),
		m_angularAcceleration(_node["AngularSpeed"s].Get(1.f)),
		m_targetAngularVelocity(0.f),
		m_maxEnergy(_node["Energy"s].Get(8.f)),
		m_energy(m_maxEnergy),
		m_energyRecharge(_node["EnergyRecharge"s].Get(2.f)),
		m_maxShield(_node["Shield"s].Get(32.f)),
		m_shield(m_maxShield),
		m_shieldDelay(_node["ShieldDelay"s].Get(3.5f)),
		m_shieldRecharge(_node["ShieldRecharge"s].Get(3.5f)),
		m_shieldWait(0.f),
		m_isRecharging(true),
		m_shieldComponent(THISACTOR, GetGeometryComponent().GetMesh()),
		m_drivePositions(_node["DriveSockets"s].Size()),
		m_weaponSockets(_node["WeaponSockets"s].Size()),
		m_thrustParticles(m_drivePositions.capacity()),
		m_thrustLights(m_drivePositions.capacity() + _node["Lights"s].Size()),
		m_particleSpawnCount(0.f),
		m_audioComponent(THISACTOR),
		m_specialMove(),
		m_upgradeLevels{}
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
		auto& lightsNode = _node["Lights"s];
		for (int i = 0; i < lightsNode.Size(); ++i)
		{
			auto& lightInfo = lightsNode[i];
			m_thrustLights.emplace(THISACTOR, GetGeometryComponent().GetMesh().GetSocket(lightInfo["Name"s]),
				lightInfo["Range"s].Get(5.f), 
				Utils::Color8U(lightInfo["R"s].Get(1.f), lightInfo["G"s].Get(1.f), lightInfo["B"s].Get(1.f)));
		}

		m_health = _node["BaseHealth"s].Get(42.f);
		m_maxHealth = m_health;

		static thread_local Generators::RandomGenerator rng(0x614AA);
		for(int i = 0; i < m_drivePositions.size(); ++i)
			m_thrustParticles[i].SetEmitter(200.f, [=]() {return rng.Direction() * 0.15f; },
				[=]() {return Vec3(0.f, 0.f, rng.Uniform(-16.f, -1.f)); },
				[=]() {return rng.Uniform(2.f, 3.f); },
				[]() {return Utils::Color8U(0.65f, 0.25f, 0.75f, 1.f).RGBA(); },
				[]() {return 0.1f; });

		m_canTakeDamage = true;
		GetCollisionComponent().SetType(CollisionComponent::Type::Any | CollisionComponent::Type::Solid 
			| CollisionComponent::Type::Ship | CollisionComponent::Type::Dynamic);
		m_rotateVelocity = true;

		Vec3 avgPos{};
		for (auto& pos : m_drivePositions)
			avgPos += pos;
		avgPos *= 1.f / static_cast<float>(m_drivePositions.size());
		m_audioComponent.SetPosition(avgPos);
		m_audioComponent.Play(Content::GetSound("thruster"), 0, true);
	}

	void Ship::OnDestroy()
	{
		// leave items behind
		if(m_inventory.GetNumElements() || m_inventory.GetCredits()) FactoryActor::GetThreadLocalInstance().Make<Crate>(m_position, m_inventory);
		FactoryActor::GetThreadLocalInstance().Make<Explosion>(m_position, m_maxHealth / 5.f, 0.f, Utils::Color8U(0.8f, 0.67f, 0.42f, 0.5f), "mist");
		Model::OnDestroy();
	}

	float Ship::OnDamageTaken(float _amount, Actor& _source, DamageType _type)
	{
		// shield absorbs damage first
		float shieldDam = _amount;
		if (_type == DamageType::Ion)
			std::min(shieldDam * 1.5f, m_shield * 1.f / 1.5f);
		else shieldDam = std::min(shieldDam, m_shield);

		m_shield -= shieldDam;

		// recharge is interrupted
		m_shieldWait = 0;
		m_isRecharging = false;

		return (_amount - shieldDam) * (_type == DamageType::Physical ? 1.5f : 1.f);
	}

	// ****************************************************************** //
	float Ship::GetCurrentSpeed() const
	{
		Vec3 forward = m_rotationMatrix * Vec3(0.0f, 0.0f, 1.0f);
		float forwardVelocity = 0.0f;
		float l = len(GetVelocity());
		if (l > 0.0f)
			forwardVelocity = dot(forward, normalize(GetVelocity())) * l;
		return forwardVelocity;
	}

	void Ship::UpdateSpeed(float currentSpeed, float _deltaTime)
	{
		Vec3 forward = m_rotationMatrix * Vec3(0.0f, 0.0f, 1.0f);
		Vec3 dif = forward * m_speed - m_velocity;
		float l = len(dif);
		float deltaSpeed = 0.f;
		if (l > 0.01f)
		{
			// since we have newtons second axiom: F = m * a => a = F / m
			float acceleration = m_thrust / m_mass;
			// and since delta_v = a * delta_t
			deltaSpeed = ei::min(acceleration * _deltaTime, l);
			// Apply the velocity to the player ship in the current direction
			// min() to prevent ship from freaking out at low framerates
			AddVelocity(dif / l * deltaSpeed);
		//	if(currentSpeed < m_speed) newVel = ei::min(sign(m_speed) * len(GetVelocity()) + deltaSpeed, m_speed) * forward;
		//	else newVel = ei::max(sign(m_speed) * len(GetVelocity()) - deltaSpeed, m_speed) * forward;
		//	SetVelocity(newVel);
		}
	}

	void Ship::UpdateAngularVelocity(float _deltaTime)
	{
	/*	Vec3 newAngularVelocity(GetAngularVelocity());
		for (int i = 0; i < 3; i++)
		{
			if (GetAngularVelocity()[i] < m_targetAngularVelocity[i])
				newAngularVelocity[i] = ei::min(lerp(newAngularVelocity[i], m_targetAngularVelocity[i], m_angularAcceleration * _deltaTime), m_targetAngularVelocity[i]);
			else
				newAngularVelocity[i] = ei::max(lerp(newAngularVelocity[i], m_targetAngularVelocity[i], m_angularAcceleration * _deltaTime), m_targetAngularVelocity[i]);
		}*/
		Vec3 delta = (m_targetAngularVelocity - m_angularVelocity);
		float l = len(delta);
		if (l > m_angularAcceleration) delta = delta / l * m_angularAcceleration;
		SetAngularVelocity(m_angularVelocity + delta * _deltaTime);
	}

	void Ship::Process(float _deltaTime)
	{
		m_energy = ei::min(m_energy + _deltaTime * m_energyRecharge, m_maxEnergy);
		// shield logic
		m_shieldWait += _deltaTime;
		if (m_isRecharging)
			m_shield = ei::min(m_shield + _deltaTime * m_shieldRecharge, m_maxShield);
		else if (m_shieldWait >= m_shieldDelay)
			m_isRecharging = true;

		m_shieldComponent.SetActive(m_shield > 0.f);

		// todo: remove this from here and let SpecialMove be an Actor/Component
		if (m_specialMove) m_specialMove->Process(_deltaTime);

		// Get current speed here for performance reasons
		float currentSpeed = GetCurrentSpeed();

		// Update the speed
		float n = abs(currentSpeed) * 4.f;
	//	if (abs(m_speed - currentSpeed) > 2.0f)
	//		n += 2048.f;
		for (auto& thruster : m_thrustParticles)
			thruster.SetOutput(n);
		UpdateSpeed(currentSpeed, _deltaTime);
		m_audioComponent.GetSource(0)->gain = currentSpeed / m_maxSpeed;
	/*	if (n < 4 && m_audioComponent.GetObject()->playing(0))
		{
			m_audioComponent.GetObject()->fade_out(0, 0.7f);
		}
		else if(n > 5 && !m_audioComponent.GetObject()->playing(0))
			m_audioComponent.Play(Content::GetSound("thruster"), 0, true);*/

		// Update angular velocity
		UpdateAngularVelocity(_deltaTime);

	//	AdjustWeaponOrientation(Vec3(0.f, 0.f, 1.f));

	//	Quaternion oldRot = m_rotation;
		Model::Process(_deltaTime);
	//	Quaternion delta = oldRot / m_rotation;
	//	m_velocity = transform(m_velocity, delta);
	}

	void Ship::RegisterComponents(SceneGraph& _sceneGraph)
	{
		Model::RegisterComponents(_sceneGraph);

		_sceneGraph.RegisterComponent(m_shieldComponent);
		_sceneGraph.RegisterComponent(m_audioComponent);

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
		for (auto& socket : m_weaponSockets)
		{
			Weapon* weapon = static_cast<Weapon*>(socket.GetAttached());
			float d; // do not fire if the ship is in the line of fire
			if (!weapon || GetCollisionComponent().RayCast(Ray(weapon->GetPosition(), weapon->GetRotationMatrix() * Vec3(0.f,0.f,1.f)), d)) 
				continue;


			m_energy -= weapon->Fire(m_velocity, m_energy);
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

	void Ship::SetWeapon(int _slot, Weapon* _weapon)
	{
	//	m_weaponSockets[_slot].GetAttached()->Destroy();
		m_weaponSockets[_slot].Detach();
		if (_weapon)
		{
			m_weaponSockets[_slot].Attach(*_weapon);
			_weapon->SetOwner(this);
		}
	}

	void Ship::SetSpecialMove(SpecialMove& _sm)
	{
		m_specialMove = std::unique_ptr<SpecialMove>(&_sm);
	}
}