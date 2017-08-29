#include "inventory.hpp"

namespace Game {
	bool Inventory::Add(Item& _item)
	{
		return m_items.insert(&_item).second;
	}

	void Inventory::Remove(Item& _item)
	{
		m_items.erase(&_item);
	}

	void Inventory::Transfer(Inventory& _source)
	{
		m_items.insert(_source.m_items.begin(), _source.m_items.end());
		_source.m_items.erase(_source.m_items.begin(), _source.m_items.end());
		m_credits += _source.m_credits;
		_source.m_credits = 0;
	}
}