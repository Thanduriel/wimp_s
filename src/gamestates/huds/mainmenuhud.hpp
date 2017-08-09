#pragma once

#include "graphic/interface/hud.hpp"

namespace GameStates
{
	class MainMenuHud : public Graphic::Hud
	{
	public:
		MainMenuHud();

		Graphic::Button& GetStartButton() { return *m_startButton; }
	private:
		Graphic::ScreenTexture* m_background;
		Graphic::Button* m_startButton;
		Graphic::Button* m_exitButton;
		//GameStates::MainMenuState* m_state;
	};
}