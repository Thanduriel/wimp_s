#pragma once

#include "hudelements.hpp"

// collection of hud elements that are intertwined with game specific features.

namespace Graphic {

	class MenuButton : public Button
	{
	public:
		using Button::Button;

		void MouseEnter() override;
	//	void MouseLeave() override;
	//	bool KeyDown(int _key, int _modifiers, ei::Vec2 _pos) override;
		bool KeyUp(int _key, int _modifiers, ei::Vec2 _pos) override;
	};
}