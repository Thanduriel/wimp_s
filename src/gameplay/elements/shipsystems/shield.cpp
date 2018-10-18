#include "shield.hpp"
#include "gameplay/elements/ship.hpp"

namespace Game {

	Shield::Shield(Item::Quality _quality,
		Item::Icon _icon,
		const std::string& _name,
		const std::string& _description,
		float _maxShield, float _recharge, float _delay,
		TakeDamageFunction&& _takeDamageFn,
		RechargeFunction&& _rechargeFn)
		: TypeItem(_quality, _icon, _name, _description),
		m_shieldRecharge(_recharge),
		m_shieldDelay(_delay),
		m_takeDamageImpl(_takeDamageFn ? std::move(_takeDamageFn) : ShieldTrait::TakeDamageDefault),
		m_rechargeImpl(_rechargeFn ? std::move(_rechargeFn) : ShieldTrait::RechargeDefault)
	{
		m_maxShield = _maxShield;
	}

	void Shield::Process(float _deltaTime)
	{
		Actor::Process(_deltaTime);
	}

	void Shield::Equip(Ship& _ship) const
	{
		_ship.SetShieldDelay(m_shieldDelay);
	//	_ship.SetMaxShield(m_maxShield + _ship.GetMaxShield());
		_ship.SetShieldRecharge(m_shieldRecharge);

		Item::Equip(_ship);
	}

	void Shield::UnEquip(Ship& _ship) const
	{
		_ship.SetShieldRecharge(0.f);
		_ship.SetShieldDelay(100000.f);

		Item::UnEquip(_ship);
	}

	float Shield::TakeDamage(float _damage)
	{
		return m_takeDamageImpl(*this, _damage);
	}

	float Shield::Recharge(float _deltaTime)
	{
		return m_rechargeImpl(*this, _deltaTime);
	}

	// ********************************************************************* //
	float ShieldTrait::RechargeDefault(Shield& _this, float _deltaTime)
	{
		return _deltaTime;
	}

	float ShieldTrait::TakeDamageDefault(Shield& _this, float _damage)
	{
		return _damage;
	}

	// ********************************************************************* //
	Shield::RechargeFunction ShieldTrait::RechargeRepair(float _amount)
	{
		return [=](Shield& _this, float _deltaTime)
		{
			const float regen = _this.m_ship->GetMaxHealth() * _amount * _deltaTime;
			_this.m_ship->SetHealth(std::min(_this.m_ship->GetHealth() + regen, 
				_this.m_ship->GetMaxHealth()));

			return _deltaTime;
		};
	}

	// ********************************************************************* //
	Shield::TakeDamageFunction ShieldTrait::ReduceDamageRelative(float _damageReduction)
	{
		return [=](Shield& _this, float _damage)
		{
			return _damage * _damageReduction;
		};
	}

	Shield::TakeDamageFunction ShieldTrait::ReduceDamageAbsolute(float _damageReduction)
	{
		return [=](Shield& _this, float _damage)
		{
			return std::max(0.f, _damage - _damageReduction);
		};
	}

	Shield::TakeDamageFunction ShieldTrait::ReduceDamageMax(float _damageCap)
	{
		return [=](Shield& _this, float _damage)
		{
			if (_this.GetShip().GetShield()) 
				return std::min(_this.GetShip().GetMaxShield() * _damageCap, _damage);

			return _damage;
		};
	}

	Shield::TakeDamageFunction ShieldTrait::AbsorbEnergyRelative(float _energyRatio)
	{
		return [=](Shield& _this, float _damage)
		{
			_this.m_ship->SetEnergy(_this.m_ship->GetEnergy() + _energyRatio * _damage);

			return _damage;
		};
	}
}