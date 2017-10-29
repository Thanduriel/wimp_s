#include "controller.hpp"

namespace Control
{
	Game::SceneGraph* Controller::s_sceneGraph = nullptr;

	Controller::Controller(Game::Ship& _ship,  GameStates::MainHud& _hud)
		: Game::DynamicComponent<Game::SceneComponent>(_ship),
		m_lookForTarget(false),
		m_followTimeCounter(0.0f),
		m_evading(-1.0f),
		m_ship(_ship),
		m_hud(_hud)
	{
		// if this is changed the creation of controllers in Map needs to be updated
		m_canTick = false;
	}

	float Controller::RotateTowards(ei::Vec3 _pos)
	{
		using namespace ei;
	/*	Vec3 delta = _pos - GetShip().GetPosition();
		Vec3 forward = normalize(GetShip().GetRotationMatrix() * Vec3(0.0f, 0.0f, 1.0f));
		float angle = acos(dot(delta, forward));

		if (abs(angle) > 0.03f)
		{
			Vec3 axis = cross(delta, forward);
			Vec3 temp = cross(axis, forward);
			if (dot(temp, delta) < 0) angle = -angle;
			//	m_ship->SetRotation(Quaternion(Vec3(0.f, 0.f, 1.f), m_targetDirection));
			m_ship.SetTargetAngularVelocity(normalize(axis) *  angle * 1.2f);
		}*/
		Vec3 delta = _pos - GetShip().GetPosition();
		Vec3 forward = normalize(GetShip().GetRotationMatrix() * Vec3(0.0f, 0.0f, 1.0f));
		//Clamp the dot product to avoid getting a domain error
		float angle = acosf(clamp(dot(normalize(delta), forward), -1.0f, 1.0f));
		float factor = clamp(angle, 0.0f, ei::PI) / (ei::PI);
		Vec3 rotationVector = normalize(cross(forward, delta)) * sqrt(factor);
		GetShip().SetTargetAngularVelocity(rotationVector);

		return angle;
	}

	void Controller::FlyTo(const ei::Vec3& _pos, float _speed)
	{
		ei::Vec3 dif = _pos - m_ship.GetPosition();
		float len = ei::len(dif);
		float a = RotateTowards(_pos);
		if (a < 0.1f)
		{
			m_ship.SetSpeed(len < 5.f ? _speed : 0.f);
		}
	}

	void Controller::EvadeObstacles()
	{
		using namespace ei;
		Vec3 forward = normalize(GetShip().GetRotationMatrix() * Vec3(0.0f, 0.0f, 1.0f));
		Ray& ray = Ray(GetShip().GetPosition() + forward * 10.0f, forward);
		Game::Actor* hitObj = s_sceneGraph->RayCast(ray, m_minDistance);
		if (hitObj && hitObj != &**m_target)
			GetShip().SetTargetAngularVelocity(GetShip().GetRotationMatrix() * Vec3(1.0f, 0.0f, 0.0f));
	}
}