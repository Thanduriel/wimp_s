#include "itemgen.hpp"

namespace Generators {


	ItemGenerator::ItemGenerator(uint32_t _seed)
		: m_rng(_seed ? _seed : clock()),
		m_randomSampler(m_rng)
	{
	}

	void ItemGenerator::AddTrait(const TraitDescription& _traitDesrc)
	{
		if (_traitDesrc.isPrefix) m_prefixes.push_back(&_traitDesrc.name);
		else m_suffixes.push_back(&_traitDesrc.name);

		if(_traitDesrc.description != "") m_description += "\n" + _traitDesrc.description;
	}

	std::string ItemGenerator::GetName(const std::string& _baseName)
	{
		std::string name = (m_prefixes.size() ? *m_prefixes[m_randomSampler.Uniform(0, m_prefixes.size()-1)] + ' ' : "") 
			+ _baseName
			+ (m_suffixes.size() ? ' ' + *m_suffixes[m_randomSampler.Uniform(0, m_suffixes.size()-1)] : "");

		return std::move(name);
	}

	float ItemGenerator::GenerateValue(float _min, float _max, float _stepSize)
	{
		const int steps = static_cast<int>(round((_max - _min) / _stepSize));

		return m_randomSampler.Uniform(0, steps) * _stepSize + _min;
	}

	using namespace Game;
	Item::Quality ItemGenerator::RollRarity(float _rarityFactor)
	{
		auto qVec = QUALITY_RARITY * _rarityFactor;
		qVec.x = 1.f; // everything is at least of normal quality
		Item::Quality rarity = Item::Quality::Unique;
		while (float n = m_randomSampler.Uniform() > qVec[(int)rarity]) rarity = Item::Quality((int)rarity - 1);

		return rarity;
	}
}