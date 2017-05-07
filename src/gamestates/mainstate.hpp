#pragma once

#include "gamestate.hpp"

namespace GameStates{

	class MainState : public GameState
	{
	public:
		void Process(float _deltaTime) override;
		void Draw(float _deltaTime) override;
	};
}