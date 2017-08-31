#pragma once

#include "graphic/interface/hud.hpp"
#include "gamestate.hpp"

namespace GameStates {

	// Post game display with stats.
	class EndState : public GameStateHT<>
	{
	public:
		EndState();

		void Process(float _deltaTime) override;
		void Draw(float _deltaTime) override;

		void MouseMove(float _dx, float _dy) override;
		void Scroll(float _dx, float _dy) override;
		void KeyDown(int _key, int _modifiers) override;
		void KeyRelease(int _key) override;
		void KeyClick(int _key) override;
		void KeyDoubleClick(int _key) override;

	private:
	};
}