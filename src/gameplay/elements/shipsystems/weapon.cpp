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
			float cdMod = 0.f;
			void operator()(Weapon& _weapon, float _deltaTime)
			{
				float fact = cdMod < 1.f ? 0.5f : 2.f;
				fact *= _deltaTime;
				if (_weapon.m_cooldown < 0.f)
				{
					// store additional shots
					cdMod = ei::min(cdMod + _deltaTime * 2.f, 5.f);
				}
				else
					cdMod = ei::max(cdMod - fact, 0.f);
				_weapon.m_cooldown -= fact;
			}
		};

		return Functor();
	}

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
		m_fireImpl(_fireFn ? std::move(_fireFn) : WeaponTrait::FireDefault(WeaponTrait::CreateProjectileFn(Bolt(ei::Vec3(0.f), ei::Vec3(0.f,0.f,82.f),5.f, 10.f))))
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

	void Weapon::Fire(float _speed)
	{
		if (m_cooldown <= 0.f)
		{
			m_beginSpeed = _speed;
			m_cooldown = m_cooldownMax;
			m_fireImpl(*this);
		}
	}
}