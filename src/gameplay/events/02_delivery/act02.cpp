#include "act02.hpp"
#include "gameplay/elements/ship.hpp"
#include "gameplay/events/conditions.hpp"
#include "gameplay/elements/factorycomponent.hpp"
#include "gameplay/elements/crate.hpp"
#include "gameplay/elements/sun.hpp"
#include "gamestates/huds/mainhud.hpp"
#include "control/waspcontroller.hpp"
#include "control/turtlecontroller.hpp"
#include "gameplay/elements/shipsystems/specialmove.hpp"

namespace Game {
namespace Acts {

	using namespace Utils;
	using namespace ei;
	using namespace Generators;

	const Vec3 PLAYER_SPAWN = Vec3(0.f);
	const Vec3 MEETING_POSITION = Vec3(100.f, 600.f, 3000.f);
	const Vec3 PLANET_POSITION = Vec3(1000.f, 3138.f, 15000.f);
	const Vec3 ASTEROID_FIELD_POSITION = Vec3(1800.f, 2400.f, 4000.f);
	const Vec3 ESCAPE_POSITION = ASTEROID_FIELD_POSITION + Vec3(10.f, 370.f, 140.f);


	Act02::Act02(SceneGraph& _sceneGraph, Ship& _player, GameStates::MainHud& _hud)
		: Map(_sceneGraph, _player, _hud),
		m_finishDescription("You won! There is nothing left to do."),
		m_asteroids(ASTEROID_FIELD_POSITION, 400.f, 0x7c23e)
	{
		SetupPlayer(_player, PLAYER_SPAWN);
		Actor::ConstHandle playerHandle = _player.GetHandle();

		m_sceneGraph.Add(*new Sun(Vec3(-3457.f, -1500.f, 20000.f), 3300.f));
		Graphic::LightSystem::SetDominantLight(ei::normalize(Vec3(-3457.f, -1500.f, 20000.f) - PLAYER_SPAWN));
		Ship& boss = CreateShip("BattleShip", MEETING_POSITION, 7, 16.f);
		Actor::Handle bossHandle = boss.GetHandle();

	/*	Model& largeAsteroid1 = *new Model("asteroid03", ASTEROID_FIELD_POSITION, ei::qidentity(), 10000.f);
		m_sceneGraph.Add(largeAsteroid1);
		Model& largeAsteroid2 = *new Model("asteroid03", ASTEROID_FIELD_POSITION - Vec3(100.f, 50.f, 13.f), g_random.Rotation(), 10000.f);
		m_sceneGraph.Add(largeAsteroid2);
		Model& largeAsteroid3 = *new Model("asteroid03", ASTEROID_FIELD_POSITION + Vec3(10.f, 150.f, 20.f), g_random.Rotation(), 10000.f);
		m_sceneGraph.Add(largeAsteroid3);*/
		m_asteroids.Generate(1400, {  });

		// background
		Model& planet = *new Model("planet", PLANET_POSITION, ei::qidentity(), 10000.f, "planet1");
		planet.SetAngularVelocity(Vec3(0.01f, 0.02f, 0.0f));
		m_sceneGraph.Add(planet);

		_hud.SetMessageAnimation(true);
		_player.SetSpecialMove(*new Game::BlackHoleGenerator(30.f));
		// events in reverse order of execution
		auto AfinishAct = CREATE_ACTION
		{
			FinishMap(Level::Act03);
		};

		auto AleaveSystem = CREATE_ACTION
		{
			CREATE_OBJECTIVE5(Conditions::IsClose, AfinishAct, "leave the system",
				_player, ESCAPE_POSITION, 50.f);
		};

		auto Anothing = CREATE_ACTION
		{};

		auto AlastAttack = CREATE_ACTION
		{
			const Vec3 playerPos = _player.GetPosition();
			const Vec3 playerLeft = _player.GetRotationMatrix() * Vec3(-1.f, 0.f, 0.f);
			Vec3 spawnPos = playerPos + playerLeft * m_randomGen.Uniform(80.f, 100.f);
			CreateAsaultForce(std::vector<ShipType>{Dart, Fighter, Drone}, spawnPos, _player, 14, 14.0f, 1.5f);

			Ship& ship = CreateShip("Corvette", playerPos - playerLeft * 155.f, 2, 16.f, 2.f, Drop::All);
			CreateController<Control::WaspController>(ship, _player.GetHandle(), _hud, "Corvette");
			CREATE_OBJECTIVE4(Conditions::OnDestroy, AleaveSystem, "destroy the pursuers",
				std::vector<Actor::ConstHandle>({ ship.GetHandle() }), 1);
		};

		auto Aescape = CREATE_ACTION
		{
			CreateController<Control::TurtleController>(static_cast<Ship&>(**bossHandle), playerHandle, _hud, "BattleShip");
			_player.SetEnergy(0.f);
			CREATE_OBJECTIVE5(Conditions::IsClose, AlastAttack, "escape into the asteroid field",
				_player, ASTEROID_FIELD_POSITION, 300.f);
			ev->SetPosition(ESCAPE_POSITION);
			_hud.AddIndicator(*ev, "asteroid field", Color8U(1.f, 1.f, 0.f));

		//	auto& timer = *new Event<Conditions::Timer>(AlastAttack, "", 15.f);
		};

		auto Aaction = CREATE_ACTION
		{
			_hud.AddObjective(m_finishDescription);
		};

		auto AturnAround = CREATE_ACTION
		{

		/*	CREATE_OBJECTIVE4(Conditions::OnDestroy, Aaction, "destroy the battleship",
				std::vector<Actor::ConstHandle>({ bossHandle }), 1);*/
			_hud.ShowMessage("[Daymon Battleship] There you are. I trust that you didn't run into any trouble.");
			_hud.ShowMessage("[Doomwhisper] Nothing unusual. Once i've received the payment the object is yours and you'll never hear from me again.");
			auto& aggroTimer = *new Event<Conditions::Timer>(Aescape, "", 14.f);
			m_sceneGraph.Add(aggroTimer);
			_hud.ShowMessage("[Daymon Battleship] I'm afraid that there has been a change of plan. It involves eliminating risks.");
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
			CreateAsaultForce(std::vector<ShipType>{Dart, Fighter}, spawnPos, _player, 12, 13.5f, 1.5f);
		};

		auto& pirateTrigger = *new Event<Conditions::IsClose>(pirateAttack2, "", _player, MEETING_POSITION, 1000.f);
		m_sceneGraph.Add(pirateTrigger);

		/**/

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