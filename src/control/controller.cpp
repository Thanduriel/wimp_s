#include "controller.hpp"

namespace Control
{
	Game::SceneGraph* Controller::s_sceneGraph = nullptr;

	Controller::Controller(Game::Ship& _ship)
		: m_ship(_ship)
	{
	}
}