#include "controller.hpp"

namespace Control
{
	Game::SceneGraph* Controller::s_sceneGraph = nullptr;

	Controller::Controller(Game::Ship& _ship, GameStates::MainHud& _hud)
		: Game::DynamicComponent<Game::SceneComponent>(_ship),
		m_ship(_ship),
		m_hud(_hud)
	{
	}

	void Controller::RotateTowards(ei::Vec3 _pos)
	{
		using namespace ei;
		Vec3 delta = _pos - GetShip().GetPosition();
		Vec3 forward = normalize(GetShip().GetRotationMatrix() * Vec3(0.0f, 0.0f, 1.0f));
		//Clamp the dot product to avoid getting a domain error
		float angle = acosf(clamp(dot(normalize(delta), forward), -1.0f, 1.0f));
		float factor = clamp(angle, 0.0f, ei::PI) / (ei::PI);
		Vec3 rotationVector = normalize(cross(forward, delta)) * sqrt(factor);
		GetShip().SetTargetAngularVelocity(rotationVector);
	}
}