#pragma once

#include "graphic/interface/hud.hpp"

namespace GameStates {
	class CreditsHud : public Graphic::Hud
	{
	public:
		CreditsHud();

	private:
		Graphic::MessageBox m_textRender;
		Graphic::Button m_returnButton;

		friend class CreditsState;
	};
}