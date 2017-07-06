#include <ctime>
#include "weapongen.hpp"

namespace Generators {
	WeaponGenerator::WeaponGenerator()
		: m_rng(clock()),
		m_randomSampler(m_rng)
	{
	}

	using namespace Game;

	Game::Weapon* WeaponGenerator::Generate(float _qualityFactor)
	{
		// decide on quality
		auto qVec = QUALITY_RARITY * _qualityFactor;
		
		Quality rarity = Quality::Unique;
		while (float n = m_randomSampler.Uniform() > qVec[(int)rarity]) rarity = Quality((int)rarity + 1);

		Weapon::FireFunction fireFn;
		Weapon::ReloadFunction reloadFn;

		if (rarity == Quality::Unique)
		{
			switch (m_randomSampler.Uniform(0, 1))
			{
			case 0: fireFn = WeaponTrait::FireDouble();
				break;
			case 1: reloadFn = WeaponTrait::ReloadBurstFire(2.f);
			}
		}

		return new Game::Weapon(m_randomSampler.Uniform(0.5f, 2.5f),
			m_randomSampler.Uniform(QUALITY_BASESTAT[(int)rarity].first, QUALITY_BASESTAT[(int)rarity].second),
			std::move(fireFn),
			std::move(reloadFn));
	}
}