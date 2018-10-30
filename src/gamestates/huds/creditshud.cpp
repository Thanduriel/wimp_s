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

		m_textRender.Push("original prototype developed during 3DGameProject 2017 by\n"
			"<c 0 255 0>Robert Jendersie\nSimon Schroer</c>\n"
			"\nfurther development\n"
			"<c 0 255 0>Robert Jendersie</c>\n"
			"\nAdditional Assets\n"
			"sounds - freesound.org\n"
			"<c 0 255 0>Quaker540, ryansnook, unfa, Zovex, bubaproducer, niamhd00145229, Adam_N</c>\n"
			"\nmodels - turbosquid\n"
			"<c 0 255 0>GrafxBox</c>"
			"\nmodels - free3d\n"
			"<c 0 255 0>thomasmattia</c>", -1.f);
		m_textRender.SetFontSize(0.5f);
		m_textRender.SetCentered(true);
		m_textRender.Process(0.f);
	}
}