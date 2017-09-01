#include "transitionhud.hpp"
#include "graphic/interface/pixelcoords.hpp"

namespace GameStates {

	using namespace Graphic;
	using namespace ei;

	TransitionHud::TransitionHud()
		: m_titleLabel(PixelOffset(0, -20), Anchor(DefP::TopMid, this)),
		m_subtitleLabel(PixelOffset(0, -90), Anchor(DefP::TopMid, this)),
		m_storyLabel(Vec2(0, -0.3f), Anchor(DefP::TopMid, this)),
		m_loadBar(Vec2(0.f, 0.3f), PixelOffset(440, 56), DefP::BotMid, Anchor(DefP::BotMid, this), true),
		m_descriptionLabel(Vec2(0.f), Anchor(DefP::BotLeft, &m_loadBar)),
		m_finishLabel(Vec2(0.f, 0.2f), Anchor(DefP::BotMid,this))
	{
		m_titleLabel.Register(*this);
		m_subtitleLabel.Register(*this);
		m_loadBar.Register(*this);
		m_descriptionLabel.Register(*this);
		m_finishLabel.Register(*this);
		m_storyLabel.Register(*this);

		m_titleLabel.SetDefaultSize(1.5f);
		m_storyLabel.SetDefaultSize(0.434f);

		m_loadBar.SetFillLevel(0.7f);
		m_loadBar.SetColor(Utils::Color8U(1.f, 0.05f, 0.01f));

		m_descriptionLabel.SetText("loading nothing");
		m_descriptionLabel.SetDefaultSize(0.4f);
		m_finishLabel.SetText("press any key to continue...");
		m_finishLabel.SetDefaultSize(0.7f);
		m_finishLabel.SetVisible(false);
		CenterLabel(m_finishLabel);
	}

	void TransitionHud::CenterLabel(TextRender& _textRender)
	{
		Vec2 pos = _textRender.GetPosition();
		_textRender.SetPosition(Vec2(-0.5f *_textRender.GetRectangle().x, pos.y));
	}
}