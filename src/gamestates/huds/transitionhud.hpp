#pragma once

#include "graphic/interface/hud.hpp"

namespace GameStates {

	class TransitionHud : public Graphic::Hud
	{
	public:
		TransitionHud();
	private:
		void CenterLabel(Graphic::TextRender& _textRender);

		Graphic::TextRender m_titleLabel;
		Graphic::TextRender m_subtitleLabel;
		Graphic::TextRender m_storyLabel;
		Graphic::FillBar m_loadBar;
		Graphic::TextRender m_descriptionLabel;
		Graphic::TextRender m_finishLabel;

		friend class TransitionState;
	};
}