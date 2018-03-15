#include "pausehud.hpp"
#include "graphic/interface/pixelcoords.hpp"
#include "gamestates/pausestate.hpp"
#include "graphic/interface/customelements.hpp"

namespace GameStates
{
	using namespace Graphic;
	using namespace ei;

	PauseHud::PauseHud()
	{
		m_background = &CreateScreenElement<ScreenTexture>("vignette_back", Vec2(0.0f, 0.0f), Vec2(2.0f, 2.0f), DefinitionPoint::MidMid, Anchor(DefinitionPoint::MidMid, this));
		m_optionsButton = &CreateScreenElement<MenuButton>("menuBtn", Vec2(0.0f, 0.0f), PixelOffset(300, 80), DefinitionPoint::MidMid, Anchor(DefinitionPoint::MidMid, this), "Options");
		m_continueButton = &CreateScreenElement<MenuButton>("menuBtn", Vec2(0.0f, 0.0f), PixelOffset(300, 80), DefinitionPoint::BotMid, Anchor(DefinitionPoint::TopMid, m_optionsButton), "Continue");
		m_mainMenuButton = &CreateScreenElement<MenuButton>("menuBtn", Vec2(0.0f, -0.0f), PixelOffset(300, 80), DefinitionPoint::TopMid, Anchor(DefinitionPoint::BotMid, m_optionsButton), "Menu");
	}
}
