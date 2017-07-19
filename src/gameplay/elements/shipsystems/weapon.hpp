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

		Weapon(float _cooldown = 1.f, float _damage = 1.f, 
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
		template<typename T>
		static Weapon::FireFunction FireDefault(const T& _prototype)
		{
			return [=](Weapon& _weapon)
			{
				T& proj = _weapon.m_factoryComponent.CopyP<T>(_prototype);
				proj.SetVelocity(_weapon.GetRotationMatrix() * proj.GetVelocity());
				proj.SetRotation(ei::Quaternion(ei::Vec3(0.f, 0.f, 1.f), proj.GetVelocity()));
				proj.GetCollisionComponent().SetType(CollisionComponent::Type::NonPlayer);
			};
		}
		static Weapon::FireFunction FireDouble();
	};
}