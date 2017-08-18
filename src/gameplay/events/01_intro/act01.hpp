#pragma once

#include "../event.hpp"

namespace GameStates {
	class MainHud;
}

namespace Game {
	class Ship;
namespace Acts {
	
	class Act01
	{
	public:
		Act01(const Ship& _player, GameStates::MainHud& _hud);
	};
}
}