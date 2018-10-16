#pragma once

#include "shieldgen.hpp"
#include "utils/stringutils.hpp"

using namespace Utils;

namespace Generators {

	enum ShieldTraitType {
		ExtC,
		ExtR,
		ExtD,
		STTCOUNT
	};

	using namespace Game;

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
		m_description.clear();
		m_baseStats.clear();

		const Item::Quality rarity = RollRarity(_qualityFactor);

		m_name = "Shield";

		float maxShield = m_randomSampler.Uniform(5.f, 10.f);
		float delay = m_randomSampler.Uniform(2.f, 8.f);
		float recharge = m_randomSampler.Uniform(0.2f, 3.f);

		m_baseStats += "max shield:       " + ToConstDigit(maxShield, 1, 4) + "\n";
		m_baseStats += "recharge delay:   " + ToConstDigit(delay, 1, 4) + "\n";
		m_baseStats += "recharge speed:   " + ToConstDigit(recharge, 1, 4) + "\n";

		m_description = m_baseStats;

		static int count = 0;
		++count;
		return new Game::Shield(rarity, count % 2 ? Game::Item::Icon::DefaultShield : Game::Item::Icon::StrongShield, m_name, m_description,
			maxShield,
			recharge,
			delay);
		{}
	}
}