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

		Graphic::EditField* m_mouseSensitivity;

		friend class SettingsState;
	};
}