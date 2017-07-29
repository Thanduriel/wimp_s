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

	const std::array < std::string, (size_t)WeaponType::Count> NAMES =
	{
		"Blaster",
		"Rocket Launcher",
		"Laser Gun"
	};

	// base values for cooldown
	const std::array< Range, (size_t)WeaponType::Count> COOLDOWN_RANGE =
	{ {
		{0.5f, 2.f},
		{2.f, 6.f}
	} };

	// base values for damage
	const std::array< Range, (size_t)WeaponType::Count> DAMAGE_RANGE =
	{ {
		{2.f, 5.f},
		{8.f, 20.f}
	} };

	class WeaponGenerator
	{
	public:
		WeaponGenerator();

		/* Generate() **********************
		 * Generates a random weapon.
		 * @param _qualityShift Modifier to the likeliness of quality classes.
		 *		  A factor of 2 means that the higher classes are twice as likely.
		 * @param _power The overall strength of the weapon in form of damage per energy seconds:
		 *		 p = damage / (energy * cooldown)
		 */
		Game::Weapon* Generate(float _power, float _qualityFactor = 1.f);

		// Returns the description of the last item.
		// Only temporary, this should be handled by Item
		const std::string& GetDescr() { return m_description; }
		const std::string& GetName() { return m_name; }
	private:
		static Range GetDamageRange(Range _base, float _power);
		static Range GetCooldownRange(Range _base, float _power);

		void AddTrait(const TraitDescription& _traitDescr);

		std::string m_name;
		std::string m_description;

		DefaultRandomGen m_rng;
		RandomSampler<> m_randomSampler;
	};
}