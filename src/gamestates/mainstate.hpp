#pragma once

#include "graphic/interface/hud.hpp"
#include "gamestate.hpp"
#include "graphic/effects/starbackground.hpp"
#include "gameplay/core/actor.hpp"
#include "gameplay/scenegraph.hpp"

#include <vector>
#include <memory>

namespace Control {
	class PlayerController;
}

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

		Game::SceneGraph m_sceneGraph;
		//Actor vector (lol rhyme)
		// todo move this to a management class
		std::vector<std::unique_ptr<Game::Actor>> m_actors;
	};
}