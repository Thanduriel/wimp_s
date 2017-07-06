#pragma once
#include <array>
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
		0.04f,
		0.01f
	};

	// Some score that is used to decide the general strength of the stats
	const std::array<std::pair<float, float>, (size_t)Quality::COUNT> QUALITY_BASESTAT =
	{ {
		{11.f, 20.f},
		{13.f, 22.f},
		{16.f, 24.f},
		{18.f, 25.f}
	} };
}