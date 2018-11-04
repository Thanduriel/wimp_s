#include "helpers.hpp"
#include "control/camera.hpp"
#include "control/kamikazecontroller.hpp"
#include "control/waspcontroller.hpp"
#include "control/turtlecontroller.hpp"
#include "generators/asteroidfield.hpp"

namespace Game {

	std::string GetColoredKeyName(Control::VirtualKey _vkey)
	{
		return "<c 0 255 0>" + Control::InputManager::GetBoundKey(_vkey) + "</c>";
	}

	Map::Map(SceneGraph& _sceneGraph, const Ship& _playerShip, GameStates::MainHud& _hud)
		: m_sceneGraph(_sceneGraph), 
		m_mainHud(_hud),
		m_playerShip(_playerShip),
		m_weaponGen(0x0), 
		m_shieldGen(0x32410015),
		m_isFinished(false),
		m_randomGen(0x0)
	{
	}

	constexpr float SELLRATIO = 0.5f;
	Ship& Map::CreateShip(const std::string& _type, const ei::Vec3& _position, int _numWeapons, float _power, float _rarityMod, Drop _drop)
	{
		Ship& ship = *new Ship(_type, _position);
		ship.GetCollisionComponent().SetType(ship.GetCollisionComponent().GetType() | CollisionComponent::Type::Hostile);
		FactoryActor::GetThreadLocalInstance().Add(ship);
		auto& inventory = ship.GetInventory();

		for (int i = 0; i < _numWeapons; ++i)
		{
			Weapon* w = m_weaponGen.Generate(_power, _rarityMod);
			FactoryActor::GetThreadLocalInstance().Add(*w);
			ship.SetWeapon(i, w);
			ship.GetInventory().Add(*w);
		}
		// shield item
		Shield* shield = m_shieldGen.Generate(_power, _rarityMod);
		inventory.Add(*shield);
		FactoryActor::GetThreadLocalInstance().Add(*shield);

		if (_drop == Drop::NonBasics || _drop == Drop::Credits)
		{
			float sellRatio = _drop == Drop::NonBasics ? SELLRATIO : 1.f;
			std::vector<Item*> sellItems;
			for (Item* item : inventory)
			{
				if (item->GetQuality() == Item::Quality::Basic && Generators::g_random.Uniform(0.f, 1.f) <= 0.5f)
					sellItems.push_back(item);
			}
			for (Item* item : sellItems)
			{
				inventory.AddCredits(item->GetValue());
				inventory.Remove(*item);
			}
		}

		return ship;
	}

	void Map::SetupPlayer(Ship& _player, const ei::Vec3& _position, const ei::Quaternion& _rotation, float _speed)
	{
		_player.SetPosition(_position);
		_player.SetRotation(_rotation);
		_player.SetSpeed(_speed);
		_player.SetVelocity(_player.GetRotationMatrix() * Vec3(0.f, 0.f, _speed));
		_player.SetHealth(_player.GetMaxHealth());
		Control::g_camera.SetPosition(_player.GetPosition() - _player.GetVelocity());
		Control::g_camera.SetRotation(_rotation);
		Control::g_camera.Attach(_player);
	}

	void Map::FinishMap(Level _nextLevel)
	{
		m_nextLevel = _nextLevel;
		m_isFinished = true;
	}

	using namespace Control;
	const std::array< Map::ShipValue, (size_t)Map::ShipType::COUNT> Map::SHIP_VALUES = 
	{ {	
		{ "DroneMK1", 1, 1, AIControllerType::Kamikaze},
		{ "Dart", 3, 1, AIControllerType::Wasp },
		{ "Fighter", 6, 2, AIControllerType::Wasp },
		{ "BattleShip", 24, 4, AIControllerType::Wasp }
	} };

	void Map::CreateAsaultForce(std::vector<ShipType>&& _shipTypes, const Vec3& _position, const Ship& _target,
		int _strength, float _weaponPower, float _rarityMod)
	{
		// used to organize occupied spaces
		Generators::AsteroidField spawnfield(_position, _strength * 8.f);
		int strengthLeft = _strength;
		Quaternion rot = m_randomGen.Rotation();

		while(true)
		{
			auto it = _shipTypes.begin();
			for (; it != _shipTypes.end(); ++it)
				if (SHIP_VALUES[*it].value > strengthLeft) break;
			int num = (int)std::distance(_shipTypes.begin(), it) - 1; // current element is to expensive
			// even the cheapest is not enough
			if (num < 0) break;
			ShipType type = _shipTypes[m_randomGen.Uniform(0, num)];
			strengthLeft -= SHIP_VALUES[type].value;

			// create ship with 50% chance to drop weapon / credits
			Ship& ship = CreateShip(SHIP_VALUES[type].typeName, _position, SHIP_VALUES[type].numWeapons, _weaponPower,
				_rarityMod, m_randomGen() % 2 ? Drop::All : Drop::Credits);
			ship.SetRotation(rot);

			// find suitable position that is not occupied
			float r = ship.GetCollisionComponent().GetBoundingRadiusSq();
			Vec3 pos = spawnfield.FindPosition(r);
			ship.SetPosition(pos);
			spawnfield.AddConstraint(Generators::SpaceConstraint(ship));
			switch (SHIP_VALUES[type].controllerType)
			{
			case AIControllerType::Kamikaze: CreateController<Control::KamikazeController>(ship, _target.GetHandle(), m_mainHud, SHIP_VALUES[type].typeName);
				break;
			case AIControllerType::Wasp: CreateController<Control::WaspController>(ship, _target.GetHandle(), m_mainHud, SHIP_VALUES[type].typeName);
				break;
			case AIControllerType::Turtle: CreateController<Control::TurtleController>(ship, _target.GetHandle(), m_mainHud, SHIP_VALUES[type].typeName);
				break;
			}
		}
	}
}