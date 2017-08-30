#pragma once

#include "huds/transitionhud.hpp"
#include "gamestate.hpp"
#include "gameplay/events/helpers.hpp"

namespace Graphic {
	class TextRender;
	class FillBar;
}

namespace Game {
	class Ship;
}

namespace GameStates {

	class TransitionState : public GameStateHT<TransitionHud>
	{
	public:
		TransitionState(Game::Level _level, Game::Ship* _player);

		void Process(float _deltaTime) override;
		void Draw(float _deltaTime) override;

		void MouseMove(float _dx, float _dy) override;
		void Scroll(float _dx, float _dy) override;
		void KeyDown(int _key, int _modifiers) override;
		void KeyRelease(int _key) override;
		void KeyClick(int _key) override;
		void KeyDoubleClick(int _key) override;

	private:
		Game::Map& CreateLevel(Game::Level _level);

		Game::Level m_level;
		Game::Ship* m_playerShip;

		int m_frameCount;
		bool m_isReady;
		class MainState* m_createdState;
	};
}