#include "act02.hpp"
#include "gameplay/elements/ship.hpp"
#include "gameplay/events/conditions.hpp"
#include "gameplay/elements/factorycomponent.hpp"
#include "gameplay/elements/crate.hpp"
#include "gameplay/elements/sun.hpp"
#include "gamestates/huds/mainhud.hpp"
#include "control/waspcontroller.hpp"
#include "control/turtlecontroller.hpp"

namespace Game {
namespace Acts {

	using namespace Utils;
	using namespace ei;

	const Vec3 PLAYER_SPAWN = Vec3(0.f);
	const Vec3 MEETING_POSITION = Vec3(100.f, 600.f, 3000.f);
	Act02::Act02(SceneGraph& _sceneGraph, Ship& _player, GameStates::MainHud& _hud)
		: Map(_sceneGraph, _player, _hud),
		m_finishDescription("You won! There is nothing left to do.")
	{
		SetupPlayer(_player, PLAYER_SPAWN);
		Actor::ConstHandle playerHandle = _player.GetHandle();

		m_sceneGraph.Add(*new Sun(Vec3(-3457.f, -1500.f, 20000.f), 3300.f));
		Graphic::LightSystem::SetDominantLight(ei::normalize(Vec3(-3457.f, -1500.f, 20000.f) - PLAYER_SPAWN));
		Ship& boss = CreateShip("BattleShip", MEETING_POSITION, 7, 13.f);
		Actor::Handle bossHandle = boss.GetHandle();

		auto Aaction = CREATE_ACTION
		{
			_hud.AddObjective(m_finishDescription);
		};

		auto AturnAround = CREATE_ACTION
		{
			CreateController<Control::TurtleController>(static_cast<Ship&>(**bossHandle), playerHandle, _hud, "BattleShip");

			CREATE_OBJECTIVE4(Conditions::OnDestroy, Aaction, "destroy the battleship",
				std::vector<Actor::ConstHandle>({ bossHandle }), 1);
		};

		auto& ev = *new Event<Conditions::IsClose>(AturnAround, "reach the rendezvous coordinates", _player, MEETING_POSITION, 250.f);
		_hud.AddObjective(ev);
		m_sceneGraph.Add(ev);
		ev.SetPosition(MEETING_POSITION);
		_hud.AddIndicator(ev, "rendezvous coordinates", Color8U(1.f, 1.f, 0.f));

		auto pirateAttack2 = CREATE_ACTION
		{
			const Vec3 playerPos = _player.GetPosition();
			const Vec3 playerLeft = _player.GetRotationMatrix() * Vec3(-1.f, 0.f, 0.f);
			Vec3 spawnPos = playerPos + playerLeft * m_randomGen.Uniform(80.f, 100.f);
			CreateAsaultForce(std::vector<ShipType>{Dart, Fighter}, spawnPos, _player, 12, 11.5f, 1.5f);
		};

		auto& pirateTrigger = *new Event<Conditions::IsClose>(pirateAttack2, "", _player, MEETING_POSITION, 1000.f);
		m_sceneGraph.Add(pirateTrigger);

		auto pirateAttack = CREATE_ACTION
		{
			const Vec3 playerPos = _player.GetPosition();
			const Vec3 playerLeft = _player.GetRotationMatrix() * Vec3(1.f, 0.f, 0.f);
			Vec3 spawnPos = playerPos + playerLeft * m_randomGen.Uniform(80.f, 100.f);
			CreateAsaultForce(std::vector<ShipType>{Dart, Fighter}, spawnPos, _player, 10, 11.5f, 1.5f);
		};
		// pirate attack!
		auto& pirateTimer = *new Event<Conditions::Timer>(pirateAttack, "", 7.f);
		m_sceneGraph.Add(pirateTimer);
	}
}
}