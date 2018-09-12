#pragma once
#include "huds/mainmenuhud.hpp"
#include "basicmenustate.hpp"

#include "gameplay/elements/grid.hpp"
#include "gameplay/elements/blackhole.hpp"

namespace GameStates
{
	class MainMenuState : public BasicMenuState<MainMenuHud>
	{
	public:
		MainMenuState();
		~MainMenuState();

		void Process(float _deltaTime) override;
		void Draw(float _deltaTime) override;
		void OnActivate() override;
		void Dispose() override;

		void MouseMove(float _dx, float _dy) override;
		void Scroll(float _dx, float _dy) override;
		void KeyDown(int _key, int _modifiers) override;
		void KeyRelease(int _key) override;
		void KeyClick(int _key) override;
		void KeyDoubleClick(int _key) override;

	private:
		Game::BlackHoleVis m_blackHole;
	};
}
