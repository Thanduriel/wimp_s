#pragma once

#include <functional>

#include "gameplay/core/actor.hpp"
#include "gameplay/elements/factorycomponent.hpp"
#include "gameplay/elements/audiocomponent.hpp"
#include "gameplay/elements/light.hpp"
#include "projectile.hpp"
#include "item.hpp"

namespace Game {

	class Weapon : public TypeItem<Item::Type::Weapon>, public Actor
	{
	public:
		typedef std::function<void(Weapon&)> FireFunction;
		typedef std::function<void(Weapon&, float)> ReloadFunction;
		typedef std::function<bool(Weapon&, float)> CanFireFunction;

		Weapon(float _cooldown = 1.f, float _range = 2560.f, float _energyCost = 1.f,
			FireFunction&& _fireFn = FireFunction(),
			ReloadFunction&& _reloadFn = ReloadFunction(),
			CanFireFunction&& _canFireFn = CanFireFunction(),
			Item::Quality _quality = Item::Quality::Basic,
			Item::Icon _icon = Item::Icon::DefaultWeapon,
			const std::string& _name = "Default Weapon",
			const std::string& _decsription = "does something");

		void RegisterComponents(class SceneGraph& _sceneGraph) override;

		void Process(float _deltaTime) override;
		// Fires this weapon if it is ready.
		// @param _speed Speed added to a new projectile in addition to its intrinsic speed
		//		  to emulate the weapon itself moving
		// @param _energyAvailable The energy that can be used to fire this weapon.
		// @return The energy consumed.
		float Fire(const ei::Vec3& _velocity, float _energyAvailable);
		void SetTarget(const Actor& _actor) { m_target = _actor.GetHandle(); }
		void SetOwner(const void* _owner) { m_owner = _owner; }

		float GetEnergyCost() const { return m_energyCost; }
		float GetRange() const { return m_range; }
		float GetCooldown() const { return m_cooldown; }

		struct AccumulatedStats
		{
			float burstDPS;
			float burstEPS;
			float sustainedDPS;
			float sustainedEPS;
		};
		const AccumulatedStats& GetAccumulatedStats() const { return m_accumulatedStats; }
		// should this weapon spawn particles when firing?
		void ShowMuzzleParticles(bool _show) { m_showMuzzleParticles = _show; }
	protected:
		float m_cooldown;
		float m_cooldownMax;
		float m_energyCost;
		float m_range;
		ei::Vec3 m_beginVelocity;
	private:
		void ComputeStats();
		std::pair<float, float> SimulateFire(float _duration);

		FactoryComponent m_factoryComponent;
		AudioComponent m_audioComponent;
		ParticleSystemComponent<Graphic::ParticleSystems::BASIC_SYSTEM> m_muzzleParticles;
		bool m_showMuzzleParticles;

		FireFunction m_fireImpl;
		ReloadFunction m_reloadImpl;
		CanFireFunction m_canFireImpl;

		Actor::ConstHandle m_target;
		const void* m_owner;

		AccumulatedStats m_accumulatedStats;

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
	//	static Weapon::FireFunction FireTripple(GenerationFunction&& _generator);
		static Weapon::FireFunction FireIterative(GenerationFunction&& _generator);

		// fire condition traits
		static bool CanFireDefault(Weapon& _this, float _energyAvailable);
		static bool CanFireInfinite(Weapon& _this, float _energyAvailable);

		// Encapsulates the creation of projectiles based on a prototype
		template<typename T>
		static GenerationFunction CreateProjectileFn(const T& _prototype)
		{
			return [=](Weapon& _weapon) -> Projectile& 
			{
				return _weapon.m_factoryComponent.CopyP<T>(_prototype); 
			};
		}

		// Specialization for rockets that transmits the current target of the weapon
		static GenerationFunction CreateProjectileFnRocket(const Rocket& _prototype)
		{
			return [=](Weapon& _weapon) -> Projectile&
			{
				Game::Rocket& proj = _weapon.m_factoryComponent.CopyP<Rocket>(_prototype);
				if(_weapon.m_target && *_weapon.m_target) proj.m_target = _weapon.m_target;
				return proj;
			};
		}
	private:

		static void SetUpProjectile(Projectile& _proj, const Weapon& _weapon, const ei::Vec3& _vel);
	};
}