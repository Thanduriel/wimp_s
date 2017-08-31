#pragma once

#include <functional>

#include "gameplay/scenegraph.hpp"
#include "gameplay/elements/ship.hpp"
#include "generators/weapongen.hpp"

// some macros to simplify the syntax of event creation
/* A reasonable default capture list for actions:
 * * other lambdas by copy NECESSARY
 * * Handles by copy NECESSARY
 * * this-pointer and members by reference
 */

#define CREATE_ACTION [=, &_hud, &_player]()
// Choose the variant with the appropriate amount of arguments. Since the type has to always be provided it does not count.
#define CREATE_OBJECTIVE2(type, arg0, arg1) auto* ev = &FactoryActor::GetThreadLocalInstance().Make<Event< type >>(arg0,arg1); \
											_hud.AddObjective(*ev);
#define CREATE_OBJECTIVE3(type, arg0, arg1, arg2) auto* ev = &FactoryActor::GetThreadLocalInstance().Make<Event< type >>(arg0,arg1, arg2); \
											_hud.AddObjective(*ev);
#define CREATE_OBJECTIVE4(type, arg0, arg1, arg2, arg3) auto* ev = &FactoryActor::GetThreadLocalInstance().Make<Event< type >>(arg0,arg1, arg2, arg3); \
											_hud.AddObjective(*ev);
#define CREATE_OBJECTIVE5(type, arg0, arg1, arg2, arg3, arg4) auto* ev = &FactoryActor::GetThreadLocalInstance().Make<Event< type >>(arg0,arg1, arg2, arg3, arg4); \
											_hud.AddObjective(*ev);


namespace Game {

	enum struct Level {
		Act01,
		Act02,
		Act03,
		Act04,
		Act05,
		COUNT
	};


	class Map
	{
	public:
		Map(SceneGraph& _sceneGraph);
		virtual ~Map() {}

		Level GetNextLevel() const { return m_nextLevel; }
		bool IsFinished() const { return m_isFinished; }
	protected:
		typedef std::function<void()> Objective;

		enum struct Drop
		{
			Weapons,
			Credits,
			Nothing
		};
		// Creates a ship with Controller type T.
		// Equips it with _numWeapons. Does not check for available sockets.
		// Additional Arguments are forwarded to the Controller's constructor.
		Ship& CreateShip(const std::string& _type, const ei::Vec3& _position, int _numWeapons, float _power, float _rarityMod = 1.f, Drop _drop = Drop::Weapons);

		template<typename T, typename... Args>
		T& CreateController(Args&&... _args)
		{
			auto& c = *new T(std::forward<Args>(_args)...);
			m_sceneGraph.RegisterComponent(c);

			return c;
		}

		void SetupPlayer(Ship& _player, const ei::Vec3& _position, const ei::Quaternion& _rotation = ei::qidentity(), float _speed = 200.f);
		void FinishMap(Level _nextLevel);

		SceneGraph& m_sceneGraph;
		Generators::WeaponGenerator m_weaponGen;

	private:
		Level m_nextLevel;
		bool m_isFinished;
	};
}