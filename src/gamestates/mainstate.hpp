#pragma once

#include "graphic/interface/hud.hpp"
#include "huds/mainhud.hpp"
#include "gamestate.hpp"
#include "graphic/effects/starbackground.hpp"
#include "gameplay/core/actor.hpp"
#include "gameplay/scenegraph.hpp"
#include "control/playercontroller.hpp"

#include <vector>
#include <memory>

namespace Control {
	class PlayerController;
}

namespace GameStates{

	/* MainState *******************************
	 * The state in which the actual game runs.
	 */

	class MainState : public GameStateHT<MainHud>
	{
	public:
		MainState();
		~MainState();

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
		Graphic::Starbackground m_starbackground;

		Control::PlayerController* m_playerController;
		Control::GameTimeControl m_gameTimeControl;

		Game::SceneGraph m_sceneGraph;
	};
}