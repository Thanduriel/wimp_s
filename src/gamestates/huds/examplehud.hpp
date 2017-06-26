#pragma once

#include "graphic/interface/hud.hpp"

namespace GameStates {
	
	class ExampleHud : public Graphic::Hud
	{
	public:
		ExampleHud();
	private:
		Graphic::FillBar* m_fillBar;
		Graphic::TextRender* m_textRender;
		Graphic::ScreenTexture* m_screenTexture01;
		Graphic::ScreenTexture* m_screenTexture02;
		Graphic::Button* m_button;
	};
}