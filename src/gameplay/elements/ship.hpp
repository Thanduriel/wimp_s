#pragma once

#include <array>

#include "gameplay/core/model.hpp"
#include "factorycomponent.hpp"
#include "gameplay/elements/socketcomponent.hpp"
#include "gameplay/elements/light.hpp"
#include "utils/fixedarray.hpp"
#include "jofilelib.hpp"
#include "particlesystemcomponent.hpp"
#include "shipsystems/inventory.hpp"
#include "shieldcomponent.hpp"

namespace Game
{
	using namespace std;
	using namespace ei;

	class SpecialMove;

	class Ship : public Model
	{
	public:
		Ship(const string& _pFile, const Vec3& _position);
		~Ship();

		void Process(float _deltaTime) override;
		void OnDestroy() override;
		float OnDamageTaken(float _amount, Actor& _source, DamageType _type) override;

		// Update speed
		void UpdateSpeed(float currentSpeed, float _deltaTime);
		void UpdateAngularVelocity(float _deltaTime);

		//Access thrust
		void SetThrust(const float _thrust) { m_thrust = _thrust; };
		const float GetThrust() const { return m_thrust; };

		// Access speed
		void SetSpeed(const float _speed) { m_speed = ei::max(ei::min(m_maxSpeed, _speed), m_minSpeed); }
		float GetSpeed() const { return m_speed; }

		// Access min speed
		void SetMinSpeed(const float _minSpeed) { m_minSpeed = _minSpeed; }
		float GetMinSpeed() const { return m_minSpeed; }

		// Access max speed
		void SetMaxSpeed(const float _maxSpeed) { m_maxSpeed = _maxSpeed; }
		float GetMaxSpeed() const { return m_maxSpeed; }

		// Calculate current speed
		float GetCurrentSpeed() const;

		// Access max spray radius
		void SetMaxSprayRadius(const float _maxSprayRadius) { m_maxSprayRadius = _maxSprayRadius; }
		float GetMaxSprayRadius() const { return m_maxSprayRadius; }

		// Access min spray radius
		void SetMinSprayRadius(const float _minSprayRadius) { m_minSprayRadius = _minSprayRadius; }
		float GetMinSprayRadius() const { return m_minSprayRadius; }

		// Access current spray radius
		void SetSprayRadius(const float _sprayRadius) { m_sprayRadius = ei::max(ei::min(m_maxSprayRadius, _sprayRadius), m_minSprayRadius); }
		float GetSprayRadius() const { return m_sprayRadius; }

		// Access target angular velocity
		void SetTargetAngularVelocity(const Vec3 _angularVelocity) { m_targetAngularVelocity = _angularVelocity; };
		Vec3 GetTargetAngularVelocity() const { return m_targetAngularVelocity; };

		//Access angular acceleration
		void SetAngularAcceleration(const float _angularAcceleration) { m_angularAcceleration = _angularAcceleration; };
		float GetAngularAcceleration() const { return m_angularAcceleration; };

		float GetEnergy() const { return m_energy; }
		float GetMaxEnergy() const { return m_maxEnergy; }
		float GetEnergyRecharge() const { return m_energyRecharge; }

		float GetShield() const { return m_shield; }
		float GetMaxShield() const { return m_maxShield; }
		float GetShieldRecharge() const { return m_shieldRecharge; }
		float GetShieldDelay() const { return m_shieldDelay; }

		void RegisterComponents(class SceneGraph& _sceneGraph);

		// Fire all weapons straight forward.
		void Fire();

		// Rotates the weapons to so that when fired the projectile 
		//has its endpoint on the given ray in front of the ship.
		void AdjustWeaponOrientation(const ei::Ray& _targetRay);

		void SetWeaponTarget(const Actor& _actor);

		// Activates the given weapon in _slot, overwriting the current one.
		// When a nullptr is given the slot is just deactivated.
		void SetWeapon(int _slot, class Weapon* _weapon);
		const FixedArray<SocketComponent>& GetWeaponSockets() const { return m_weaponSockets; }
		const Inventory& GetInventory() const { return m_inventory; }
		Inventory& GetInventory() { return m_inventory; }

		void SetSpecialMove(SpecialMove& _sm);
		SpecialMove* GetSpecialMove() const { return m_specialMove.get(); }
	private:
		// actual constructor
		Ship(const Jo::Files::MetaFileWrapper::Node& _node, const Vec3& _position);

		// the power of acceleration
		float m_thrust;
		// the speed at which the ship will fly
		float m_speed;
		// the minimum speed of the ship
		float m_minSpeed;
		// the maximum speed of the ship
		float m_maxSpeed;
		// min spray radius
		float m_minSprayRadius;
		// max spray radius
		float m_maxSprayRadius;
		// the spray radius
		float m_sprayRadius;
		Vec3 m_targetAngularVelocity;
		float m_angularAcceleration;

		float m_maxEnergy;
		float m_energyRecharge;
		float m_energy;

		float m_maxShield;
		float m_shieldRecharge;
		float m_shieldWait; //< time since the last interrupt
		float m_shieldDelay; //< time after being hit until the shield starts
		float m_shield;
		bool m_isRecharging;
		ShieldComponent m_shieldComponent;

		float m_particleSpawnCount;
		FixedArray<ei::Vec3> m_drivePositions;
		FixedArray<ParticleSystemComponent<Graphic::ParticleSystems::BASIC_SYSTEM, true>> m_thrustParticles;
		FixedArray< PointLightComponent > m_thrustLights;
		FixedArray<SocketComponent> m_weaponSockets;

		Inventory m_inventory;
		std::unique_ptr< class SpecialMove > m_specialMove;
	};
}