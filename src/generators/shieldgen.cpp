#pragma once

#include "shieldgen.hpp"
#include "utils/stringutils.hpp"

using namespace Utils;

namespace Generators {

	using namespace Game;

	Shield* ShieldGenerator::Generate(uint32_t _seed, float _power, float _qualityFactor)
	{
		m_rng.seed(_seed);

		return Generate(_power, _qualityFactor);
	}

	Shield* ShieldGenerator::Generate(float _power, float _qualityFactor)
	{
		m_description.clear();
		m_baseStats.clear();

		const Item::Quality rarity = RollRarity(_qualityFactor);

		m_name = "shield";

		float maxShield = m_randomSampler.Uniform(5.f, 10.f);
		float delay = m_randomSampler.Uniform(2.f, 8.f);
		float recharge = m_randomSampler.Uniform(0.2f, 3.f);

		m_baseStats += "max shield:       " + ToConstDigit(maxShield, 1, 4) + "\n";
		m_baseStats += "recharge delay:   " + ToConstDigit(delay, 1, 4) + "\n";
		m_baseStats += "recharge speed:   " + ToConstDigit(recharge, 1, 4) + "\n";

		m_description = m_baseStats;

		return new Game::Shield(rarity, Game::Item::Icon::DefaultShield, m_name, m_description,
			maxShield,
			recharge,
			delay);
		{}
	}
}