#include "act03.hpp"
#include "gameplay/elements/ship.hpp"
#include "gameplay/events/conditions.hpp"
#include "gameplay/elements/factorycomponent.hpp"
#include "gameplay/elements/crate.hpp"
#include "gameplay/elements/sun.hpp"
#include "gamestates/huds/mainhud.hpp"
#include "control/waspcontroller.hpp"
#include "control/turtlecontroller.hpp"
#include "gameplay/elements/shipsystems/specialmove.hpp"
#include "gameplay/elements/blackhole.hpp"

namespace Game {
namespace Acts {

	using namespace Utils;
	using namespace ei;
	using namespace Generators;

	const Vec3 PLAYER_SPAWN = Vec3(0.f);
	const Vec3 ASTEROID_FIELD_POSITION = Vec3(2000.f, 1000.f, 300.f);
	const Vec3 PLANET_POSITION = Vec3(6234.f, 31234.f, 1561.f);

	Act03::Act03(SceneGraph& _sceneGraph, Ship& _player, GameStates::MainHud& _hud)
		: Map(_sceneGraph, _player, _hud),
		m_finishDescription("You won! There is nothing left to do."),
		m_asteroids(ASTEROID_FIELD_POSITION, 600.f, 0x7c523e)
	{
		SetupPlayer(_player, PLAYER_SPAWN);
		Actor::ConstHandle playerHandle = _player.GetHandle();

		m_sceneGraph.Add(*new Sun(Vec3(3457.f, 1500.f, 20000.f), 3300.f));
		Graphic::LightSystem::SetDominantLight(ei::normalize(Vec3(3457.f, 1500.f, 20000.f) - PLAYER_SPAWN));
		m_sceneGraph.Add(*new BlackHoleVis(Vec3(-3457.f, -1500.f, 20000.f), 1000.f));

		Ship& boss = CreateShip("BattleShip", ASTEROID_FIELD_POSITION, 7, 16.f);
		Actor::Handle bossHandle = boss.GetHandle();
		CreateController<Control::TurtleController>(static_cast<Ship&>(**bossHandle), playerHandle, _hud, "BattleShip");
		Ship& wingman = CreateShip("Corvette", m_asteroids.FindPosition(40.f*40.f), 2, 17.f);
		CreateAsaultForce(std::vector<ShipType>{Dart, Fighter, Drone}, ASTEROID_FIELD_POSITION + Vec3(100.f, 100.f, 100.f), _player, 8, 15.0f, 1.5f);
		CreateController<Control::WaspController>(wingman, playerHandle, _hud, "Corvette");


		Inventory inventory;
		Generators::WeaponGenerator weaponGen(g_random());
		Generators::ShieldGenerator shieldGen(g_random());
		for (int i = 0; i < 3; ++i)
		{
			Weapon* w = weaponGen.Generate(16.f, 4.f);
			inventory.Add(*w);
			m_sceneGraph.Add(*w);
			Shield* shield = shieldGen.Generate(16.f, 4.f);
			inventory.Add(*shield);
			m_sceneGraph.Add(*shield);
		}
		Crate* crate = new Crate(PLAYER_SPAWN + ei::Vec3(0.f, 0.f, 150.f), inventory, 5.f);
		m_sceneGraph.Add(*crate);
		Actor::Handle hndl = crate->GetHandle();

		/*	Model& largeAsteroid1 = *new Model("asteroid03", ASTEROID_FIELD_POSITION, ei::qidentity(), 10000.f);
			m_sceneGraph.Add(largeAsteroid1);
			Model& largeAsteroid2 = *new Model("asteroid03", ASTEROID_FIELD_POSITION - Vec3(100.f, 50.f, 13.f), g_random.Rotation(), 10000.f);
			m_sceneGraph.Add(largeAsteroid2);
			Model& largeAsteroid3 = *new Model("asteroid03", ASTEROID_FIELD_POSITION + Vec3(10.f, 150.f, 20.f), g_random.Rotation(), 10000.f);
			m_sceneGraph.Add(largeAsteroid3);*/
		m_asteroids.Generate(250, { SpaceConstraint(boss) });

		// background
		Model& planet = *new Model("planet", PLANET_POSITION, ei::qidentity(), 10000.f, "planet2");
		planet.SetAngularVelocity(Vec3(0.01f, 0.02f, 0.0f));
		m_sceneGraph.Add(planet);

		_hud.SetMessageAnimation(true);

		auto Awin = CREATE_ACTION
		{
			_hud.AddObjective(m_finishDescription);
		};

		CREATE_OBJECTIVE4(Conditions::OnDestroy, Awin, "destroy the director's ship and their escort.",
			std::vector<Actor::ConstHandle>({ boss.GetHandle(), wingman.GetHandle() }), 2);
	}
}
}