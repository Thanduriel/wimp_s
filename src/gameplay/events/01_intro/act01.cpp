#include "act01.hpp"
#include "gameplay/elements/ship.hpp"
#include "gameplay/events/conditions.hpp"
#include "gameplay/elements/factorycomponent.hpp"
#include "gameplay/elements/crate.hpp"
#include "gameplay/elements/sun.hpp"
#include "generators/weapongen.hpp"
#include "generators/asteroidfield.hpp"
#include "gamestates/mainstate.hpp"

namespace Game {
namespace Acts {

	using namespace Utils;
	using namespace ei;

	Act01::Act01(const Ship& _player, GameStates::MainHud& _hud)
	{
		FactoryActor& factory = FactoryActor::GetThreadLocalInstance();
		// some simple weapons
		Inventory inventory;
		Generators::WeaponGenerator weaponGen(0x42);
		Weapon* w = weaponGen.Generate(10.f, 0.f);
		inventory.Add(*w);
		factory.Add(*w);
		w = weaponGen.Generate(10.f, 0.f);
		inventory.Add(*w);
		factory.Add(*w);
		Crate* crate = &factory.Make<Crate>(ei::Vec3(0.f, 0.f, 120.f), inventory, 10.f);
		Actor::Handle hndl = crate->GetHandle();

		// background
		using namespace Generators;
		Generators::AsteroidField asteroids(Vec3(0.f), 200.f, 120, { SpaceConstraint(crate->GetPosition(), 5.f), SpaceConstraint(_player) });
		factory.Make<Sun>(Vec3(777.f, 1500.f, 10000.f), 2000.f);

		// some cheap personnel

		// --- events --------------------------------------- //
		// 04
		auto nextAct = CREATE_ACTION(
			FactoryActor::GetThreadLocalInstance().Make<Ship>("TestShip", ei::Vec3(0.f));
		);

		// 03
		auto beginDestroyShips = CREATE_ACTION(
			CREATE_OBJECTIVE3(Conditions::Timer, nextAct, "destroy the ships near by", 8.f);
		);

		// 02
		auto beginPickUpWeapon = CREATE_ACTION( 
			CREATE_OBJECTIVE4(Conditions::OnDestroy, beginDestroyShips, 
				"pick up and equip the weapons", std::vector<Actor::Handle>({ hndl }), 1);
			_hud.AddIndicator(*crate, "crate", Color8U(1.f,1.f,0.f));
		);

		// 01
		auto& ev = factory.Make<Event<Conditions::IsClose>>(beginPickUpWeapon, "get closer to the facility", _player, crate->GetPosition(), 60.f);
		_hud.AddObjective(ev);
	//	_hud.AddIndicator(crate, "facility");

	}
}
}