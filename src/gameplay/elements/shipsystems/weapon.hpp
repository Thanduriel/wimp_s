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

		Weapon(float _cooldown = 1.f, float _damage = 10.f, 
			FireFunction&& _fireFn = FireFunction(),
			ReloadFunction&& _reloadFn = ReloadFunction());

		void RegisterComponents(class SceneGraph& _sceneGraph) override;

		void Process(float _deltaTime) override;
		// Fires this weapon if it is ready.
		void Fire();

		float GetEnergyCost() const { return m_energyCost; }
	protected:
		float m_cooldown;
		float m_cooldownMax;
		float m_damage;
		float m_energyCost;
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
		static Weapon::FireFunction FireDefault();
		static Weapon::FireFunction FireDouble();
	};

}