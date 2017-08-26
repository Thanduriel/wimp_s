#pragma once

#include "../event.hpp"
#include "../helpers.hpp"
#include "generators/asteroidfield.hpp"

namespace GameStates {
	class MainHud;
}

namespace Game {
	class Ship;
namespace Acts {
	
	class Act01 : public Map
	{
	public:
		Act01(SceneGraph& _sceneGraph, const Ship& _player, GameStates::MainHud& _hud);

	private:
		Generators::AsteroidField m_asteroids;
	};
}
}