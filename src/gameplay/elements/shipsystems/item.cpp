#include "item.hpp"

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
}