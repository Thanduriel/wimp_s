#pragma once

#include "graphic/interface/hud.hpp"
#include "gamestate.hpp"
#include "graphic/effects/starbackground.hpp"

namespace GameStates{

	/* MainState *******************************
	 * The state in wich the actual game runs.
	 */

	class MainState : public GameStateHT<Graphic::Hud>
	{
	public:
		MainState();
		~MainState();

		void Process(float _deltaTime) override;
		void Draw(float _deltaTime) override;

		void MouseMove(float _dx, float _dy) override;
		void Scroll(float _dx, float _dy) override;
		void KeyDown(int _key, int _modifiers) override;
		void KeyRelease(int _key) override;
		void KeyClick(int _key) override;
		void KeyDoubleClick(int _key) override;

	private:
		Graphic::Starbackground m_starbackground;
	};
}