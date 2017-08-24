#include "inventoryhud.hpp"
#include "graphic/interface/pixelcoords.hpp"
#include "gameplay/elements/shipsystems/weapon.hpp"

namespace GameStates
{
	using namespace Graphic;
	using namespace ei;

	InventoryHud::InventoryHud()
	{
		//m_inventoryBackground = &CreateScreenElement<ScreenTexture>("box_uncut", PixelOffset(0, 0), Vec2(2.0f, 2.0f), DefP::MidMid, Anchor(DefP::MidMid, this));
/*		std::vector<DropField*> dropFields = std::vector<DropField*>();
		m_vicinityItems = std::vector<DraggableTexture*>();
		m_vicinityField = &CreateScreenElement<DropField>("box_uncut", PixelOffset(0, 0), PixelOffset(500, 500), DefP::TopLeft, Anchor(DefP::TopLeft, this));
		dropFields.push_back(m_vicinityField);
		dropFields.push_back(m_inventoryField);
		//Create some items for testing purposes
		for (int i = 0; i < 10; i++)
		{
			DraggableTexture* item = &CreateScreenElement<DraggableTexture>("box_uncut", PixelOffset(0, 0), PixelOffset(100, 100), DefP::MidMid, Anchor(DefP::MidMid, this), dropFields);
			m_vicinityItems.push_back(item);
			m_vicinityField->DropElement(*item);
			item->SetPosition(item->GetBackupPosition());
		}*/
		m_inventoryField = &CreateScreenElement<DropField>("box_uncut", PixelOffset(0, 0), PixelOffset(400, 400), DefP::TopRight, Anchor(DefP::TopRight, this));
		m_weaponFields.push_back(m_inventoryField);
		m_descriptionLabel = &CreateScreenElement<TextRender>(Vec2(0.05f, 0.05f), ScreenPosition::Anchor(DefP::MidLeft, this));
		m_descriptionLabel->SetDefaultSize(0.5f);

		m_shipInfoLabel = &CreateScreenElement<TextRender>(Vec2(0.05f, -0.05f), ScreenPosition::Anchor(DefP::TopLeft, this));
		m_shipInfoLabel->SetDefaultSize(0.5f);

		for (int i = 0; i < Upgrades::COUNT; i++)
		{
			m_upgradeBtns[i] = &CreateScreenElement<Button>("upgradeBtn", m_shipInfoLabel->GetPosition() + Vec2(m_shipInfoLabel->GetRectangle().x, 0.0f), PixelOffset(25.0f, 25.0f), DefP::TopLeft, Anchor(DefP::TopLeft, this));
			m_upgradeLabels[i] = &CreateScreenElement<TextRender>(Vec2(0.0f, 0.0f), ScreenPosition::Anchor(DefP::TopLeft, this));
			m_upgradeLabels[i]->SetDefaultSize(0.5f);
		}

		m_moneyLabel = &CreateScreenElement<TextRender>(Vec2(0.0f, 0.0f), ScreenPosition::Anchor(DefP::TopLeft, this));
		m_moneyLabel->SetDefaultSize(0.5f);
	}

	void InventoryHud::MouseMove(float _dx, float _dy)
	{
		Hud::MouseMove(_dx, _dy);

		// in this hud nothing else can take focus right now
		if (const DraggableTexture* tex = dynamic_cast<DraggableTexture*>(m_preElem))
		{
			const Game::Weapon* itm = static_cast<const Game::Weapon*>(tex->GetContent());
			m_descriptionLabel->SetText(itm->GetName() + "\n--------\n" + itm->GetDescription());
		}
	}
}
