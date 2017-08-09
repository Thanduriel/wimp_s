#pragma once
#include "gamestate.hpp"
#include "huds/mainmenuhud.hpp"

namespace GameStates
{
	class MainMenuState : public GameStateHT<MainMenuHud>
	{
	public:
		MainMenuState();
		~MainMenuState();

		void Process(float _deltaTime) override;
		void Draw(float _deltaTime) override;
		void Dispose() override;

		void StartGame();

		void MouseMove(float _dx, float _dy) override;
		void Scroll(float _dx, float _dy) override;
		void KeyDown(int _key, int _modifiers) override;
		void KeyRelease(int _key) override;
		void KeyClick(int _key) override;
		void KeyDoubleClick(int _key) override;
	};
}
