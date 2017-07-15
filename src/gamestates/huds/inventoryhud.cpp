#include "inventoryhud.hpp"
#include "graphic/interface/pixelcoords.hpp"

namespace GameStates
{
	using namespace Graphic;
	using namespace ei;

	InventoryHud::InventoryHud()
	{
		//m_inventoryBackground = &CreateScreenElement<ScreenTexture>("simpleWindow", PixelOffset(0, 0), Vec2(2.0f, 2.0f), DefP::MidMid, Anchor(DefP::MidMid, this));
		//m_button = &CreateScreenElement<Button>("simpleWindow");
		std::vector<DropField*> dropFields = std::vector<DropField*>();
		m_field = &CreateScreenElement<DropField>("simpleWindow", PixelOffset(0, 0), PixelOffset(500, 500), DefP::TopRight, Anchor(DefP::TopRight, this));
		dropFields.push_back(m_field);
		m_item = &CreateScreenElement<DraggableTexture>("simpleWindow", PixelOffset(0, 0), PixelOffset(100, 100), DefP::MidMid, Anchor(DefP::MidMid, this), []() {return; }, dropFields);
	}
}
