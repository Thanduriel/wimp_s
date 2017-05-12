#pragma once

#include "gamestate.hpp"

namespace GameStates{

	/* MainState *******************************
	 * The state in wich the actual game runs.
	 */

	class MainState : public GameState
	{
	public:
		MainState();
		~MainState();

		void Process(float _deltaTime) override;
		void Draw(float _deltaTime) override;
	};
}