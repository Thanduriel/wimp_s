#pragma once

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

namespace Game {
	class Ship;
	class Map;
}

namespace GameStates{

	/* MainState *******************************
	 * The state in which the actual game runs.
	 */

	class MainState : public GameStateHT<MainHud>
	{
	public:
		MainState(Game::Ship& _player);
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

		static Game::Ship& CreatePlayerShip();
		Game::SceneGraph& GetSceneGraph() { return m_sceneGraph; }
		void SetMap(Game::Map& _map);
	private:
		// remove player and associated equipment from the scenegraph
		void RemovePlayer();

		Graphic::Starbackground m_starbackground;

		Game::Ship& m_playerShip; // needs to be stored to retrieve the player on closing
		std::unique_ptr<Game::Map> m_map;
		Control::PlayerController* m_playerController;
		Control::GameTimeControl m_gameTimeControl;
		float m_timeLeft; // timer after the player got destroyed

		Game::SceneGraph m_sceneGraph;
	};
}