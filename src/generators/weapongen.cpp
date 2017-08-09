#include <ctime>
#include "weapongen.hpp"
#include "gameplay/elements/shipsystems/projectile.hpp"

namespace Generators {

	enum WeaponTraitType {
		Burst,
		Twin,
		ExtR,
		ExtD,
		ExtC,
		HighPower,
		Gatling,
		WTTCOUNT // since this is not in its own name space
	};

	const std::array< TraitDescription, WTTCOUNT> WEAPON_TRAITS =
	{ {
		{"Burst", "cd increases with continues fire", true},
		{"Twin", "fires additional projectile", true },
		{"[EXT_R]", "10% increased rate of fire", false},
		{"[EXT_D]", "12% increased damage", false},
		{"[EXT_C]", "15% reduced power consumption", false},
		{"of High Power", "2x damage, 2x power consumption", false},
		{"Gatling", "fire rate increases with continues fire", false}
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

	WeaponGenerator::WeaponGenerator(uint32_t _seed)
		: m_rng(_seed ? _seed : clock()),
		m_randomSampler(m_rng)
	{
	}

	static std::string MakeNum(float _num)
	{
		std::string s = std::to_string(_num);
		s.erase(s.find_first_of('.') + 2, std::string::npos);
		return /*"<s 012>" +*/ s /*+ "</s>"*/;
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
		int numTraits = QUALITY_NUM_TRAITS[(int)rarity];

		// roll basic type; -2 since lasers are not implemented
		WeaponType type = (WeaponType)m_randomSampler.Uniform(0, (int32_t)NAMES.size()-((int)rarity> 0 ? 2 : 3));
		m_name = NAMES[(int)type];
		if (type == WeaponType::Rocket) numTraits -= 1;
		m_description.clear();

		// attributes
		Range damageRange = GetDamageRange(DAMAGE_RANGE[(int)type], _power);
		float damage = m_randomSampler.Uniform(damageRange);
		m_description += "damage:    " + MakeNum(damage) + "\n";

		Range cdRange = GetCooldownRange(COOLDOWN_RANGE[(int)type], _power);
		float cooldown = m_randomSampler.Uniform(cdRange);
		m_description += "cooldown:  " + MakeNum(cooldown) + "[s]\n";

		float speed = type == WeaponType::Simple ? Projectile::DEFAULT_SPEED : Rocket::DEFAULT_SPEED;
		float lifeTime = m_randomSampler.Uniform(5.f, 10.f);

		float eCost = damage / (cooldown * _power);
		m_description += "power use: " + MakeNum(eCost) + "[J]\n";

		m_description += "speed:     " + MakeNum(speed) + "[m/s]\n";
		Weapon::FireFunction fireFn;
		Weapon::ReloadFunction reloadFn;

		m_description += "-----";
		// traits that have to be added after projectile generation has finished
		std::vector< WeaponTraitType > lateTraits;
		std::vector< int > hasTrait(WeaponTraitType::WTTCOUNT);
		memset(&hasTrait.begin(), 0, sizeof(int) * hasTrait.size());

		while(numTraits)
		{
			WeaponTraitType trait = (WeaponTraitType)m_randomSampler.Uniform(0, WTTCOUNT-1);
			if (hasTrait[trait]) continue; // every trait can only occur once
			--numTraits;

			hasTrait[trait] = 1;
			AddTrait(WEAPON_TRAITS[trait]);
			switch (trait)
			{
			case WeaponTraitType::Burst: lateTraits.push_back(Burst);
				hasTrait[Gatling] = 1; // prevent these two traits to appear on the same weapon
				break;
			case WeaponTraitType::Twin: lateTraits.push_back(Twin);
				break;
			case WeaponTraitType::ExtR: cooldown *= 0.9f;
				break;
			case WeaponTraitType::ExtD: damage *= 1.12f;
				break;
			case WeaponTraitType::ExtC: eCost *= 0.85f;
				break;
			case WeaponTraitType::HighPower: damage *= 2.f; eCost *= 2.f;
				break;
			case WeaponTraitType::Gatling: lateTraits.push_back(Gatling);
				hasTrait[Burst] = 1;
				break;
			default:
				Assert(false, "Trait not implemented.");
			}
		}

		// construct projectile generator
		WeaponTrait::GenerationFunction projGenerator;
		switch (type)
		{
		case WeaponType::Simple:
			projGenerator = WeaponTrait::CreateProjectileFn(Bolt(ei::Vec3(0.f), ei::Vec3(0.f, 0.f, speed), damage, lifeTime,
				Utils::Color8U(m_randomSampler.Uniform(), m_randomSampler.Uniform(), m_randomSampler.Uniform())));
			break;
		case WeaponType::Rocket:
			projGenerator = WeaponTrait::CreateProjectileFn(Rocket(ei::Vec3(0.f), ei::Vec3(0.f, 0.f, speed), damage, lifeTime));
			break;
		default:
			Assert(false, "This type is not implemented.");
		}

		// add traits that require the generator
		for (auto trait : lateTraits)
		{
			switch (trait) 
			{
			case Burst: reloadFn = WeaponTrait::ReloadBurstFire();
				break;
			case Twin: fireFn = WeaponTrait::FireDouble(std::move(projGenerator));
				break;
			case Gatling: reloadFn = WeaponTrait::ReloadBuildUp();
				break;
			default:
				Assert(false, "This trait should not be added late");
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
			std::move(reloadFn),
			std::move(m_name),
			std::move(m_description));
	}

	Game::Weapon* WeaponGenerator::Generate(uint32_t _seed, float _power, float _qualityFactor)
	{
		m_rng.seed(_seed);

		return Generate(_power, _qualityFactor);
	}

	// *************************************************************** //
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