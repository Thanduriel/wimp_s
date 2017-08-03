#include "mainhud.hpp"
#include "graphic/interface/pixelcoords.hpp"
#include <string.h>

namespace GameStates {

	using namespace Graphic;
	using namespace ei;

	MainHud::MainHud()
	{
		ShowCursor(Hud::CursorType::Crosshair);

		float outerSpeedBarHeight = 512;
		m_outerSpeedBar = &CreateScreenElement<ScreenTexture>("speed_bar_outer", PixelOffset(10, 10), PixelOffset(64, outerSpeedBarHeight), DefP::BotLeft, Anchor(DefP::BotLeft, this));
		m_innerSpeedBar = &CreateScreenElement<ScreenTexture>("speed_bar_inner", PixelOffset(10, 10), PixelOffset(64, 512), DefP::BotLeft, Anchor(DefP::BotLeft, this));
		m_speedLabel = &CreateScreenElement<TextRender>(PixelOffset(0, 50), Anchor(DefP::BotLeft, m_outerSpeedBar));
		m_speedLabel->SetText("0 m/s");
		m_speedLabel->SetDefaultSize(0.7f);
		m_speedSlider = &CreateScreenElement<ScreenTexture>("targetspeed_slider", Vec2(0, 0.11328125f * m_outerSpeedBar->GetSize().y), PixelOffset(128, 32), DefP::MidMid, Anchor(DefP::BotMid, m_outerSpeedBar));
		m_crossHairDot = &CreateScreenElement<ScreenTexture>("crosshair_dot", PixelOffset(0, 0), PixelOffset(32, 32), DefP::MidMid, Anchor(DefP::MidMid, this));
		m_crossHairLeft = &CreateScreenElement<ScreenTexture>("crosshair_left", PixelOffset(0, 0), PixelOffset(64, 64), DefP::MidRight, Anchor(DefP::MidMid, m_crossHairDot));
		m_crossHairRight = &CreateScreenElement<ScreenTexture>("crosshair_right", PixelOffset(0, 0), PixelOffset(64, 64), DefP::MidLeft, Anchor(DefP::MidMid, m_crossHairDot));

		m_healthBar = &CreateScreenElement<FillBar>(PixelOffset(-36, 20), PixelOffset(220, 28), DefP::BotRight, Anchor(DefP::BotRight, this));
		m_healthBar->SetColor(Utils::Color8U(173_uc, 226_uc, 70_uc));
		m_energyBar = &CreateScreenElement<FillBar>(PixelOffset(0, 10), PixelOffset(220, 28), DefP::BotRight, Anchor(DefP::TopRight, m_healthBar));
	}

	void MainHud::UpdateSpeedLabel(float _speed)
	{
		std::string speedString = std::to_string(_speed);
		size_t pos = speedString.find('.', 0);
		// +2 for 1 character after point
		speedString = speedString.substr(0, pos + 2);
		//We only move between speeds of 0 - 999
		for (int i = 0; i < 3 - (int)pos; i++)
			speedString = " " + speedString;
		m_speedLabel->SetText(speedString + " m/s");
	}

	void MainHud::UpdateSpeedBar(float _speed, float _maxSpeed)
	{
		//Magic numbers FTW!!!
		//The relative heights from the bottom of the drawn texture to the top of the drawn texture
		Vec2 vec(1.0f, 0.11328125f + 0.7734375f * (_speed / _maxSpeed));
		if (_speed >= _maxSpeed - 0.01f)
			vec[1] = 1.0f;
		m_innerSpeedBar->SetScale(vec);
		m_innerSpeedBar->SetTextureRect(vec);
		m_innerSpeedBar->Scale(Vec2(1.f));
	}

	void MainHud::UpdateTargetSpeedSlider(float _targetSpeed, float _maxSpeed)
	{
		float height = 0.11328125f + 0.7734375f * (_targetSpeed / _maxSpeed);
		m_speedSlider->SetPosition(Vec2(0, height * m_outerSpeedBar->GetSize().y));
	}

	void MainHud::UpdateCrossHair(float _sprayRadius)
	{
		float maxSpread = 50.0f;
		float spread = maxSpread * _sprayRadius;
		m_crossHairLeft->SetPosition(PixelOffset(-spread, 0));
		m_crossHairRight->SetPosition(PixelOffset(spread, 0));
	}
}