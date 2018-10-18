#pragma once

#include "shieldgen.hpp"
#include "utils/stringutils.hpp"

using namespace Utils;
using namespace Game;

namespace Generators {

	enum ShieldTraitType {
		ExtMax,
		ExtDelay,
		ExtRecharge,
		FastCharge,
		Resistance,
		Repair,
		Absorb,
		Nova,
		Adaptive,
		Deflection,
		STTCOUNT
	};

	constexpr std::array<ShieldTraitType, 3> TAKE_DAMAGE_TRAITS = 
	{
		Resistance, Absorb, Deflection
	};

	constexpr std::array<ShieldTraitType, 1> RECHARGE_TRAITS =
	{
		Repair
	};

	const std::array< TraitDescription, STTCOUNT> SHIELD_TRAITS =
	{ {
		{"", "%.0f%% max shield", true},
		{"", "%.0f%% shorter delay", true},
		{"", "%.0f%% faster recharge", true},
		{"Quick Charge", "3x faster recharge, 2x slower start%", true},
		{"of Resistance", "reduces damage taken by %.0f%%", false },
		{"Repairing", "regenerates %.1f%% health per second", true },
		{"of Absorption", "gain %.0f%% of damage taken as energy", false },
		{"Nova", "releases an explosion when empty", true },
		{"Adaptive", "reduces damage taken by 5% for 4s when hit", true },
		{"Deflector", "hits can deal no more than %0.f%% of max shield as damage", false}
	} };

	ShieldGenerator::ShieldGenerator(uint32_t _seed)
		: ItemGenerator(STTCOUNT, _seed)
	{}

	Shield* ShieldGenerator::Generate(uint32_t _seed, float _power, float _qualityFactor)
	{
		m_rng.seed(_seed);

		return Generate(_power, _qualityFactor);
	}

	Shield* ShieldGenerator::Generate(float _power, float _qualityFactor)
	{
		Reset();

		const Item::Quality rarity = RollRarity(_qualityFactor);
		int numTraits = QUALITY_NUM_TRAITS[(int)rarity];

		m_name = "Shield";

		float maxShield = m_randomSampler.Uniform(5.f, 10.f);
		float delay = m_randomSampler.Uniform(2.f, 8.f);
		float recharge = m_randomSampler.Uniform(0.2f, 3.f);

		Shield::TakeDamageFunction takeDamageFn;
		Shield::RechargeFunction rechargeFn;

		while (numTraits)
		{
			const ShieldTraitType trait = (ShieldTraitType)m_randomSampler.Uniform(0, STTCOUNT - 1);
			if (m_hasTrait[trait]) continue; // every trait can only occur once
			--numTraits;

			m_hasTrait[trait] = 1;
			m_isTraitInfoSet = false;

			float temp;
			switch (trait)
			{
			case ExtMax:
				temp = GenerateValue(0.10f, 0.25f, 0.01f);
				maxShield *= (1.f + temp);
				AddTrait(SHIELD_TRAITS[trait], 100.f * temp);
				break;
			case ExtDelay:
				temp = GenerateValue(0.08f, 0.20f, 0.01f);
				delay *= (1.f - temp);
				AddTrait(SHIELD_TRAITS[trait], 100.f * temp);
				break;
			case ExtRecharge:
				temp = GenerateValue(0.05f, 0.15f, 0.01f);
				recharge *= (1.f + temp);
				AddTrait(SHIELD_TRAITS[trait], 100.f * temp);
				break;
			case FastCharge:
				recharge *= 3.f;
				delay *= 2.f;
				break;
			case Resistance:
				temp = GenerateValue(0.05f, 0.25f, 0.01f);
				takeDamageFn = ShieldTrait::ReduceDamageRelative(temp);
				AddTrait(SHIELD_TRAITS[trait], 100.f * temp);
				AddTraitGroup(TAKE_DAMAGE_TRAITS);
				break;
			case Deflection:
				temp = GenerateValue(0.25f, 0.50f, 0.01f);
				takeDamageFn = ShieldTrait::ReduceDamageMax(temp);
				AddTrait(SHIELD_TRAITS[trait], 100.f * temp);
				AddTraitGroup(TAKE_DAMAGE_TRAITS);
				break;
			case Absorb:
				temp = GenerateValue(0.08f, 0.16f, 0.01f);
				takeDamageFn = ShieldTrait::AbsorbEnergyRelative(temp);
				AddTrait(SHIELD_TRAITS[trait], 100.f * temp);
				AddTraitGroup(TAKE_DAMAGE_TRAITS);
				break;
			case Repair:
				temp = GenerateValue(0.005f, 0.02, 0.01f);
				rechargeFn = ShieldTrait::RechargeRepair(temp);
				AddTrait(SHIELD_TRAITS[trait], temp * 100.f);
				AddTraitGroup(RECHARGE_TRAITS);
			}
		}

		m_baseStats += "max shield:       " + ToConstDigit(maxShield, 1, 4) + "\n";
		m_baseStats += "recharge delay:   " + ToConstDigit(delay, 1, 4) + "\n";
		m_baseStats += "recharge speed:   " + ToConstDigit(recharge, 1, 4) + "\n";

		m_description = m_baseStats + "-----" + m_description;

		m_name = GetName(m_name);

		static int count = 0;
		++count;
		return new Game::Shield(rarity, count % 2 ? Game::Item::Icon::DefaultShield : Game::Item::Icon::StrongShield, m_name, m_description,
			maxShield,
			recharge,
			delay,
			std::move(takeDamageFn),
			std::move(rechargeFn));
		{}
	}
}