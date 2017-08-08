#pragma once

#include "graphic/interface/hud.hpp"
#include "gameplay/elements/ship.hpp"

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

		void AddIndicator(Game::Ship& _ship);
		
		void ShowSpecialMoveMarker(bool _show) { m_specialMoveIndicator->SetVisible(_show); }
		Graphic::FillBar& GetEnergyBar() { return *m_energyBar; }
		Graphic::FillBar& GetHealthBar() { return *m_healthBar; }
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

		std::vector<Graphic::Indicator*> m_indicators;
	};
}