#pragma once
#include <array>
#include <string>
#include "ei/vector.hpp"

// todo: move this closer to Game::Item(does not exist yet)
enum struct Quality : int
{
	Basic,
	Advanced,
	Premium,
	Unique,
	COUNT
};

namespace Generators {
	
	// probability for an item to be of the given quality
	// The different classes are rolled beginning with Unique.
	// Thus the actual probability differs slightly.
	const ei::Vec<float, (size_t)Quality::COUNT> QUALITY_RARITY =
	{
		1.f,
		0.2f,
		0.08f, //0.04
		0.04f//0.01
	};

	// Some score that is used to decide the general strength of the stats
	const std::array<std::pair<float, float>, (size_t)Quality::COUNT> QUALITY_BASESTAT =
	{ {
		{11.f, 20.f},
		{13.f, 22.f},
		{16.f, 24.f},
		{18.f, 25.f}
	} };

	const std::array<std::string, (size_t)Quality::COUNT> QUALITY_COLOR = 
	{
		"<c 255 255 255>",
		"<c 0 119 200>",
		"<c 220 235 32>",
		"<c 222 75 44>"
	};

	const std::array<int, (size_t)Quality::COUNT> QUALITY_NUM_TRAITS =
	{
		0,
		1,
		2,
		3
	};

	struct TraitDescription
	{
		std::string name;
		std::string description;
		bool isPrefix;
	};

	typedef std::pair<float, float> Range;
}