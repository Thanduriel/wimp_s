#pragma once

#include "graphic/interface/hud.hpp"

namespace GameStates
{
	class MainMenuHud : public Graphic::Hud
	{
	public:
		MainMenuHud();

	private:
		Graphic::Button* m_startButton;
		Graphic::Button* m_optionsButton;
		Graphic::Button* m_creditsButton;
		Graphic::Button* m_exitButton;
		//GameStates::MainMenuState* m_state;
		Graphic::ScreenTexture* m_title;

		friend class MainMenuState;
	};
}