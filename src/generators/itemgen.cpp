#include "itemgen.hpp"

namespace Generators {

	using namespace Game;

	const std::array< TraitDescription, 5> RARITY_NAME_TRAITS =
	{ {
		{"Basic", "", true},
		{"Advanced", "", true },
		{"Premium", "", true },
		{"Prototype", "", false },
		{"Unique", "", true }
	} };


	ItemGenerator::ItemGenerator(size_t _numTraits, uint32_t _seed)
		: m_rng(_seed ? _seed : clock()),
		m_randomSampler(m_rng),
		m_hasTrait(_numTraits)
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
		std::string name = (m_prefixes.size() ? *m_prefixes[m_randomSampler.Uniform(0, static_cast<int>(m_prefixes.size())-1)] + ' ' : "") 
			+ _baseName
			+ (m_suffixes.size() ? ' ' + *m_suffixes[m_randomSampler.Uniform(0, static_cast<int>(m_suffixes.size())-1)] : "");

		return Item::QUALITY_COLOR_STR[(int)m_rarity] + name + "</c>";
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

		m_rarity = rarity;

		return rarity;
	}

	void ItemGenerator::AddRarityNames(Item::Quality _quality)
	{
		if (_quality == Item::Quality::Basic) return;
		
		AddTrait(RARITY_NAME_TRAITS[static_cast<size_t>(_quality)]);
		if (_quality == Item::Quality::Unique)
			AddTrait(RARITY_NAME_TRAITS[3]);
	}

	void ItemGenerator::Reset()
	{
		m_name.clear();
		m_baseStats.clear();
		m_description.clear();

		memset(&m_hasTrait.front(), 0, sizeof(int) * m_hasTrait.size());
	}
}