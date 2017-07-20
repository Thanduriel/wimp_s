#pragma once

#include "itemgen.hpp"
#include "random.hpp"
#include "gameplay/elements/shipsystems/weapon.hpp"

namespace Generators{



	const std::array< TraitDescription, 2> WEAPON_TRAITS =
	{ {
		{"Burst", "cd increases with continues fire", true},
		{"Twin", "fires additional projectile", true }
	} };

	enum struct WeaponType {
		Simple,
		Rocket,
		Ray, // not implemented
		Count
	};

	const std::array< Range, (size_t)WeaponType::Count> COOLDOWN_RANGE =
	{ {
		{0.2f, 2.f},
		{2.f, 6.f}
	} };

	const std::array< Range, (size_t)WeaponType::Count> DAMAGE_RANGE =
	{ {
		{}
	} };

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

		// Returns the description of the last item.
		// Only temporary, this should be handled by Item
		const std::string& GetDescr() { return m_description; }
		const std::string& GetName() { return m_name; }
	private:
		void AddTrait(const TraitDescription& _traitDescr);

		std::string m_name;
		std::string m_description;

		DefaultRandomGen m_rng;
		RandomSampler<> m_randomSampler;
	};
}