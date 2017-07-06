#pragma once

#include "itemgen.hpp"
#include "random.hpp"
#include "gameplay/elements/shipsystems/weapon.hpp"

namespace Generators{
	class WeaponGenerator
	{
	public:
		WeaponGenerator();

		/* Generate() **********************
		 * Generates a random weapon.
		 * @param _qualityShift Modifier to the likeliness of quality classes.
		 *		  A factor of 2 means that the higher classes are twice as likely.
		 */
		Game::Weapon* Generate(float _qualityFactor = 1.f);
	private:
		DefaultRandomGen m_rng;
		RandomSampler<> m_randomSampler;
	};
}