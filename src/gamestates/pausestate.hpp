#pragma once
#include "gamestate.hpp"
#include "huds/pausehud.hpp"
#include "mainstate.hpp"

namespace GameStates
{
	class PauseState : public GameStateHT<PauseHud>
	{
	public:
		PauseState(MainState& _oldState);
		~PauseState();

		void Process(float _deltaTime) override;
		void Draw(float _deltaTime) override;
		void Dispose() override;

		void MouseMove(float _dx, float _dy) override;
		void Scroll(float _dx, float _dy) override;
		void KeyDown(int _key, int _modifiers) override;
		void KeyRelease(int _key) override;
		void KeyClick(int _key) override;
		void KeyDoubleClick(int _key) override;

	private:
		MainState* m_oldState;
	};
}
