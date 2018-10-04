#include "shield.hpp"
#include "gameplay/elements/ship.hpp"

namespace Game {

	Shield::Shield(Item::Quality _quality,
		Item::Icon _icon,
		const std::string& _name,
		const std::string& _description,
		float _maxShield, float _recharge, float _delay)
		: TypeItem(_quality, _icon, _name, _description),
		m_shieldRecharge(_recharge),
		m_shieldDelay(_delay)
	{
		m_maxShield = _maxShield;
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
}