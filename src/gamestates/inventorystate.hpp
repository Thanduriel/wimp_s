#pragma once
#include "gamestate.hpp"
#include "huds/inventoryhud.hpp"
#include "graphic/interface/hud.hpp"

namespace GameStates
{
	class InventoryState : public GameStateHT<InventoryHud>
	{
	public:
		InventoryState();
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
	};
}

