#include "mainhud.hpp"
#include "graphic/interface/pixelcoords.hpp"
#include "control/camera.hpp"
#include "ei/3dintersection.hpp"
#include "gameplay/events/event.hpp"
#include <string.h>

namespace GameStates {

	using namespace Graphic;
	using namespace ei;

	MainHud::MainHud()
	{
		ShowCursor(Hud::CursorType::Crosshair);

		GetDebugLabel().SetDefaultSize(0.7f);
#ifndef _DEBUG
		GetDebugLabel().SetVisible(false);
#endif // !_DEBUG

		float outerSpeedBarHeight = 512;
		m_outerSpeedBar = &CreateScreenElement<ScreenTexture>("speed_bar_outer", PixelOffset(10, 10), PixelOffset(64, outerSpeedBarHeight), DefP::BotLeft, Anchor(DefP::BotLeft, this));
		m_innerSpeedBar = &CreateScreenElement<ScreenTexture>("speed_bar_inner", PixelOffset(10, 10), PixelOffset(64, 512), DefP::BotLeft, Anchor(DefP::BotLeft, this));
		m_speedLabel = &CreateScreenElement<TextRender>(PixelOffset(0, 50), Anchor(DefP::BotLeft, m_outerSpeedBar));
		m_speedLabel->SetText("0 m/s");
		m_speedLabel->SetDefaultSize(0.7f);
		m_speedSlider = &CreateScreenElement<ScreenTexture>("targetspeed_slider", Vec2(0, 0.11328125f * m_outerSpeedBar->GetSize().y), PixelOffset(128, 32), DefP::MidMid, Anchor(DefP::BotMid, m_outerSpeedBar));
		m_crossHairDot = &CreateScreenElement<ScreenTexture>("crosshair_dot", PixelOffset(0, 0), PixelOffset(32, 32), DefP::MidMid, Anchor(DefP::MidMid, this));
		m_crossHairDot->SetColor(Utils::Color8U(0.f, 0.f, 0.f));
		m_crossHairLeft = &CreateScreenElement<ScreenTexture>("crosshair_left", PixelOffset(0, 0), PixelOffset(64, 64), DefP::MidRight, Anchor(DefP::MidMid, m_crossHairDot));
		m_crossHairRight = &CreateScreenElement<ScreenTexture>("crosshair_right", PixelOffset(0, 0), PixelOffset(64, 64), DefP::MidLeft, Anchor(DefP::MidMid, m_crossHairDot));

		// todo: different texture for this
		m_specialMoveIndicator = &CreateScreenElement<ScreenTexture>("targetspeed_slider", PixelOffset(0, 20), Vec2(0.f), DefP::BotMid, Anchor(DefP::BotMid, this));
		m_specialMoveIndicator->SetColor(Utils::Color8U(250_uc, 32_uc, 255_uc));
		m_healthBar = &CreateScreenElement<FillBar>(PixelOffset(-36, 20), PixelOffset(220, 28), DefP::BotRight, Anchor(DefP::BotRight, this));
		m_healthBar->SetColor(Utils::Color8U(173_uc, 226_uc, 70_uc));
		m_shieldBar = &CreateScreenElement<FillBar>(PixelOffset(0, 10), PixelOffset(220, 28), DefP::BotRight, Anchor(DefP::TopRight, m_healthBar));
		m_shieldBar->SetColor(Utils::Color8U(95_uc, 121_uc, 211_uc));
		
		m_energyBar = &CreateScreenElement<FillBar>(PixelOffset(-36, 0), PixelOffset(220, 28), DefP::MidRight, Anchor(DefP::MidRight, this));
		m_energyBar->SetColor(Utils::Color8U(35_uc, 71_uc, 141_uc));

		m_objectivesLabel = &CreateScreenElement<TextRender>(PixelOffset(170, -40), ScreenPosition::Anchor(DefP::TopMid, this));
		m_objectivesLabel->SetDefaultSize(0.6f);

		m_aimAssist = &CreateScreenElement<ScreenTexture>("crosshair_dot", PixelOffset(0, -20), PixelOffset(64, 64), DefP::MidMid);
		m_aimAssist->SetColor(Utils::Color8U(35_uc, 77_uc, 144_uc));
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

	void MainHud::UpdateIndicators(Vec3 _playerPos)
	{
		auto removeBeg = std::remove_if(m_indicators.begin(), m_indicators.end(), [this](Indicator* _indicator) 
		{
			if (_indicator->TargetIsDestroyed())
			{
				// the pointer is lost after this, so remove it right now
				DeleteScreenElement(*_indicator);
				return true;
			}
			else return false;
		});

		m_indicators.erase(removeBeg, m_indicators.end());

		for (auto indicator : m_indicators)
			indicator->Update();
	}

	void MainHud::UpdateObjectives()
	{
		// clear completed events
		auto it = std::remove_if(m_currentEvents.begin(), m_currentEvents.end(), [](const Game::Actor::ConstHandle& _hndl)
		{ return !*_hndl; });

		m_currentEvents.erase(it, m_currentEvents.end());

		std::string s;
		// collect objectives
		for (auto& ev : m_currentEvents)
			s += static_cast<const Game::BaseEvent&>(**ev).GetObjective() + "\n";

		m_objectivesLabel->SetText(s);
	}

	void MainHud::UpdateAimAssist(const ei::Vec2 _position)
	{
		m_aimAssist->SetPosition(_position);
	}

	// ******************************************************************** //
	void MainHud::AddIndicator(const Game::Actor& _target, const std::string& _tag, Utils::Color8U _color)
	{
		m_indicators.push_back(&CreateScreenElement<Indicator>(Vec2(0.0f, 0.0f), _target, *this, _tag, _color));
	}

	Indicator* MainHud::FindIndicator(const Game::Actor& _actor) const
	{
		auto it = std::find_if(m_indicators.begin(), m_indicators.end(), [&](const Indicator* _indicator)
		{
			return &_indicator->GetTarget() == &_actor;
		});

		return it != m_indicators.end() ? *it : nullptr;
	}

	void MainHud::AddObjective(const Game::BaseEvent& _event)
	{
		m_currentEvents.push_back(_event.GetHandle());
	}
}