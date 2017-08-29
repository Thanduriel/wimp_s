#pragma once

#include <string>
#include <array>
#include "utils/color.hpp"

namespace Game {

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

		static const std::array<Utils::Color32F, (size_t)Quality::COUNT> QUALITY_COLOR;
		static const std::array<std::string, (size_t)Quality::COUNT> QUALITY_COLOR_STR;
		static const std::array<int, (size_t)Quality::COUNT> QUALITY_VALUE;

		Item(Quality _quality, const std::string& _name, const std::string& _description, int _value = -1);

		Quality GetQuality() const { return m_quality; }
		const std::string& GetName() const { return m_name; }
		const std::string& GetDescription() const { return m_description; }
		int GetValue() const { return m_value; }
	protected:
		Quality m_quality;
		std::string m_name;
		std::string m_description;
		int m_value;
	};
}