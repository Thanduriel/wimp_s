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
		m_descriptionLabel = &CreateScreenElement<TextRender>(Vec2(0.05f,0.05f), ScreenPosition::Anchor(DefP::MidLeft, this));
		m_descriptionLabel->SetDefaultSize(0.5f);

		m_shipInfoLabel = &CreateScreenElement<TextRender>(Vec2(0.05f, -0.05f), ScreenPosition::Anchor(DefP::TopLeft, this));
		m_shipInfoLabel->SetDefaultSize(0.5f);

		m_energyUpgrade = &CreateScreenElement<Button>("upgradeBtn", m_shipInfoLabel->GetPosition() + Vec2(m_shipInfoLabel->GetRectangle().x, 0.0f), PixelOffset(25.0f, 25.0f), DefP::TopLeft, Anchor(DefP::TopLeft, this));
		m_energyRegUpgrade = &CreateScreenElement<Button>("upgradeBtn", m_shipInfoLabel->GetPosition() + Vec2(m_shipInfoLabel->GetRectangle().x, 0.0f), PixelOffset(25.0f, 25.0f), DefP::TopLeft, Anchor(DefP::TopLeft, this));
		m_shieldUpgrade = &CreateScreenElement<Button>("upgradeBtn", m_shipInfoLabel->GetPosition() + Vec2(m_shipInfoLabel->GetRectangle().x, 0.0f), PixelOffset(25.0f, 25.0f), DefP::TopLeft, Anchor(DefP::TopLeft, this));
		m_shieldRegUpgrade = &CreateScreenElement<Button>("upgradeBtn", m_shipInfoLabel->GetPosition() + Vec2(m_shipInfoLabel->GetRectangle().x, 0.0f), PixelOffset(25.0f, 25.0f), DefP::TopLeft, Anchor(DefP::TopLeft, this));
		m_shieldRegDelayUpgrade = &CreateScreenElement<Button>("upgradeBtn", m_shipInfoLabel->GetPosition() + Vec2(m_shipInfoLabel->GetRectangle().x, 0.0f), PixelOffset(25.0f, 25.0f), DefP::TopLeft, Anchor(DefP::TopLeft, this));
		m_lifeUpgrade = &CreateScreenElement<Button>("upgradeBtn", m_shipInfoLabel->GetPosition() + Vec2(m_shipInfoLabel->GetRectangle().x, 0.0f), PixelOffset(25.0f, 25.0f), DefP::TopLeft, Anchor(DefP::TopLeft, this));

		m_energyUpgradeLabel = &CreateScreenElement<TextRender>(Vec2(0.0f, 0.0f), ScreenPosition::Anchor(DefP::TopLeft, this));
		m_energyUpgradeLabel->SetDefaultSize(0.5f);
		m_energyRegUpgradeLabel = &CreateScreenElement<TextRender>(Vec2(0.0f, 0.0f), ScreenPosition::Anchor(DefP::TopLeft, this));
		m_energyRegUpgradeLabel->SetDefaultSize(0.5f);
		m_shieldUpgradeLabel = &CreateScreenElement<TextRender>(Vec2(0.0f, 0.0f), ScreenPosition::Anchor(DefP::TopLeft, this));
		m_shieldUpgradeLabel->SetDefaultSize(0.5f);
		m_shieldRegUpgradeLabel = &CreateScreenElement<TextRender>(Vec2(0.0f, 0.0f), ScreenPosition::Anchor(DefP::TopLeft, this));
		m_shieldRegUpgradeLabel->SetDefaultSize(0.5f);
		m_shieldRegDelayUpgradeLabel = &CreateScreenElement<TextRender>(Vec2(0.0f, 0.0f), ScreenPosition::Anchor(DefP::TopLeft, this));
		m_shieldRegDelayUpgradeLabel->SetDefaultSize(0.5f);
		m_lifeUpgradeLabel = &CreateScreenElement<TextRender>(Vec2(0.0f, 0.0f), ScreenPosition::Anchor(DefP::TopLeft, this));
		m_lifeUpgradeLabel->SetDefaultSize(0.5f);

		m_moneyLabel = &CreateScreenElement<TextRender>(Vec2(0.0f, 0.0f), ScreenPosition::Anchor(DefP::BotRight, this));
	}

	void InventoryHud::UpdateUpgradeLabels()
	{
		Vec2 margin = PixelOffset(10.0f, 0.0f);
		m_lifeUpgradeLabel->SetPosition(margin + m_shipInfoLabel->GetPosition() + Vec2(m_shipInfoLabel->GetRectangle().x, -9 * m_shipInfoLabel->GetCharSize().y * m_shipInfoLabel->GetDefaultSize()));
		m_lifeUpgrade->SetPosition(margin + m_lifeUpgradeLabel->GetPosition() + Vec2(m_lifeUpgradeLabel->GetRectangle().x, 0.0f));

		m_shieldUpgradeLabel->SetPosition(margin + m_shipInfoLabel->GetPosition() + Vec2(m_shipInfoLabel->GetRectangle().x, -5 * m_shipInfoLabel->GetCharSize().y * m_shipInfoLabel->GetDefaultSize()));
		m_shieldUpgrade->SetPosition(margin + m_shieldUpgradeLabel->GetPosition() + Vec2(m_shieldUpgradeLabel->GetRectangle().x, 0.0f));

		m_shieldRegUpgradeLabel->SetPosition(margin + m_shipInfoLabel->GetPosition() + Vec2(m_shipInfoLabel->GetRectangle().x, -6 * m_shipInfoLabel->GetCharSize().y * m_shipInfoLabel->GetDefaultSize()));
		m_shieldRegUpgrade->SetPosition(margin + m_shieldRegUpgradeLabel->GetPosition() + Vec2(m_shieldRegUpgradeLabel->GetRectangle().x, 0.0f));

		m_shieldRegDelayUpgradeLabel->SetPosition(margin + m_shipInfoLabel->GetPosition() + Vec2(m_shipInfoLabel->GetRectangle().x, -7 * m_shipInfoLabel->GetCharSize().y * m_shipInfoLabel->GetDefaultSize()));
		m_shieldRegDelayUpgrade->SetPosition(margin + m_shieldRegDelayUpgradeLabel->GetPosition() + Vec2(m_shieldRegDelayUpgradeLabel->GetRectangle().x, 0.0f));

		m_energyUpgradeLabel->SetPosition(margin + m_shipInfoLabel->GetPosition() + Vec2(m_shipInfoLabel->GetRectangle().x, -2 * m_shipInfoLabel->GetCharSize().y * m_shipInfoLabel->GetDefaultSize()));
		m_energyUpgrade->SetPosition(margin + m_energyUpgradeLabel->GetPosition() + Vec2(m_energyUpgradeLabel->GetRectangle().x, 0.0f));

		m_energyRegUpgradeLabel->SetPosition(margin + m_shipInfoLabel->GetPosition() + Vec2(m_shipInfoLabel->GetRectangle().x, -3 * m_shipInfoLabel->GetCharSize().y * m_shipInfoLabel->GetDefaultSize()));
		m_energyRegUpgrade->SetPosition(margin + m_energyRegUpgradeLabel->GetPosition() + Vec2(m_energyRegUpgradeLabel->GetRectangle().x, 0.0f));
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
