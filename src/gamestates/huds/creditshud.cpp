#include "creditshud.hpp"
#include "graphic/interface/pixelcoords.hpp"

namespace GameStates {

	using namespace Graphic;
	using namespace ei;

	CreditsHud::CreditsHud()
		: m_textRender(ei::Vec2(-1.f,1.f), ei::Vec2(2.f)),
		m_returnButton("menuBtn", Vec2(0.f), Vec2(0.f), DefinitionPoint::BotMid, Anchor(DefinitionPoint::BotMid, this), "Return")
	{
		m_textRender.Register(*this);
		m_returnButton.Register(*this);
		m_returnButton.Scale(Vec2(0.5f, 0.5f));

		m_textRender.Push("original prototype developed during 3DGameProject 2017 by\nRobert Jendersie\nSimon Schroer\nfurther development\nRobert Jendersie\nAdditional Assets", -1.f);
		m_textRender.SetCentered(true);
		m_textRender.Process(0.f);
	}
}