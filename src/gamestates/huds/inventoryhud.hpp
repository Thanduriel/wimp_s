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
		std::vector<Graphic::DraggableTexture*> m_vicinityItems;
		Graphic::DropField* m_vicinityField;
		Graphic::DropField* m_inventoryField;
	};
}