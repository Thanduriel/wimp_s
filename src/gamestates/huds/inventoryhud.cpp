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
		m_item = &CreateScreenElement<DraggableTexture>("simpleWindow", PixelOffset(0, 0), PixelCoord(100, 100), DefP::MidMid, Anchor(DefP::MidMid, this));
	}
}
