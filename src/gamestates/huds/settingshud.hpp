#pragma once

#include "graphic/interface/hud.hpp"

namespace GameStates {

	class SettingsHud : public Graphic::Hud
	{
	public:
		SettingsHud();
	private:
		Graphic::Button* m_applyButton;
		Graphic::Button* m_cancelButton;

		Graphic::Button* m_aimAssistButton;
		Graphic::EditField* m_mouseSensitivity;
		Graphic::Button* m_resolutionButton;
		Graphic::Button* m_fullScreenButton;
		Graphic::EditField* m_frameCountTarget;

		Graphic::EditField* m_brightness;
		Graphic::EditField* m_contrast;

		Graphic::EditField* m_masterVolume;

		friend class SettingsState;
	};
}