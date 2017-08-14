#pragma once

#include "graphic/interface/hud.hpp"

namespace GameStates
{
	class PauseHud : public Graphic::Hud
	{
	public:
		PauseHud();

		Graphic::Button& GetContinueButton() const { return *m_continueButton; };
		Graphic::Button& GetOptionsButton() const { return *m_optionsButton; };
		Graphic::Button& GetMenuButton() const { return *m_mainMenuButton; };
	private:
		Graphic::ScreenTexture* m_background;
		Graphic::Button* m_continueButton;
		Graphic::Button* m_optionsButton;
		Graphic::Button* m_mainMenuButton;
	};
}