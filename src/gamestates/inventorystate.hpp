#pragma once
#include "gamestate.hpp"
#include "huds/inventoryhud.hpp"
#include "graphic/interface/hud.hpp"

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

		void MouseMove(float _dx, float _dy) override;
		void Scroll(float _dx, float _dy) override;
		void KeyDown(int _key, int _modifiers) override;
		void KeyRelease(int _key) override;
		void KeyClick(int _key) override;
		void KeyDoubleClick(int _key) override;

		void UpgradeValue(Upgrades _upgrade);

	private:
		Game::Ship& m_ship;

		ei::Vec3 m_oldCamPosition;
		ei::Quaternion m_oldCamRotation;

		int m_money;

		int m_upgradeLvls[Upgrades::COUNT];

		//The rows in which the upgradable values of the ship info label are displayed
		int* m_rows = new int[Upgrades::COUNT]{ 2, 3, 5, 6, 7, 9 };

		int GetUpgradeCost(Upgrades _upgrade);
		float NextUpgradeValue(Upgrades _upgrade);

		void UpdateUpgradeLabels();
	};
}