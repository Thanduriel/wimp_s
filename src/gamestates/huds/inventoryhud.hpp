#pragma once

#include "graphic/interface/hud.hpp"

namespace GameStates
{
	class InventoryHud : public Graphic::Hud
	{
	public:
		InventoryHud();

		void MouseMove(float _dx, float _dy) override;

		void UpdateUpgradeLabels();
	private:
		//Graphic::ScreenTexture* m_inventoryBackground;
		std::vector<Graphic::DraggableTexture*> m_vicinityItems;
		Graphic::DropField* m_vicinityField;
		Graphic::DropField* m_inventoryField;

		std::vector<Graphic::DropField*> m_weaponFields;
		Graphic::TextRender* m_descriptionLabel;

		Graphic::TextRender* m_shipInfoLabel;

		Graphic::Button* m_lifeUpgrade;
		Graphic::Button* m_shieldUpgrade;
		Graphic::Button* m_shieldRegUpgrade;
		Graphic::Button* m_shieldRegDelayUpgrade;
		Graphic::Button* m_energyUpgrade;
		Graphic::Button* m_energyRegUpgrade;

		Graphic::TextRender* m_lifeUpgradeLabel;
		Graphic::TextRender* m_shieldUpgradeLabel;
		Graphic::TextRender* m_shieldRegUpgradeLabel;
		Graphic::TextRender* m_shieldRegDelayUpgradeLabel;
		Graphic::TextRender* m_energyUpgradeLabel;
		Graphic::TextRender* m_energyRegUpgradeLabel;

		Graphic::TextRender* m_moneyLabel;

		friend class InventoryState; // state and hud are heavily depended
	};
}