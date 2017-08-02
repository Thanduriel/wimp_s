#include "weapon.hpp"
#include "gameplay/scenegraph.hpp"
#include "projectile.hpp"

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
					x = ei::max(0.f, x - _deltaTime / _weapon.m_cooldownMax);
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
			vel.z += _weapon.m_beginSpeed;
			SetUpProjectile(proj, _weapon, vel);
			//	proj.GetCollisionComponent().SetType(CollisionComponent::Type::NonPlayer);
		};
	}

	Weapon::FireFunction WeaponTrait::FireDouble(GenerationFunction&& _generator)
	{
		return [=](Weapon& _weapon)
		{
			Projectile& proj1 = _generator(_weapon);
			ei::Vec3 vel = proj1.GetVelocity();
			vel.z += _weapon.m_beginSpeed;
			SetUpProjectile(proj1, _weapon, vel);
			proj1.Translate(_weapon.GetRotationMatrix() * ei::Vec3(-0.4f, 0.f, 0.f));

			Projectile& proj2 = _generator(_weapon);
			SetUpProjectile(proj2, _weapon, vel);
			proj2.Translate(_weapon.GetRotationMatrix() * ei::Vec3(0.4f, 0.f, 0.f));
		};
	}

	void WeaponTrait::SetUpProjectile(Projectile& _proj, const Weapon& _weapon, const ei::Vec3& _vel)
	{
		_proj.SetVelocity(_weapon.GetRotationMatrix() * _vel);
		_proj.SetRotation(ei::Quaternion(ei::Vec3(0.f, 0.f, 1.f), _proj.GetVelocity()));
	}

	// ********************************************************************* //
	Weapon::Weapon(float _cooldown, float _range, float _energyCost, FireFunction&& _fireFn, ReloadFunction&& _reloadFn)
		: Actor(ei::Vec3()),
		m_factoryComponent(THISACTOR),
		m_cooldown(0.f),
		m_cooldownMax(_cooldown),
		m_range(_range),
		m_energyCost(_energyCost),
		m_beginSpeed(0.f),
		m_reloadImpl(_reloadFn ? std::move(_reloadFn) : WeaponTrait::ReloadDefault()),
		m_fireImpl(_fireFn ? std::move(_fireFn) : WeaponTrait::FireDefault(WeaponTrait::CreateProjectileFn(Bolt(ei::Vec3(0.f), ei::Vec3(0.f,0.f,82.f),5.f, 10.f)))),
		m_target(nullptr)
	{

	}

	void Weapon::RegisterComponents(SceneGraph& _sceneGraph)
	{
		_sceneGraph.RegisterComponent(m_factoryComponent);
	}

	void Weapon::Process(float _deltaTime)
	{
		Actor::Process(_deltaTime);

		m_reloadImpl(*this, _deltaTime);
	}

	float Weapon::Fire(float _speed, float _energyAvailable)
	{
		if (m_cooldown <= 0.f && m_energyCost < _energyAvailable)
		{
			m_beginSpeed = _speed;
			m_cooldown = m_cooldownMax;
			m_fireImpl(*this);

			return m_energyCost;
		}
		return 0.f;
	}
}