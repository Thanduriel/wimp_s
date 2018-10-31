#pragma once

#include <string>
#include <array>
#include "utils/color.hpp"

namespace Generators {
	class WeaponGenerator;
	class ShieldGenerator;
}

namespace Game {

	class Ship;

	class Item
	{
	public:
		enum struct Quality : int
		{
			Basic,
			Advanced,
			Premium,
			Unique,
			COUNT
		};

		enum struct Icon : int
		{
			DefaultWeapon,
			Missile,
			DefaultShield,
			StrongShield,
			COUNT
		};
		enum struct Type : int
		{
			Weapon,
			Shield,
			 COUNT
		};

		static const std::array<Utils::Color32F, (size_t)Quality::COUNT> QUALITY_COLOR;
		static const std::array<std::string, (size_t)Quality::COUNT> QUALITY_COLOR_STR;
		static const std::array<int, (size_t)Quality::COUNT> QUALITY_VALUE;
		static const std::array<std::string, (size_t)Icon::COUNT> ICON_STR;

		static constexpr int USE_QUALITY = -1;
		Item(Quality _quality, Icon _icon, const std::string& _name, const std::string& _description, int _value = USE_QUALITY);

		Quality GetQuality() const { return m_quality; }
		Icon GetIcon() const { return m_icon; }
		const std::string& GetName() const { return m_name; }
		const std::string& GetDescription() const { return m_description; }
		int GetValue() const { return m_value; }

		virtual Item::Type GetType() const = 0;
		bool IsEquiped() const { return m_ship; }
		virtual void Equip(Ship& _ship) const;
		virtual void UnEquip(Ship& _ship) const;
	protected:
		Quality m_quality;
		Icon m_icon;
		std::string m_name;
		std::string m_description;
		int m_value;
		mutable Ship* m_ship; // ship that currently owns this item, nullptr when not equipped

		// global buffs
		float m_maxEnergy = 0.f;
		float m_maxShield = 0.f;
		float m_maxHealth = 0.f;
		float m_energyRecharge = 0.f;

		friend class Generators::WeaponGenerator;
		friend class Generators::ShieldGenerator;
	};

	template<Item::Type _Type>
	class TypeItem : public Item
	{
	public:
		using Item::Item;

		Item::Type GetType() const final
		{
			return _Type;
		}
	};
}