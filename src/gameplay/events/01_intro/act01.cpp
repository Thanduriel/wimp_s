#include "act01.hpp"
#include "gameplay/elements/ship.hpp"
#include "gameplay/events/conditions.hpp"
#include "gameplay/elements/factorycomponent.hpp"
#include "gameplay/elements/crate.hpp"
#include "gameplay/elements/sun.hpp"
#include "gameplay/elements/blackhole.hpp"
#include "generators/weapongen.hpp"
#include "generators/asteroidfield.hpp"
#include "gamestates/mainstate.hpp"
#include "control/waspcontroller.hpp"

namespace Game {
namespace Acts {

	using namespace Utils;
	using namespace ei;

	const Vec3 BASE_POSITON = Vec3(-240.f, 50.f, 820.f);

	Act01::Act01(SceneGraph& _sceneGraph, const Ship& _player, GameStates::MainHud& _hud)
		: Map(_sceneGraph),
		m_asteroids(BASE_POSITON - Vec3(12.f, -6.f, 110.f), 200.f)
	{
		Actor::ConstHandle playerHndl = _player.GetHandle();

		// the research base
		Ship& researchBase = *new Ship("ResearchBase", BASE_POSITON);
		m_sceneGraph.Add(researchBase);

		// some simple weapons
		Inventory inventory;
		Generators::WeaponGenerator weaponGen(0x42);
		Weapon* w = weaponGen.Generate(10.f, 0.f);
		inventory.Add(*w);
		m_sceneGraph.Add(*w);
		w = weaponGen.Generate(10.f, 0.f);
		inventory.Add(*w);
		m_sceneGraph.Add(*w);
		Crate* crate = new Crate(BASE_POSITON - ei::Vec3(0.f, 7.f, 150.f), inventory, 5.f);
		m_sceneGraph.Add(*crate);
		Actor::Handle hndl = crate->GetHandle();

		// background
		using namespace Generators;
		m_asteroids.Generate(120, { SpaceConstraint(crate->GetPosition(), 10.f), SpaceConstraint(_player), SpaceConstraint(researchBase) });
		m_sceneGraph.Add(*new Sun(Vec3(777.f, 1500.f, 10000.f), 2000.f));

		// --- events --------------------------------------- //

		auto Aexplosion = CREATE_ACTION
		{
			BlackHole& blackHole = FactoryActor::GetThreadLocalInstance().Make<BlackHole>(BASE_POSITON, 90.f, 60.f, 16.f);
			blackHole.Activate();
		};

		// 04
		auto AapproachFacility = CREATE_ACTION
		{
			CREATE_OBJECTIVE5(Conditions::IsClose, Aexplosion, "approach the facility",
				_player, BASE_POSITON, 100.f);
		};

		// 03
		auto AbeginDestroyShips = CREATE_ACTION
		{
			Ship& ship01 = CreateShip("Dart", m_asteroids.FindPosition(10.f), 1, 8.f, 0.f);
			CreateController<Control::WaspController>(ship01, playerHndl, _hud);
			m_asteroids.AddConstraint(ship01);
			Ship& ship02 = CreateShip("Dart", m_asteroids.FindPosition(10.f), 1, 8.f, 0.f);
			CreateController<Control::WaspController>(ship02, playerHndl, _hud);

			CREATE_OBJECTIVE4(Conditions::OnDestroy, AapproachFacility, "destroy the ships near by",
				std::vector<Actor::Handle>({ ship01.GetHandle(), ship02.GetHandle() }), 2);
		};

		auto AtimeToEqiup = CREATE_ACTION
		{
			CREATE_OBJECTIVE3(Conditions::Timer, AbeginDestroyShips, "equip the weapons and wait", 10.f);
		};

		// 02
		auto AbeginPickUpWeapon = CREATE_ACTION
		{
			CREATE_OBJECTIVE4(Conditions::OnDestroy, AtimeToEqiup,
				"pick up the testing equipment", std::vector<Actor::Handle>({ hndl }), 1);
			_hud.AddIndicator(*crate, "crate", Color8U(1.f,1.f,0.f));
		};

		// 01
		auto& ev = *new Event<Conditions::IsClose>(AbeginPickUpWeapon, "get closer to the facility", _player, researchBase.GetPosition(), 250.f);
		_hud.AddObjective(ev);
		m_sceneGraph.Add(ev);
		_hud.AddIndicator(researchBase, "research facility");

	}
}
}