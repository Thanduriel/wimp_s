#pragma once

#include "graphic/interface/hud.hpp"

namespace GameStates {

	class MainHud : public Graphic::Hud
	{
	public:
		MainHud();

		void UpdateSpeedLabel(float _speed);
		void UpdateSpeedBar(float _speed, float _maxSpeed);
		void UpdateTargetSpeedSlider(float _targetSpeed, float _maxSpeed);
	private:
		Graphic::ScreenTexture* m_outerSpeedBar;
		Graphic::ScreenTexture* m_innerSpeedBar;
		Graphic::ScreenTexture* m_speedSlider;
		Graphic::TextRender* m_speedLabel;
	};
}