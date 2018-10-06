#pragma once

#include "itemgen.hpp"
#include "gameplay/elements/shipsystems/shield.hpp"

namespace Generators {

	class ShieldGenerator : public ItemGenerator
	{
	public:
		ShieldGenerator(uint32_t _seed = 0x0);

		/* Generate() **********************
		 * Generates a random weapon.
		 * @param _qualityShift Modifier to the likeliness of quality classes.
		 *		  A factor of 2 means that the higher classes are twice as likely.
		 * @param _power The overall strength of the weapon in form of damage per energy seconds:
		 *		 p = damage / (energy * cooldown)
		 */
		Game::Shield* Generate(float _power, float _qualityFactor = 1.f);
		Game::Shield* Generate(uint32_t _seed, float _power, float _qualityFactor = 1.f);

	private:
	};
}