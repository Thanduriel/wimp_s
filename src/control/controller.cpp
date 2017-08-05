#include "controller.hpp"

namespace Control
{
	Game::SceneGraph* Controller::s_sceneGraph = nullptr;

	Controller::Controller(Game::Ship& _ship, GameStates::MainHud& _hud)
		: m_ship(_ship),
		m_hud(_hud)
	{
	}
}