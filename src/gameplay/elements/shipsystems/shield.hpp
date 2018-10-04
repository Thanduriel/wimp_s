#pragma once

#include "gameplay/core/actor.hpp"
#include "item.hpp"

namespace Game {

	class Shield : public TypeItem<Item::Type::Shield>, public Actor
	{
	public:
		Shield(Item::Quality _quality = Item::Quality::Basic,
			Item::Icon _icon = Item::Icon::DefaultWeapon,
			const std::string& _name = "Default Shield",
			const std::string& _description = "absorbs some damage",
			float _maxShield = 10.f, float _recharge = 1.f, float _delay = 4.f);

		void OnDepletion();
		void OnFullCharge();
		void OnDamageTaken();

		void Equip(Ship& _ship) const override;
		void UnEquip(Ship& _ship) const override;
	private:
	//	float m_maxShield;
		float m_shieldRecharge;
		float m_shieldDelay;
	};
}