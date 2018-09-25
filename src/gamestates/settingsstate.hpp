#pragma once

#include "basicmenustate.hpp"
#include "huds/settingshud.hpp"

namespace GameStates {
	class SettingsState : public BasicMenuState<SettingsHud>
	{
	public:
		SettingsState();

		void Process(float _deltaTime) override;
		void Draw(float _deltaTime) override;
		void Dispose() override;

		void MouseMove(float _dx, float _dy) override;
		void Scroll(float _dx, float _dy) override;
		void KeyDown(int _key, int _modifiers) override;
		void KeyRelease(int _key) override;
		void KeyClick(int _key) override;
		void KeyDoubleClick(int _key) override;

	private:
		ei::Vec2 m_screenSize;
		bool m_isFullScreen;
		bool m_enableAimAssist;
		int m_currentResolution;
		float m_masterVolume;
	};
}