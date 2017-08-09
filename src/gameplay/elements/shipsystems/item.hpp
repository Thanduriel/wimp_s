#pragma once

#include <string>

namespace Game {
	class Item
	{
	public:
		Item(const std::string& _name, const std::string& _description)
			: m_name(_name),
			m_description(_description)
		{}

		const std::string& GetName() const { return m_name; }
		const std::string& GetDescription() const { return m_description; }

	protected:
		std::string m_name;
		std::string m_description;
	};
}