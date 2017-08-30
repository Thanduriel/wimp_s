#include "helpers.hpp"
#include "control/camera.hpp"

namespace Game {

	Map::Map(SceneGraph& _sceneGraph) : m_sceneGraph(_sceneGraph), m_weaponGen(0x0), m_isFinished(false)
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

	void Map::SetupPlayer(Ship& _player, const ei::Vec3& _position, const ei::Quaternion& _rotation, float _speed)
	{
		_player.SetPosition(_position);
		_player.SetRotation(_rotation);
		_player.SetSpeed(_speed);
		_player.SetVelocity(_player.GetRotationMatrix() * Vec3(0.f, 0.f, _speed));
		Control::g_camera.SetPosition(_player.GetPosition() - _player.GetVelocity());
		Control::g_camera.SetRotation(_rotation);
		Control::g_camera.Attach(_player);
	}

	void Map::FinishMap(Level _nextLevel)
	{
		m_nextLevel = _nextLevel;
		m_isFinished = true;
	}
}