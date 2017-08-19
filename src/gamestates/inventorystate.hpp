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

	private:
		Game::Ship& m_ship;

		ei::Vec3 m_oldCamPosition;
		ei::Quaternion m_oldCamRotation;
	};
}

