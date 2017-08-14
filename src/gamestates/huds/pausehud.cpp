#include "pausehud.hpp"
#include "graphic/interface/pixelcoords.hpp"
#include "gamestates/pausestate.hpp"

namespace GameStates
{
	using namespace Graphic;
	using namespace ei;

	PauseHud::PauseHud()
	{
		m_background = &CreateScreenElement<ScreenTexture>("vignette_back", Vec2(0.0f, 0.0f), Vec2(2.0f, 2.0f), DefinitionPoint::MidMid, Anchor(DefinitionPoint::MidMid, this));
		m_continueButton = &CreateScreenElement<Button>("menuBtn", Vec2(0.0f, 0.2f), PixelOffset(300.0f, 50.0f), DefinitionPoint::MidMid, Anchor(DefinitionPoint::MidMid, this), "Continue");
		m_optionsButton = &CreateScreenElement<Button>("menuBtn", Vec2(0.0f, 0.0f), PixelOffset(300.0f, 50.0f), DefinitionPoint::MidMid, Anchor(DefinitionPoint::MidMid, this), "Options");
		m_mainMenuButton = &CreateScreenElement<Button>("menuBtn", Vec2(0.0f, -0.2f), PixelOffset(300.0f, 50.0f), DefinitionPoint::MidMid, Anchor(DefinitionPoint::MidMid, this), "Back to menu");
	}
}
