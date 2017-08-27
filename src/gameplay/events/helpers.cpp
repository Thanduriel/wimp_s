#include "helpers.hpp"

namespace Game {

	Map::Map(SceneGraph& _sceneGraph) : m_sceneGraph(_sceneGraph), m_weaponGen(0x0)
	{
	}

	Ship& Map::CreateShip(const std::string& _type, const ei::Vec3& _position, int _numWeapons, float _power, float _rarityMod)
	{
		Ship& ship = *new Ship(_type, _position);
		m_sceneGraph.Add(ship);

		for (int i = 0; i < _numWeapons; ++i)
		{
			Weapon* w = m_weaponGen.Generate(_power, _rarityMod);
			m_sceneGraph.Add(*w);
			ship.SetWeapon(i, w);
		}

		return ship;
	}
}