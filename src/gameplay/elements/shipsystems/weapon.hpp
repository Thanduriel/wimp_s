#pragma once

#include <functional>

#include "gameplay/core/actor.hpp"
#include "gameplay/elements/factorycomponent.hpp"

namespace Game {

	class Weapon : public Actor
	{
	public:
		typedef std::function<void(Weapon&)> FireFunction;
		typedef std::function<void(Weapon&, float)> ReloadFunction;

		Weapon(float _cooldown = 1.f, float _range = 100.f, 
			FireFunction&& _fireFn = FireFunction(),
			ReloadFunction&& _reloadFn = ReloadFunction());

		void RegisterComponents(class SceneGraph& _sceneGraph) override;

		void Process(float _deltaTime) override;
		// Fires this weapon if it is ready.
		void Fire();

		// Since a Weapon is not dynamic it needs a way to convey the current speed
		// of the ship to spawned projectiles.
		void SetBeginSpeed(float _beginSpeed) { m_beginSpeed = _beginSpeed; }

		float GetEnergyCost() const { return m_energyCost; }
		float GetRange() const { return m_range; }
	protected:
		float m_cooldown;
		float m_cooldownMax;
		float m_energyCost;
		float m_range;
		float m_beginSpeed;
	private:
		FactoryComponent m_factoryComponent;
		
		FireFunction m_fireImpl;
		ReloadFunction m_reloadImpl;

		friend class WeaponTrait;
	};

	class WeaponTrait
	{
	public:
		// reload behavior altering traits
		static Weapon::ReloadFunction ReloadDefault();
		static Weapon::ReloadFunction ReloadBurstFire(float _highFactor = 2.f, float _lowFactor = 0.25f, int _numShots = 10);

		// fire traits
		template<typename T>
		static Weapon::FireFunction FireDefault(const T& _prototype)
		{
			return [=](Weapon& _weapon)
			{
				T& proj = _weapon.m_factoryComponent.CopyP<T>(_prototype);
				ei::Vec3 vel = proj.GetVelocity();
				vel.z += _weapon.m_beginSpeed;
				SetUpProjectile(proj, _weapon, vel);
				proj.GetCollisionComponent().SetType(CollisionComponent::Type::NonPlayer);
			};
		}
		static Weapon::FireFunction FireDouble();

	private:
		static void SetUpProjectile(class Projectile& _proj, const Weapon& _weapon, const ei::Vec3& _vel);
	};
}