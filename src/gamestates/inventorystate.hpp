#pragma once
#include "gamestate.hpp"
#include "huds/inventoryhud.hpp"
#include "graphic/interface/hud.hpp"
#include "gameplay/elements/shipsystems/item.hpp"

namespace Game {
	class Ship;
}

namespace GameStates
{
	class InventoryState : public GameStateHT<InventoryHud>
	{
	public:
		InventoryState(Game::Ship& _ship);
		~InventoryState();

		void Process(float _deltaTime) override;
		void Draw(float _deltaTime) override;
		void Dispose() override;
		void OnActivate() override;

		void MouseMove(float _dx, float _dy) override;
		void Scroll(float _dx, float _dy) override;
		void KeyDown(int _key, int _modifiers) override;
		void KeyRelease(int _key) override;
		void KeyClick(int _key) override;
		void KeyDoubleClick(int _key) override;

		void UpgradeValue(Upgrades _upgrade);

	private:
		Graphic::DropField& CreateItemSocket(ei::Vec2 _position, const Game::Item* _item);

		Game::Ship& m_ship;
		std::unordered_map<const Game::Item*, Graphic::ItemIcon*> m_itemIcons;

		Control::Camera m_oldCamera;

		int m_money;
		bool m_shouldPlaySounds;

		std::array<int, Upgrades::COUNT>& m_upgradeLvls;

		//The rows in which the upgradeable values of the ship info label are displayed
		std::array<int, Upgrades::COUNT> m_rows = { 2, 3, 5, 6, 7, 9 };

		int GetUpgradeCost(Upgrades _upgrade);
		float NextUpgradeValue(Upgrades _upgrade);

		void UpdateUpgradeLabels();
	};
}