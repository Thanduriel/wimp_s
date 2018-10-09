#include "weapon.hpp"
#include "gameplay/scenegraph.hpp"
#include "projectile.hpp"
#include "gameplay/content.hpp"

namespace Game {

	// ********************************************************************* //
	Weapon::ReloadFunction WeaponTrait::ReloadDefault()
	{
		return [](Weapon& _weapon, float _deltaTime)
		{
			_weapon.m_cooldown -= _deltaTime;
		};
	}

	Weapon::ReloadFunction WeaponTrait::ReloadBurstFire(float _highFactor, float _lowFactor, int _numShots)
	{
		struct Functor
		{
			Functor(float _h, float _l, int _n)
				: highFactor(_h), lowFactor(_l), numShots(_n),
				a((lowFactor - highFactor) / (numShots*numShots))
			{}

			float x = 0.f;
			float highFactor;
			float lowFactor;
			int numShots;
			float a;
			void operator()(Weapon& _weapon, float _deltaTime)
			{
				// factor for recovery
				float y = a * x * x + highFactor;
				// effective time reduced
				float f = _deltaTime * y;
				if (_weapon.m_cooldown <= 0.f)
				{
					// cool down to zero
					// factor 2 so that this goes faster
					x = ei::max(0.f, x - _deltaTime * 2.5f / _weapon.m_cooldownMax);
				}
				else // heat up
					x = ei::min(x+f/_weapon.m_cooldownMax, (float)numShots);
				_weapon.m_cooldown -= f;
			}
		};

		return Functor(_highFactor, _lowFactor, _numShots);
	}

	Weapon::ReloadFunction WeaponTrait::ReloadBuildUp(float _speedUp, int _numShots)
	{
		struct Functor
		{
			Functor(float _speedUp, int _numShots) : speedUp(_speedUp-1.f), numShots(_numShots) {}
			float speedUp;
			int numShots;
			float x = 0.f;

			void operator()(Weapon& _weapon, float _deltaTime)
			{
				float y = 1.f + speedUp * x / numShots;
				float f = _deltaTime * y;

				if (_weapon.m_cooldown <= 0.f)
				{
					// cool down to zero
					x = ei::max(0.f, x - _deltaTime/ _weapon.m_cooldownMax);
				}
				else // build up
					x = ei::min(x + f/ _weapon.m_cooldownMax, (float)numShots);

				_weapon.m_cooldown -= f;
			}
		};

		return Functor(_speedUp, _numShots);
	}

	// ********************************************************************* //
	Weapon::FireFunction WeaponTrait::FireDefault(GenerationFunction&& _generator)
	{
		return [=](Weapon& _weapon)
		{
			Projectile& proj = _generator(_weapon);
			ei::Vec3 vel = proj.GetVelocity();
			SetUpProjectile(proj, _weapon, vel);
		};
	}

	Weapon::FireFunction WeaponTrait::FireDouble(GenerationFunction&& _generator)
	{
		return [=](Weapon& _weapon)
		{
			Projectile& proj1 = _generator(_weapon);
			ei::Vec3 vel = proj1.GetVelocity();
			SetUpProjectile(proj1, _weapon, vel);
			proj1.Translate(_weapon.GetRotationMatrix() * ei::Vec3(-0.4f, 0.f, 0.f));

			Projectile& proj2 = _generator(_weapon);
			SetUpProjectile(proj2, _weapon, vel);
			proj2.Translate(_weapon.GetRotationMatrix() * ei::Vec3(0.4f, 0.f, 0.f));
		};
	}

	Weapon::FireFunction WeaponTrait::FireIterative(GenerationFunction&& _generator)
	{
		int count = 0;
		return [=](Weapon& _weapon) mutable
		{
			Projectile& proj = _generator(_weapon);
			ei::Vec3 vel = proj.GetVelocity();
			SetUpProjectile(proj, _weapon, vel);

			++count;
			if(count % 3 == 0)
				proj.m_damage *= 2.f;
		};
	}

	void WeaponTrait::SetUpProjectile(Projectile& _proj, const Weapon& _weapon, const ei::Vec3& _vel)
	{
		_proj.SetVelocity(_weapon.GetRotationMatrix() * _vel + _weapon.m_beginVelocity);
		_proj.SetRotation(ei::Quaternion(ei::Vec3(0.f, 0.f, 1.f), _proj.GetVelocity()));
		_proj.SetOwner(_weapon.m_owner);
	}

	// ********************************************************************* //
	Weapon::Weapon(float _cooldown, float _range, float _energyCost, FireFunction&& _fireFn, ReloadFunction&& _reloadFn,
		Item::Quality _quality, Item::Icon _icon, const std::string& _name, const std::string& _description)
		: Actor(ei::Vec3()),
		TypeItem(_quality, _icon, _name, _description),
		m_factoryComponent(THISACTOR),
		m_audioComponent(THISACTOR),
		m_cooldown(0.f),
		m_cooldownMax(_cooldown),
		m_range(_range),
		m_energyCost(_energyCost),
		m_beginVelocity(0.f),
		m_reloadImpl(_reloadFn ? std::move(_reloadFn) : WeaponTrait::ReloadDefault()),
		m_fireImpl(_fireFn ? std::move(_fireFn) : WeaponTrait::FireDefault(WeaponTrait::CreateProjectileFn(Bolt(ei::Vec3(0.f), ei::Vec3(0.f,0.f,Projectile::DEFAULT_SPEED),5.f, 10.f)))),
		m_target(nullptr)
	{
		ComputeStats();
	}

	void Weapon::RegisterComponents(SceneGraph& _sceneGraph)
	{
		_sceneGraph.RegisterComponent(m_factoryComponent);
		_sceneGraph.RegisterComponent(m_audioComponent);
	}

	void Weapon::Process(float _deltaTime)
	{
		Actor::Process(_deltaTime);

		m_reloadImpl(*this, _deltaTime);
	}

	float Weapon::Fire(const ei::Vec3& _velocity, float _energyAvailable)
	{
		if (m_cooldown <= 0.f && m_energyCost < _energyAvailable)
		{
			m_beginVelocity = _velocity;
			m_cooldown = m_cooldownMax;
			m_fireImpl(*this);

			static const clunk::Sample& sound = Content::GetSound("shoot_blaster");
		//	m_audioComponent.Stop();
			m_audioComponent.Play(sound);

			return m_energyCost;
		}
		return 0.f;
	}

	constexpr float BURST_TIME = 1.f;
	constexpr float SUSTAINED_TIME = 100.f;
	constexpr float TIME_STEP = 0.01f;

	std::pair<float, float> Weapon::SimulateFire(float _duration)
	{
		float damage = 0.f;
		float energy = 0.f;

		for (float t = 0.f; t < _duration; t += TIME_STEP)
		{
			energy += Fire(ei::Vec3(), 10000.f);
			Process(TIME_STEP);
		}
		for (auto& act : m_factoryComponent.m_createdActors)
		{
			Projectile& proj = *static_cast<Projectile*>(act.get());
			damage += proj.GetDamage();
		}

		return { damage, energy };
	}

	void Weapon::ComputeStats()
	{
		m_audioComponent.Mute(true);

		const auto&[damage, energy] = SimulateFire(BURST_TIME);
		m_accumulatedStats.burstDPS = damage / BURST_TIME;
		m_accumulatedStats.burstEPS = energy / BURST_TIME;

		const auto&[damageLong, energyLong] = SimulateFire(SUSTAINED_TIME);
		m_accumulatedStats.sustainedDPS = damageLong / SUSTAINED_TIME;
		m_accumulatedStats.sustainedEPS = energyLong / SUSTAINED_TIME;

		// reload
		Process(20.f);

		// throw away created projectiles
		m_factoryComponent.Reset();
		m_audioComponent.Mute(false);
	}
}