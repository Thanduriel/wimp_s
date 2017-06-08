#include "playercontroller.hpp"
#include "graphic/core/device.hpp"
#include "graphic/core/opengl.hpp"
#include "camera.hpp"
#include "input.hpp"
#include "gameplay/elements/grid.hpp"
#include "gameplay/core/model.hpp"
#include "ei/3dintersection.hpp"


namespace Control
{
	using namespace ei;

	PlayerController::PlayerController(Game::Model& _model, Game::Grid& _grid, Game::Actor& _indicator)
		: m_model(&_model),
		m_mouseSensitivity(10.0f), 
		m_xForce(20.f, -20.f), 
		m_yForce(20.f, -20.f), 
		m_zForce(20.f, -20.f),
		m_targetingMode(TargetingMode::Normal),
		m_grid(_grid),
		m_indicator(_indicator)
	{};

	void PlayerController::Process(float _deltaTime)
	{
		// Apply the input to the model
		HandleInput(_deltaTime);
		
		if (m_targetingMode == TargetingMode::Tactical)
		{
			Vec3 pos = m_grid.GetPosition();
			pos.y = g_camera.GetPosition().y - 32.f;
			m_grid.SetPosition(pos);

			Plane plane(Vec3(0.f, 1.f, 0.f), pos);
			Ray ray = g_camera.GetRay(InputManager::GetCursorPosScreenSpace());
			float d = dot((pos - ray.origin), plane.n) / dot(ray.direction, plane.n);
			m_indicator.SetPosition(ray.origin + d * ray.direction);
		}
	}

	// ************************************************************ //
	void PlayerController::MouseMove(float _dx, float _dy)
	{
		m_mouseMovement = Vec2(_dx, _dy);
	}

	void PlayerController::KeyClick(int _key)
	{
		if (_key == GLFW_KEY_SPACE)
		{
			if (m_targetingMode == TargetingMode::Normal)
			{
 				m_targetingMode = TargetingMode::Tactical;
				const float angle = PI / 3.2f;
				g_camera.FixRotation(ei::Quaternion(Vec3(1.f, 0.f, 0.f), angle));
				// shift the camera back so that the player is in the center
				g_camera.SetPosition(m_model->GetPosition() + Vec3(0.f, 32.f, - 32.f / tan(angle)));
				m_grid.SetPosition(m_model->GetPosition());
			}
			else
			{
				m_targetingMode = TargetingMode::Normal;
				m_mouseMovement = Vec2(0.f);
				g_camera.Detach();
			}
		}
	}

	void PlayerController::Scroll(float _dx, float _dy)
	{
		if (m_targetingMode == TargetingMode::Tactical) g_camera.Translate(Vec3(0.f, _dy, 0.f));
	}

	// ************************************************************ //
	void PlayerController::HandleInput(float _deltaTime)
	{
		if (m_targetingMode == TargetingMode::Tactical)
		{
			// todo move this to a config
			const float tacticalCamSpeed = 10.f;
			Vec3 camVel(0.f);
			// todo: change this to virtual keys
			if (InputManager::IsKeyPressed(GLFW_KEY_W))
				camVel.z += tacticalCamSpeed;
			if (InputManager::IsKeyPressed(GLFW_KEY_S))
				camVel.z -= tacticalCamSpeed;
			if (InputManager::IsKeyPressed(GLFW_KEY_A))
				camVel.x -= tacticalCamSpeed;
			if (InputManager::IsKeyPressed(GLFW_KEY_D))
				camVel.x += tacticalCamSpeed;
			g_camera.Translate(camVel * _deltaTime);
		}
		else
		{
			Vec3 force(0.0f);
			if (glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_W))
				force[2] += m_zForce[0];
			if (glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_S))
				force[2] += m_zForce[1];
			if (glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_D))
				force[0] += m_xForce[0];
			if (glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_A))
				force[0] += m_xForce[1];
			if (glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_SPACE))
				force[1] += m_yForce[0];
			if (glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_LEFT_SHIFT))
				force[1] += m_yForce[1];
			// since we have newtons second axiom: F = m * a => a = F / m
			Vec3 acceleration = force / m_model->GetWeight();
			// and since delta_v = a * delta_t
			Vec3 deltaVelocity = acceleration * _deltaTime;
			// Apply the velocity to the player ship
	//		m_model->SetVelocity(m_model->GetVelocity() + deltaVelocity);
		}

		// mouse rotation
		if (m_mouseMovement.x + m_mouseMovement.y == 0.f) return;

		float dx = m_mouseSensitivity.x * _deltaTime * m_mouseMovement.x;
		float dy = m_mouseSensitivity.y * _deltaTime * m_mouseMovement.y;

		// this rotation needs to be inverted
		m_model->Rotate(Quaternion(dx, dy, 0.f));
		m_mouseMovement = Vec2(0.f);
	}
}