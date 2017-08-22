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

		void UpdateSpeedLabel(float _speed);
		void UpdateSpeedBar(float _speed, float _maxSpeed);
		void UpdateTargetSpeedSlider(float _targetSpeed, float _maxSpeed);
		void UpdateCrossHair(float _sprayRadius);
		void UpdateIndicators(Vec3 _playerPos);
		// refreshes currently shown objectives based on all Events known by this.
		void UpdateObjectives();

		void AddIndicator(const Game::Actor& _actor, const std::string& _tag, Utils::Color8U _color = Utils::Color8U(1.f,1.f,1.f));
		void AddObjective(const Game::BaseEvent& _event);
		
		void ShowSpecialMoveMarker(bool _show) { m_specialMoveIndicator->SetVisible(_show); }
		Graphic::FillBar& GetEnergyBar() { return *m_energyBar; }
		Graphic::FillBar& GetHealthBar() { return *m_healthBar; }
		Graphic::FillBar& GetShieldBar() { return *m_shieldBar; }
	private:
		Graphic::ScreenTexture* m_outerSpeedBar;
		Graphic::ScreenTexture* m_innerSpeedBar;
		Graphic::ScreenTexture* m_speedSlider;
		Graphic::TextRender* m_speedLabel;
		Graphic::ScreenTexture* m_crossHairDot;
		Graphic::ScreenTexture* m_crossHairLeft;
		Graphic::ScreenTexture* m_crossHairRight;

		Graphic::ScreenTexture* m_specialMoveIndicator;
		Graphic::FillBar* m_energyBar;
		Graphic::FillBar* m_healthBar;
		Graphic::FillBar* m_shieldBar;

		std::vector<Graphic::Indicator*> m_indicators;

		std::vector<Game::Actor::ConstHandle> m_currentEvents;
		Graphic::TextRender* m_objectivesLabel;
	};
}