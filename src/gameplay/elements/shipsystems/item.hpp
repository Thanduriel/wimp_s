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

		Item(Quality _quality, const std::string& _name, const std::string& _description)
			: m_quality(_quality),
			m_name(_name),
			m_description(_description)
		{}

		Quality GetQuality() const { return m_quality; }
		const std::string& GetName() const { return m_name; }
		const std::string& GetDescription() const { return m_description; }

	protected:
		Quality m_quality;
		std::string m_name;
		std::string m_description;
	};
}