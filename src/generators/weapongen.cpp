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

	Game::Weapon* WeaponGenerator::Generate(float _power, float _qualityFactor)
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

		// roll basic type; -2 since lasers are not implemented
		WeaponType type = (WeaponType)m_randomSampler.Uniform(0, (int32_t)NAMES.size()-2);
		m_name = NAMES[(int)type];
		m_description.clear();

		// attributes
		Range damageRange = GetDamageRange(DAMAGE_RANGE[(int)type], _power);
		float damage = m_randomSampler.Uniform(damageRange);
		m_description += "damage: " + std::to_string(damage) + "\n";

		Range cdRange = GetCooldownRange(COOLDOWN_RANGE[(int)type], _power);
		float cooldown = m_randomSampler.Uniform(cdRange);
		m_description += "cooldown: " + std::to_string(cooldown) + "s\n";

		float speed = type == WeaponType::Simple ? Projectile::DEFAULT_SPEED : Rocket::DEFAULT_SPEED;
		float lifeTime = m_randomSampler.Uniform(5.f, 10.f);

		float eCost = damage / (cooldown * _power);
		m_description += "energy(per shot): " + std::to_string(eCost) + "\n";

		m_description += "speed: " + std::to_string(speed);
		Weapon::FireFunction fireFn;
		Weapon::ReloadFunction reloadFn;

		WeaponTrait::GenerationFunction projGenerator;
		switch (type)
		{
		case WeaponType::Simple:
			projGenerator = WeaponTrait::CreateProjectileFn(Bolt(ei::Vec3(0.f), ei::Vec3(0.f, 0.f, speed), damage, lifeTime));
			break;
		case WeaponType::Rocket:
			projGenerator = WeaponTrait::CreateProjectileFn(Rocket(ei::Vec3(0.f), ei::Vec3(0.f, 0.f, speed), damage, lifeTime));
			break;
		default:
			Assert(false, "This type is not implemented.");
		}

	//	if (rarity == Quality::Unique)
		{
			switch (m_randomSampler.Uniform(0, 1))
			{
			case 0: fireFn = WeaponTrait::FireDouble(std::move(projGenerator));
				AddTrait(WEAPON_TRAITS[1]);
				break;
			case 1: reloadFn = WeaponTrait::ReloadBurstFire();
				AddTrait(WEAPON_TRAITS[0]);
				break;
			}
		}
		if (!fireFn) fireFn = WeaponTrait::FireDefault(std::move(projGenerator));

		// accumulated stats
		m_description += "\n-----\ndps: " + std::to_string(1.f / cooldown * damage)
			+ "\nrange: " + std::to_string(speed * lifeTime);

		m_name = QUALITY_COLOR[(int)rarity] + m_name + "</c>";

		return new Game::Weapon(cooldown,
			speed * lifeTime,
			eCost,
			std::move(fireFn),
			std::move(reloadFn));
	}

	Range WeaponGenerator::GetDamageRange(Range _base, float _power)
	{
		float f = 1.f + _power / 10.f;
		return std::make_pair(_base.first * f, _base.second * f);
	}

	const float MAX_POWER = 132.f;
	const float MIN_COOLDOWN = 0.1f;
	const float MAX_COOLDOWN = 1.f;
	Range WeaponGenerator::GetCooldownRange(Range _base, float _power)
	{
		float f = ei::min(1.f, _power / MAX_POWER);
		return std::make_pair(ei::lerp(_base.first, MIN_COOLDOWN, f), ei::lerp(_base.second, MAX_COOLDOWN, f));
	}

	void WeaponGenerator::AddTrait(const TraitDescription& _traitDesrc)
	{
		m_name = _traitDesrc.isPrefix ? _traitDesrc.name + " " + m_name
			: m_name + " " + _traitDesrc.name;
		m_description += "\n" + _traitDesrc.description;
	}
}