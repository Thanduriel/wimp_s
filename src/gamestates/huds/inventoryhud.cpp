#include "inventoryhud.hpp"
#include "graphic/interface/pixelcoords.hpp"
#include "gameplay/elements/shipsystems/weapon.hpp"

namespace GameStates
{
	using namespace Graphic;
	using namespace ei;

	constexpr float BORDER_SIZE = 15;

	InventoryHud::InventoryHud()
		: m_shipInfoBackground("box_cutout", PixelOffset(BORDER_SIZE, -BORDER_SIZE), Vec2(0.f), DefP::TopLeft, Anchor(DefP::TopLeft, this)),
		m_moneyBackground("box_cutout", PixelOffset(0, -5), Vec2(1.f), DefP::TopLeft, Anchor(DefP::BotLeft, &m_shipInfoBackground)),
		m_itemBackground("box_cutout", PixelOffset(BORDER_SIZE, BORDER_SIZE), Vec2(0.68f, 0.80f), DefP::BotLeft, Anchor(DefP::BotLeft, this)),
		m_itemDescriptionBox(Vec2(0.f), m_itemBackground.GetSize(), DefP::TopLeft, Anchor(DefP::TopLeft, &m_itemBackground)),
		m_healthBar(PixelOffset(-36, 20), PixelOffset(220, 28), DefP::BotRight, Anchor(DefP::BotRight, this)),
		m_repairButton("slotBtn", PixelOffset(5, 0), m_healthBar.GetSize() * Vec2(0.3f, 1.f), DefP::BotRight, Anchor(DefP::TopRight, &m_healthBar), "Repair"),
		m_repairLabel(Vec2(0.f), ScreenPosition::Anchor(DefP::TopLeft, &m_healthBar))
	{
	//	m_shipInfoBackground.SetScale(Vec2(1.05f, 1.3f));
		m_shipInfoBackground.Register(*this);
		m_moneyBackground.Register(*this);
		m_itemBackground.Register(*this);
		m_itemDescriptionBox.Register(*this);
		m_itemDescriptionBox.SetCentered(true);
		m_healthBar.Register(*this);
		m_healthBar.SetColor(Utils::Color8U(173_uc, 226_uc, 70_uc));
		m_repairButton.Register(*this);
		m_repairLabel.Register(*this);
		m_repairLabel.SetText("trololo");
		m_repairLabel.SetDefaultSize(0.5f);
		m_repairLabel.SetPosition(Vec2(0.f, m_repairLabel.GetRectangle().y));
		
		// weapon related
		m_inventoryField = &CreateScreenElement<DropField>("box_uncut", PixelOffset(-BORDER_SIZE, -BORDER_SIZE), PixelOffset(400, 400), DefP::TopRight, Anchor(DefP::TopRight, this));
		m_weaponFields.push_back(m_inventoryField);
		m_shieldFields.push_back(m_inventoryField);
		m_descriptionLabel = &CreateScreenElement<TextRender>(Vec2(0.05f, -0.15f), ScreenPosition::Anchor(DefP::MidLeft, this));
		m_descriptionLabel->SetDefaultSize(0.5f);
		m_sellField = &CreateScreenElement<DropField>("box_uncut", Vec2(0.f), PixelOffset(96, 96), DefP::TopMid, Anchor(DefP::BotMid, m_inventoryField));
		m_weaponFields.push_back(m_sellField);
		m_shieldFields.push_back(m_sellField);

		m_sellLabel = &CreateScreenElement<TextRender>(Vec2(0.f), ScreenPosition::Anchor(DefP::BotLeft, m_sellField));
		m_sellLabel->SetText("sell");
		m_sellLabel->SetDefaultSize(0.8f);
		const Vec2 labelSize = m_sellLabel->GetRectangle();
		m_sellLabel->SetPosition(Vec2(-labelSize.x * 1.1f, m_sellField->GetSize().y * 0.5 + labelSize.y * 0.5f));

		m_sellAllButton = &CreateScreenElement<Button>("slotBtn",
			PixelOffset(25, 0),
			m_sellField->GetSize() * Vec2(0.7f, 0.7f), DefP::MidLeft, Anchor(DefP::MidRight , m_sellField));
		m_sellAllButton->SetCaption("all");

		// general information
		m_shipInfoLabel = &CreateScreenElement<TextRender>(Vec2(0.025f, -0.03f), ScreenPosition::Anchor(DefP::TopLeft, &m_shipInfoBackground));
		m_shipInfoLabel->SetDefaultSize(0.5f);

		// upgrades
		for (int i = 0; i < Upgrades::COUNT; i++)
		{
			m_upgradeBtns[i] = &CreateScreenElement<Button>("upgradeBtn", 
				m_shipInfoLabel->GetPosition() + Vec2(m_shipInfoLabel->GetRectangle().x, 0.0f), 
				PixelOffset(25, 25), DefP::MidLeft, Anchor(DefP::TopLeft, &m_shipInfoBackground));
			m_upgradeLabels[i] = &CreateScreenElement<TextRender>(Vec2(0.0f, 0.0f), ScreenPosition::Anchor(DefP::TopLeft, &m_shipInfoBackground));
			m_upgradeLabels[i]->SetDefaultSize(0.5f);
			// set content to the correct size
			// so that the buttons can be positioned behind
			m_upgradeLabels[i]->SetText("12345678"); 
		}

		m_moneyLabel = &CreateScreenElement<TextRender>(Vec2(0.015f, -0.02f), ScreenPosition::Anchor(DefP::TopLeft, &m_moneyBackground));
		m_moneyLabel->SetDefaultSize(0.5f);
	}

	void InventoryHud::MouseMove(float _dx, float _dy)
	{
		Hud::MouseMove(_dx, _dy);

		// in this hud nothing else can take focus right now
		if (const DraggableTexture* tex = dynamic_cast<DraggableTexture*>(m_preElem))
		{
			const Game::Weapon* itm = static_cast<const Game::Weapon*>(tex->GetContent());
		//	m_descriptionLabel->SetText(itm->GetName() + "\n--------\n" + itm->GetDescription());
			m_itemDescriptionBox.Push(itm->GetName() + "\n--------\n" + itm->GetDescription(), 0.1f);
			m_itemDescriptionBox.Process(1.f);
		}
	}
}
