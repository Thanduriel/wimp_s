#include <ctime>
#include "weapongen.hpp"
#include "gameplay/elements/shipsystems/projectile.hpp"

namespace Generators {
	WeaponGenerator::WeaponGenerator()
		: m_rng(clock()),
		m_randomSampler(m_rng)
	{
	}

	using namespace Game;

	Game::Weapon* WeaponGenerator::Generate(float _qualityFactor)
	{
		static bool killer = false;
		if (!killer)
		{
			killer = true;
			m_name = "<c 154 153 203>performance killer</c>";
			m_description = "damage: " + std::to_string(std::numeric_limits<float>::infinity())
				+ "\n" + "cooldown: 0.0";
			return new Weapon(0.f);
		}

		// decide on quality
		auto qVec = QUALITY_RARITY * _qualityFactor;
		
		Quality rarity = Quality::Unique;
		while (float n = m_randomSampler.Uniform() > qVec[(int)rarity]) rarity = Quality((int)rarity - 1);

		m_name = "Rocket Launcher";
		m_description.clear();

		float cooldown = m_randomSampler.Uniform(0.5f, 2.5f);
		m_description += "cooldown: " + std::to_string(cooldown) + "s\n";

		float damage = m_randomSampler.Uniform(QUALITY_BASESTAT[(int)rarity].first, QUALITY_BASESTAT[(int)rarity].second);
		m_description += "damage: " + std::to_string(damage) + "\n";

		float speed = m_randomSampler.Uniform(20.f, 50.f);

		Game::Rocket projProto(ei::Vec3(0.f), ei::Vec3(0.f,0.f, speed), damage, 10.f);
		m_description += "speed: " + std::to_string(speed);
		Weapon::FireFunction fireFn;
		Weapon::ReloadFunction reloadFn;

		if (rarity == Quality::Unique)
		{
			switch (m_randomSampler.Uniform(0, 1))
			{
			case 0: fireFn = WeaponTrait::FireDouble();
				AddTrait(WEAPON_TRAITS[1]);
				break;
			case 1: reloadFn = WeaponTrait::ReloadBurstFire();
				AddTrait(WEAPON_TRAITS[0]);
				break;
			}
		}
		if (!fireFn) fireFn = WeaponTrait::FireDefault(projProto);

		m_description += "\n-----\ndps: " + std::to_string(1.f / cooldown * damage)
			+ "\nrange: " + std::to_string(speed * 10.f);

		m_name = QUALITY_COLOR[(int)rarity] + m_name + "</c>";

		return new Game::Weapon(cooldown,
			damage,
			std::move(fireFn),
			std::move(reloadFn));
	}

	void WeaponGenerator::AddTrait(const TraitDescription& _traitDesrc)
	{
		m_name = _traitDesrc.isPrefix ? _traitDesrc.name + " " + m_name
			: m_name + " " + _traitDesrc.name;
		m_description += "\n" + _traitDesrc.description;
	}
}