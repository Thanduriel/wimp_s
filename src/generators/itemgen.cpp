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
}