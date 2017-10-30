#pragma once

#include "itemgen.hpp"
#include "random.hpp"
#include "gameplay/elements/shipsystems/weapon.hpp"

namespace Generators{

	class WeaponGenerator
	{
	public:
		WeaponGenerator(uint32_t _seed = 0x0);

		/* Generate() **********************
		 * Generates a random weapon.
		 * @param _qualityShift Modifier to the likeliness of quality classes.
		 *		  A factor of 2 means that the higher classes are twice as likely.
		 * @param _power The overall strength of the weapon in form of damage per energy seconds:
		 *		 p = damage / (energy * cooldown)
		 */
		Game::Weapon* Generate(float _power, float _qualityFactor = 1.f);
		Game::Weapon* Generate(uint32_t _seed, float _power, float _qualityFactor = 1.f);

	private:
		static Range GetDamageRange(Range _base, float _power);
		static Range GetCooldownRange(Range _base, float _power);

		void AddTrait(const TraitDescription& _traitDescr);

		std::string m_name;
		std::string m_baseStats;
		std::string m_description;

		DefaultRandomGen m_rng;
		RandomSampler<> m_randomSampler;
	};
}