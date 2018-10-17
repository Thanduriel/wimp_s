#include "item.hpp"
#include "../ship.hpp"

namespace Game {

	using namespace Utils;

	const std::array<Color32F, (size_t)Item::Quality::COUNT> Item::QUALITY_COLOR =
	{
		Color32F( 255.f / 255.f, 255.f / 255.f, 255.f / 255.f),
		Color32F( 0.f / 255.f, 119.f / 255.f, 200.f / 255.f),
		Color32F( 220.f / 255.f, 235.f / 255.f, 32.f / 255.f),
		Color32F( 222.f / 255.f, 75.f / 255.f, 44.f / 255.f)
	};

	const std::array<std::string, (size_t)Item::Quality::COUNT> Item::QUALITY_COLOR_STR =
	{
		"<c 255 255 255>",
		"<c 0 119 200>",
		"<c 220 235 32>",
		"<c 222 75 44>"
	};

	const std::array<std::string, (size_t)Item::Icon::COUNT> Item::ICON_STR =
	{
		"weaponIcon",
		"missileIcon",
		"shieldIcon",
		"strongShieldIcon"
	};

	const std::array<int, (size_t)Item::Quality::COUNT> Item::QUALITY_VALUE = 
	{
		60,
		100,
		200,
		500
	};

	Item::Item(Quality _quality, Icon _icon, const std::string& _name, const std::string& _description, int _value)
		: m_quality(_quality),
		m_icon(_icon),
		m_name(_name),
		m_description(_description),
		m_value(_value == USE_QUALITY ? QUALITY_VALUE[(int)_quality] : _value),
		m_ship(nullptr)
	{}

	// ******************************************************************* //
	void Item::Equip(Ship& _ship) const
	{
		if (m_ship) return;
		m_ship = &_ship;
		_ship.SetMaxEnergy(_ship.GetMaxEnergy() + m_maxEnergy);
		_ship.SetMaxShield(_ship.GetMaxShield() + m_maxShield);
		_ship.SetMaxHealth(_ship.GetMaxHealth() + m_maxHealth);
		_ship.SetEnergyRecharge(_ship.GetEnergyRecharge() + m_energyRecharge);
	}

	void Item::UnEquip(Ship& _ship) const
	{
		if (!m_ship) return;

		m_ship = nullptr;
		_ship.SetMaxEnergy(_ship.GetMaxEnergy() - m_maxEnergy);
		_ship.SetMaxShield(_ship.GetMaxShield() - m_maxShield);
		_ship.SetMaxHealth(_ship.GetMaxHealth() - m_maxHealth);
		_ship.SetEnergyRecharge(_ship.GetEnergyRecharge() + m_energyRecharge);
	}
}