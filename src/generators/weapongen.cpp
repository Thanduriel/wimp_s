#include <ctime>
#include "weapongen.hpp"
#include "gameplay/elements/shipsystems/projectile.hpp"
#include "utils/stringutils.hpp"

namespace Generators {

	enum WeaponTraitType {
		Burst,
		Twin,
		ExtR,
		ExtD,
		ExtC,
		ExtT,
		Short,
		HighPower,
		Gatling,
		Iterative,
		LowPower,
		Ionized,
		Corroding,
	//	Infinite,
		WTTCOUNT // since this is not in its own name space
	};

	const std::array< TraitDescription, WTTCOUNT> WEAPON_TRAITS =
	{ {
		{"Burst", "cd increases with continues fire", true},
		{"Twin", "fires additional projectile", true },
		{"[EXT R]", "10% increased rate of fire", false},
		{"[EXT D]", "12% increased damage", false},
		{"[EXT C]", "15% reduced power consumption", false},
		{"[EXT L]", "20% increased life time", false },
		{"Short", "40% reduced life time, 50% increased damage", true },
		{"of High Power", "2x damage, 2x power consumption", false},
		{"Gatling", "fire rate increases with continues fire", false},
		{ "Iterative", "every 3rth shot deals 2xdamage", true },
		{ "of Low Power", "95% reduced consumption, 80% less damage", false },
		{"Ionizing", "deals bonus damage against shields", true},
		{"Corroding", "deals bonus damage against hulls", true}
	//	{"Infinite", "can fire even without sufficient energy", true}
	} };

	const std::array< TraitDescription, 4> EXTRA_NAME_TRAITS = 
	{ {
		{"Rapid Fire", "", true},
		{"Fast Fire", "", true},
		{"Advanced", "", true },
		{"Premium", "", true },
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
		{0.5f, 1.f},
		{2.f, 6.f}
	} };

	// base values for damage
	const std::array< Range, (size_t)WeaponType::Count> DAMAGE_RANGE =
	{ {
		{2.f, 5.f},
		{8.f, 20.f}
	} };

	const std::array< Range, (size_t)WeaponType::Count> LIFETIME_RANGE =
	{ {
		{ 0.5f, 1.7f },
		{ 8.f, 24.f }
	} };

	using namespace Game;
	using namespace Utils;

	Game::Weapon* WeaponGenerator::Generate(float _power, float _qualityFactor)
	{
	/*	static bool killer = false;
		if (!killer)
		{
			killer = true;
			m_name = "<c 154 153 203>performance killer</c>";
			m_description = "damage: " + std::to_string(std::numeric_limits<float>::infinity())
				+ "\n" + "cooldown: 0.0";
			return new Weapon(0.f);
		}*/
		std::vector< int > hasTrait(WeaponTraitType::WTTCOUNT);
		memset(&hasTrait.begin(), 0, sizeof(int) * hasTrait.size());

		// decide on quality
		auto qVec = QUALITY_RARITY * _qualityFactor;
		qVec.x = 1.f; // everything is at least of normal quality
		Item::Quality rarity = Item::Quality::Unique;
		while (float n = m_randomSampler.Uniform() > qVec[(int)rarity]) rarity = Item::Quality((int)rarity - 1);
		int numTraits = QUALITY_NUM_TRAITS[(int)rarity];
		
		// roll basic type; -2 since lasers are not implemented
		WeaponType type = (WeaponType)m_randomSampler.Uniform(0, (int32_t)NAMES.size()-((int)rarity> 0 ? 2 : 3));
		// decrease chance for rocket launchers; todo: refractory this
		if (type == WeaponType::Rocket && !m_randomSampler.Uniform(0, 2)) type = WeaponType::Simple;
		m_name = NAMES[(int)type];
		m_description.clear();
		m_baseStats.clear();

		if (rarity == Item::Quality::Advanced && type != WeaponType::Rocket) AddTrait(EXTRA_NAME_TRAITS[2]);
		else if (rarity == Item::Quality::Premium) AddTrait(EXTRA_NAME_TRAITS[3]);

		// attributes
		Range damageRange = GetDamageRange(DAMAGE_RANGE[(int)type], _power);
		float damage = m_randomSampler.Uniform(damageRange);

		Range cdRange = GetCooldownRange(COOLDOWN_RANGE[(int)type], _power);
		float cooldown = m_randomSampler.Uniform(cdRange);
		// extra name traits
		float relativeCD = (cooldown - cdRange.first) / (cdRange.second - cdRange.first);
		if (relativeCD < 0.1f) AddTrait(EXTRA_NAME_TRAITS[0]);
		else if (relativeCD < 0.25f) AddTrait(EXTRA_NAME_TRAITS[1]);

		float speed = type == WeaponType::Simple ? Projectile::DEFAULT_SPEED : Rocket::DEFAULT_SPEED;
		float lifeTime = m_randomSampler.Uniform(LIFETIME_RANGE[(int)type]);

		float eCost = damage / (cooldown * _power);
		if (type == WeaponType::Rocket)
		{
			// rocket properties count as trait
			if (type == WeaponType::Rocket)
				numTraits -= 1;
			eCost *= 4.f;
		}

		Weapon::FireFunction fireFn;
		Weapon::ReloadFunction reloadFn;
		DamageType damageType = DamageType::Normal;

		// traits that have to be added after projectile generation has finished
		std::vector< WeaponTraitType > lateTraits;

		while(numTraits)
		{
			WeaponTraitType trait = (WeaponTraitType)m_randomSampler.Uniform(0, WTTCOUNT-1);
			if (hasTrait[trait]) continue; // every trait can only occur once
			--numTraits;

			// every trait can appear only once
			hasTrait[trait] = 1;
			AddTrait(WEAPON_TRAITS[trait]);
			switch (trait)
			{
			// reload functions
			case WeaponTraitType::Burst: lateTraits.push_back(Burst);
				hasTrait[Gatling] = 1; // prevent these two traits to appear on the same weapon
				break;
			case WeaponTraitType::Gatling: lateTraits.push_back(Gatling);
				hasTrait[Burst] = 1;
				break;
			case WeaponTraitType::ExtR: cooldown *= 0.9f;
				break;
			case WeaponTraitType::ExtD: damage *= 1.12f;
				break;
			case WeaponTraitType::ExtC: eCost *= 0.85f;
				break;
			case WeaponTraitType::ExtT: lifeTime *= 1.2f;
				break;
			case WeaponTraitType::Short: lifeTime *= 0.6f; damage *= 1.4f;
				break;
			case WeaponTraitType::HighPower: damage *= 2.f; eCost *= 2.f;
				hasTrait[LowPower] = 1;
				break;
			case WeaponTraitType::LowPower: eCost *= 0.05f;
				damage *= 0.1f;
				hasTrait[HighPower] = 1;
				break;
			// fire functions
			case WeaponTraitType::Twin: lateTraits.push_back(Twin);
				hasTrait[Iterative] = 1;
				break;
			case WeaponTraitType::Iterative: lateTraits.push_back(Iterative);
				hasTrait[Twin] = 1;
				break;
			case WeaponTraitType::Ionized:
				hasTrait[Corroding] = 1;
				damageType = DamageType::Ion;
				break;
			case WeaponTraitType::Corroding:
				hasTrait[Ionized] = 1;
				damageType = DamageType::Physical;
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
			projGenerator = WeaponTrait::CreateProjectileFn(Bolt(ei::Vec3(0.f), ei::Vec3(0.f, 0.f, speed), damage, lifeTime, damageType,
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
			case Iterative: fireFn = WeaponTrait::FireIterative(std::move(projGenerator));
				break;
			default:
				Assert(false, "This trait should not be added late");
			}
		}
		if (!fireFn) fireFn = WeaponTrait::FireDefault(std::move(projGenerator));

		m_baseStats += "damage:   " + ToConstDigit(damage,1,4) + "\n";
		m_baseStats += "cooldown:  " + ToFixPoint(cooldown,1) + " [s]\n";
		m_baseStats += "power use: " + ToFixPoint(eCost,1) + " [J]\n";
		m_baseStats += "lifetime: " + ToConstDigit(lifeTime,1, 4) + " [s]\n";

		m_description = m_baseStats + "-----" + m_description;
		// accumulated stats
		m_description += "\n-----\ndps:      " + ToConstDigit(1.f / cooldown * damage,1,4)
			+ "\nrange:   " + ToConstDigit(speed * lifeTime,1,5);

		m_name = Item::QUALITY_COLOR_STR[(int)rarity] + GetName(m_name) + "</c>";

		return new Game::Weapon(cooldown,
			speed * lifeTime,
			eCost,
			std::move(fireFn),
			std::move(reloadFn),
			rarity,
			type == WeaponType::Simple ? Item::Icon::DefaultWeapon : Item::Icon::Missile,
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
}