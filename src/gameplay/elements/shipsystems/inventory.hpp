#pragma once

#include <unordered_set>

namespace Game {
	class Item;

	class Inventory
	{
	public:
		// Add a single Item to the inventory.
		// @return Has the element been inserted?
		bool Add(Item& _item);
		void Remove(Item& _item);
		// Takes all items from the given inventory and puts them into this.
		void Transfer(Inventory& _source);
		size_t GetNumElements() const { return m_items.size(); }


		auto begin() { return m_items.begin(); }
		auto end() { return m_items.end(); }

		auto begin() const { return m_items.begin(); }
		auto end() const { return m_items.end(); }
	private:
		std::unordered_set<Item*> m_items;
	};
}