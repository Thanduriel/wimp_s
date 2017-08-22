#include "act01.hpp"
#include "gameplay/elements/ship.hpp"
#include "gameplay/events/conditions.hpp"
#include "gameplay/elements/factorycomponent.hpp"
#include "gameplay/elements/crate.hpp"
#include "generators/weapongen.hpp"
#include "gamestates/mainstate.hpp"

namespace Game {
namespace Acts {

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
		// some cheap personnel

		// --- events --------------------------------------- //
		// 04
		auto nextAct = CREATE_ACTION(
			FactoryActor::GetThreadLocalInstance().Make<Ship>("TestShip", ei::Vec3(0.f));
		);

		// 03
		auto beginDestroyShips = CREATE_ACTION(
			auto& ev = CREATE_OBJECTIVE(Conditions::Timer)(nextAct, "destroy the ships near by", 8.f);
			_hud.AddObjective(ev);
		);

		// 02
		auto beginPickUpWeapon = CREATE_ACTION( 
			auto& ev = CREATE_OBJECTIVE(Conditions::OnDestroy)(beginDestroyShips, 
				"pick up and equip the weapons", std::vector<Actor::Handle>({ hndl }), 1);
			_hud.AddObjective(ev);
			_hud.AddIndicator(*crate, "crate");
		);

		// 01
		auto& ev = factory.Make<Event<Conditions::IsClose>>(beginPickUpWeapon, "get closer to the facility", _player, crate->GetPosition(), 60.f);
		_hud.AddObjective(ev);
	//	_hud.AddIndicator(crate, "facility");

	}
}
}