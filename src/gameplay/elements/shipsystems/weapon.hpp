#pragma once

#include <functional>

#include "gameplay/core/actor.hpp"
#include "gameplay/elements/factorycomponent.hpp"
#include "projectile.hpp"

namespace Game {

	class Weapon : public Actor
	{
	public:
		typedef std::function<void(Weapon&)> FireFunction;
		typedef std::function<void(Weapon&, float)> ReloadFunction;

		Weapon(float _cooldown = 1.f, float _range = 100.f, float _energyCost = 1.f, 
			FireFunction&& _fireFn = FireFunction(),
			ReloadFunction&& _reloadFn = ReloadFunction());

		void RegisterComponents(class SceneGraph& _sceneGraph) override;

		void Process(float _deltaTime) override;
		// Fires this weapon if it is ready.
		// @param _speed Speed added to a new projectile in addition to its intrinsic speed
		//		  to emulate the weapon itself moving
		// @param _energyAvailable The energy that can be used to fire this weapon.
		// @return The energy consumed.
		float Fire(float _speed, float _energyAvailable);

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
		typedef std::function<Projectile&(Weapon&)> GenerationFunction;

		// reload behavior altering traits
		static Weapon::ReloadFunction ReloadDefault();
		static Weapon::ReloadFunction ReloadBurstFire(float _highFactor = 2.5f, float _lowFactor = 0.25f, int _numShots = 10);
		static Weapon::ReloadFunction ReloadBuildUp(float _speedUp = 2.f, int _numShots = 6);

		// fire traits
		// this functions require a generator function that can spawn a single projectile.
		static Weapon::FireFunction FireDefault(GenerationFunction&& _generator);
		static Weapon::FireFunction FireDouble(GenerationFunction&& _generator);

		// Encapsulates the creation of projectiles based on a prototype
		template<typename T>
		static GenerationFunction CreateProjectileFn(const T& _prototype)
		{
			return [=](Weapon& _weapon) -> Projectile& 
			{
				return _weapon.m_factoryComponent.CopyP<T>(_prototype); 
			};
		}
	private:

		static void SetUpProjectile(Projectile& _proj, const Weapon& _weapon, const ei::Vec3& _vel);
	};
}