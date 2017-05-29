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

	PlayerController::PlayerController(Game::Model& _model, Game::Grid& _grid)
		: m_model(&_model),
		m_mouseSensitivity(0.5f), 
		m_xAcceleration(20.f, -20.f), 
		m_yAcceleration(20.f, -20.f), 
		m_zAcceleration(20.f, -20.f),
		m_targetingMode(TargetingMode::Normal),
		m_grid(_grid)
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
			m_model->SetPosition(ray.origin + d * ray.direction);
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
	/*	Vec3 velocity(0.f);
		// basic movement (no intertia yet)
		if (glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_W))
			velocity[2] += m_zAcceleration[0];
		if (glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_S))
			velocity[2] += m_zAcceleration[1];
		if (glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_D))
			velocity[0] += m_xAcceleration[0];
		if (glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_A))
			velocity[0] += m_xAcceleration[1];
		if (glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_SPACE))
			velocity[1] += m_yAcceleration[0];
		if (glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_LEFT_SHIFT))
			velocity[1] += m_yAcceleration[1];
		SetVelocity(m_rotationMatrix * velocity);*/

		// mouse rotation
		if (m_mouseMovement.x + m_mouseMovement.y == 0.f) return;

		float dx = m_mouseSensitivity.x * _deltaTime * m_mouseMovement.x;
		float dy = m_mouseSensitivity.y * _deltaTime * m_mouseMovement.y;

		// this rotation needs to be inverted
		m_model->Rotate(Quaternion(dx, dy, 0.f));
		m_mouseMovement = Vec2(0.f);
	}
}