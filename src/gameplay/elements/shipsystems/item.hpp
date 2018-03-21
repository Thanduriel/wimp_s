#pragma once

#include <string>
#include <array>
#include "utils/color.hpp"

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

		bool IsEquiped() const { return m_isEquiped; }

		virtual void Equip(Ship& _ship) const;
		virtual void UnEquip(Ship& _ship) const;
	protected:
		Quality m_quality;
		Icon m_icon;
		std::string m_name;
		std::string m_description;
		int m_value;

		// global buffs
		mutable bool m_isEquiped;
		float m_maxEnergy;
	};
}