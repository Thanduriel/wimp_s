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
		m_title = &CreateScreenElement<ScreenTexture>("wimp_s", Vec2(0.0f, 0.0f), Vec2(0.f), DefinitionPoint::TopMid, Anchor(DefinitionPoint::TopMid, this));
		m_title->Scale(ei::Vec2(0.54f, 0.54f));
		m_startButton = &CreateScreenElement<Button>("menuBtn", Vec2(0.0f, 0.0f), Vec2(0.f), DefinitionPoint::BotMid, Anchor(DefinitionPoint::MidMid, this), "Start");
		m_startButton->Scale(ei::Vec2(0.6f, 0.6f));
		m_optionsButton = &CreateScreenElement<Button>("menuBtn", Vec2(0.0f, 0.0f), Vec2(0.f), DefinitionPoint::TopLeft, Anchor(DefinitionPoint::BotLeft, m_startButton), "Options");
		m_optionsButton->Scale(ei::Vec2(0.6f, 0.6f));
		m_creditsButton = &CreateScreenElement<Button>("menuBtn", Vec2(0.0f, -0.0f), Vec2(0.f), DefinitionPoint::TopLeft, Anchor(DefinitionPoint::BotLeft, m_optionsButton), "Credits");
		m_creditsButton->Scale(ei::Vec2(0.6f, 0.6f));
		m_exitButton = &CreateScreenElement<Button>("menuBtn", Vec2(0.0f, -0.0f), Vec2(0.f), DefinitionPoint::TopLeft, Anchor(DefinitionPoint::BotLeft, m_creditsButton), "Exit");
		m_exitButton->Scale(ei::Vec2(0.6f, 0.6f));
	}
}
