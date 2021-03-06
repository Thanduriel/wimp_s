#pragma once
#include <array>
#include <string>
#include "ei/vector.hpp"

#include "gameplay/elements/shipsystems/item.hpp"
#include "random.hpp"
#include "utils/stringutils.hpp"

namespace Generators {
	
	// probability for an item to be of the given quality
	// The different classes are rolled beginning with Unique.
	// Thus the actual probability differs slightly.
	const ei::Vec<float, (size_t)Game::Item::Quality::COUNT> QUALITY_RARITY =
	{
		1.f,
		0.2f,
		0.08f, //0.04
		0.04f//0.01
	};

	// Some score that is used to decide the general strength of the stats
	const std::array<std::pair<float, float>, (size_t)Game::Item::Quality::COUNT> QUALITY_BASESTAT =
	{ {
		{11.f, 20.f},
		{13.f, 22.f},
		{16.f, 24.f},
		{18.f, 25.f}
	} };

	const std::array<int, (size_t)Game::Item::Quality::COUNT> QUALITY_NUM_TRAITS =
	{
		0,
		1,
		2,
		3
	};

	struct TraitDescription
	{
		std::string name;
		std::string description;
		bool isPrefix;
	};

	class Range : public std::pair<float, float>
	{
	public:
		using pair<float, float>::pair;

		float GetRelativePosition(float _abs) const
		{
			return (_abs - first) / (second - first);
		}
	};

	class ItemGenerator
	{
	public:	
		ItemGenerator(size_t _numTraits, uint32_t _seed = 0x0);
		
		std::string GetName(const std::string& _baseName);

	protected:
		void AddTrait(const TraitDescription& _trait);
		template<typename... Args>
		void AddTrait(const TraitDescription& _traitDesrc, Args&&... _args)
		{
			m_isTraitInfoSet = true;

			if (_traitDesrc.isPrefix) m_prefixes.push_back(&_traitDesrc.name);
			else m_suffixes.push_back(&_traitDesrc.name);

			if (_traitDesrc.description != "")
			{
				m_description += "\n" + Utils::FormatString(_traitDesrc.description.c_str(), 
					std::forward<Args>(_args)...);
			}
		}

		// groups of traits that can not occur on the same item
		// does not handle individual traits
		template<typename T, size_t S>
		void AddTraitGroup(const std::array<T, S>& _traits)
		{
			for (const T& el : _traits)
				m_hasTrait[static_cast<size_t>(el)] = 1;
		}

		/* GenerateValue()
		 * Generate a random number v in [_min, _max]
		 *  with v = _min + x * _stepSize.
		 */
		float GenerateValue(float _min, float _max, float _stepSize);

		Game::Item::Quality RollRarity(float _rarityFactor);
		void AddRarityNames(Game::Item::Quality _quality);
		void Reset();

		std::string m_name;
		std::string m_baseStats;
		std::string m_description;
		std::vector<int> m_hasTrait;
		Game::Item::Quality m_rarity;

		DefaultRandomGen m_rng;
		RandomSampler<> m_randomSampler;

		bool m_isTraitInfoSet = false; // flag to not add a trait description twice
	private:
		std::vector<const std::string*> m_prefixes;
		std::vector<const std::string*> m_suffixes;
	};
}