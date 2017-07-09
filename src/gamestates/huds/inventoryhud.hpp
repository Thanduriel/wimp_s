#pragma once

#include "graphic/interface/hud.hpp"

namespace GameStates
{
	class InventoryHud : public Graphic::Hud
	{
	public:
		InventoryHud();
	private:
		//Graphic::ScreenTexture* m_inventoryBackground;
		//Graphic::Button* m_button;
		Graphic::DraggableTexture* m_item;
	};
}

