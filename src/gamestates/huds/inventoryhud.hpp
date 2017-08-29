#pragma once

#include "graphic/interface/hud.hpp"

namespace GameStates
{
	enum Upgrades
	{
		ENERGY,
		ENERGY_REG,
		SHIELD,
		SHIELD_REG,
		SHIELD_REG_DELAY,
		HULL,
		COUNT
	};

	class InventoryHud : public Graphic::Hud
	{
	public:
		InventoryHud();

		void MouseMove(float _dx, float _dy) override;
	private:
		//Graphic::ScreenTexture* m_inventoryBackground;

		Graphic::DropField* m_inventoryField;
		Graphic::DropField* m_sellField;
		std::vector<Graphic::DropField*> m_weaponFields;
		Graphic::TextRender* m_descriptionLabel;

		Graphic::TextRender* m_shipInfoLabel;

		Graphic::Button* m_upgradeBtns[Upgrades::COUNT];

		Graphic::TextRender* m_upgradeLabels[Upgrades::COUNT];

		Graphic::TextRender* m_moneyLabel;

		friend class InventoryState; // state and hud are heavily depended
	};
}