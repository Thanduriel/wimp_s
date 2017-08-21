#include "mainmenuhud.hpp"
#include "graphic/interface/pixelcoords.hpp"
#include "gamestates/mainstate.hpp"

namespace GameStates
{
	using namespace Graphic;
	using namespace ei;

	MainMenuHud::MainMenuHud()
	{
		//m_state = _state;
	//	m_background = &CreateScreenElement<ScreenTexture>("box_cutout", Vec2(0.0f, 0.0f), Vec2(2.0f, 2.0f), DefinitionPoint::MidMid, Anchor(DefinitionPoint::MidMid, this));
		m_startButton = &CreateScreenElement<Button>("menuBtn", Vec2(0.0f, 0.1f), Vec2(0.f), DefinitionPoint::MidMid, Anchor(DefinitionPoint::MidMid, this), "Start");
		m_startButton->Scale(ei::Vec2(0.6f, 0.6f));
		m_exitButton = &CreateScreenElement<Button>("menuBtn", Vec2(0.0f, -0.1f), Vec2(0.f), DefinitionPoint::MidMid, Anchor(DefinitionPoint::MidMid, this), "Exit");
		m_exitButton->Scale(ei::Vec2(0.6f, 0.6f));
	}
}
