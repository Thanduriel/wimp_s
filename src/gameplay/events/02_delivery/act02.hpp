#pragma once

#include "../event.hpp"
#include "../helpers.hpp"

namespace GameStates {
	class MainHud;
}

namespace Game {
	class Ship;
	namespace Acts {

		class Act02 : public Map
		{
		public:
			Act02(SceneGraph& _sceneGraph, Ship& _player, GameStates::MainHud& _hud);

		private:
		};
	}
}