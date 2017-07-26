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

	Weapon::FireFunction WeaponTrait::FireDouble()
	{
		return [](Weapon& _weapon)
		{
	/*		Rocket& rocket = _weapon.m_factoryComponent.MakeP<Rocket>(ei::Vec3(-0.2f,0.f,0.f),
				_weapon.GetRotationMatrix() * ei::Vec3(0.f, 0.f, 12.f), _weapon.m_damage);
			rocket.m_model.SetType(CollisionComponent::Type::NonPlayer);

			Rocket& rocket2 = _weapon.m_factoryComponent.MakeP<Rocket>(ei::Vec3(0.2f, 0.f, 0.f),
				_weapon.GetRotationMatrix() * ei::Vec3(0.f, 0.f, 12.f), _weapon.m_damage);
			rocket2.m_model.SetType(CollisionComponent::Type::NonPlayer);*/
		};
	}

	void WeaponTrait::SetUpProjectile(Projectile& _proj, const Weapon& _weapon, const ei::Vec3& _vel)
	{
		_proj.SetVelocity(_weapon.GetRotationMatrix() * _vel);
		_proj.SetRotation(ei::Quaternion(ei::Vec3(0.f, 0.f, 1.f), _proj.GetVelocity()));
	}

	// ********************************************************************* //
	Weapon::Weapon(float _cooldown, float _range, FireFunction&& _fireFn, ReloadFunction&& _reloadFn)
		: Actor(ei::Vec3()),
		m_factoryComponent(THISACTOR),
		m_cooldown(0.f),
		m_cooldownMax(_cooldown),
		m_range(_range),
		m_energyCost(1.f),
		m_beginSpeed(0.f),
		m_reloadImpl(_reloadFn ? std::move(_reloadFn) : WeaponTrait::ReloadDefault()),
		m_fireImpl(_fireFn ? std::move(_fireFn) : WeaponTrait::FireDefault(Bolt(ei::Vec3(0.f), ei::Vec3(0.f,0.f,82.f),5.f, 10.f)))
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

	void Weapon::Fire()
	{
		if (m_cooldown <= 0.f)
		{
			m_cooldown = m_cooldownMax;
			m_fireImpl(*this);
		}
	}
}