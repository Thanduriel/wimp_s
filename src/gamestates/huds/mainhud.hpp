#pragma once

#include "graphic/interface/hud.hpp"
#include "gameplay/elements/ship.hpp"

namespace Game {
	class BaseEvent;
}

namespace GameStates {

	using namespace ei;

	class MainHud : public Graphic::Hud
	{
	public:
		MainHud();

		// process step to update timers of MessageBox
		void Process(float _deltaTime);

		void UpdateSpeedLabel(float _speed);
		void UpdateSpeedBar(float _speed, float _maxSpeed);
		void UpdateTargetSpeedSlider(float _targetSpeed, float _maxSpeed);
		void UpdateCrossHair(float _sprayRadius);
		void UpdateIndicators(Vec3 _playerPos);
		// refreshes currently shown objectives based on all Events known by this.
		void UpdateObjectives();
		void UpdateAimAssist(const ei::Vec2 _position);
		void ShowAimAssist(bool _show) { m_aimAssist->SetVisible(_show); }

		void AddIndicator(const Game::Actor& _actor, const std::string& _tag, Utils::Color8U _color = Graphic::Indicator::DEFAULT_COLOR);
		Graphic::Indicator* FindIndicator(const Game::Actor& _actor) const;
		void AddObjective(const Game::BaseEvent& _event);
		
		void ShowSpecialMoveMarker(bool _show) { m_specialMoveIndicator->SetVisible(_show); }

		// display message on the main message box
		void ShowMessage(const std::string& _message, float _time = Graphic::MessageBox::AUTO);
		void SetMessageAnimation(bool _active);
		Graphic::FillBar& GetEnergyBar() { return *m_energyBar; }
		Graphic::FillBar& GetHealthBar() { return *m_healthBar; }
		Graphic::FillBar& GetShieldBar() { return *m_shieldBar; }
	private:
		Graphic::MessageBox m_messageBox;

		Graphic::ScreenTexture* m_outerSpeedBar;
		Graphic::ScreenTexture* m_innerSpeedBar;
		Graphic::ScreenTexture* m_speedSlider;
		Graphic::TextRender* m_speedLabel;
		Graphic::ScreenTexture* m_crossHairDot;
		Graphic::ScreenTexture* m_crossHairLeft;
		Graphic::ScreenTexture* m_crossHairRight;
		Graphic::ScreenTexture* m_aimAssist;

		Graphic::ScreenTexture* m_specialMoveIndicator;
		Graphic::FillBar* m_energyBar;
		Graphic::FillBar* m_healthBar;
		Graphic::FillBar* m_shieldBar;

		std::vector<Graphic::Indicator*> m_indicators;

		std::vector<Game::Actor::ConstHandle> m_currentEvents;
		Graphic::TextRender* m_objectivesLabel;
	};
}