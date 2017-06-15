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

	const float TACTICALCAM_DIST = 32.f;

	PlayerController::PlayerController(Game::Model& _model, Game::Grid& _grid, Game::Actor& _indicator)
		: m_model(&_model),
		m_mouseSensitivity(10.0f),
		m_xForce(200.f, -200.f),
		m_yForce(200.f, -200.f),
		m_zForce(200.f, -200.f),
		m_friction(1.5f),
		m_targetRotation(ei::qidentity()),
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

				m_tacticalDirSign = m_model->GetPosition().y < g_camera.GetPosition().y ? 1.f : -1.f;
				Quaternion rot = m_tacticalDirSign < 0.f ? Quaternion(Vec3(0.f, 0.f, 1.f), PI) : qidentity();
				// shift the camera back so that the player is in the center
				g_camera.FixRotation(ei::Quaternion(Vec3(1.f, 0.f, 0.f), m_tacticalDirSign * angle) * rot,
					m_model->GetPosition() + Vec3(0.f, m_tacticalDirSign * TACTICALCAM_DIST, -TACTICALCAM_DIST / tan(angle)));
				m_model->SetVelocity({});
		/*		Vec3 shift = m_model->GetRotationMatrix() * Vec3(0.f, 0.f, -1.f);
				shift.y = 0.f;
				shift = normalize(shift) * 32.f / tan(angle);
				shift.y = 32.f;
				Vec3 pos = m_model->GetPosition() + shift;
				Vec3 rotAxis = xaxis(m_model->GetRotation());
				rotAxis.y = 0.f;
				rotAxis = normalize(rotAxis);
				g_camera.FixRotation(Quaternion(rotAxis, angle),
					pos);*/
		
				m_grid.SetPosition(m_model->GetPosition());
			}
			else
			{
				m_targetingMode = TargetingMode::Normal;
				m_mouseMovement = Vec2(0.f);
				g_camera.Attach(*m_model);
			}
		}
	}

	void PlayerController::Scroll(float _dx, float _dy)
	{
		if (m_targetingMode == TargetingMode::Tactical)
		{
			g_camera.Translate(Vec3(0.f, _dy, 0.f));
			m_grid.Translate(Vec3(0.f, _dy, 0.f));
		}
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
				camVel.x -= tacticalCamSpeed * m_tacticalDirSign;
			if (InputManager::IsKeyPressed(GLFW_KEY_D))
				camVel.x += tacticalCamSpeed * m_tacticalDirSign;
			g_camera.Translate(camVel * _deltaTime);
			m_grid.Translate(camVel * _deltaTime);
		}
		else
		{
			Vec3 force(0.0f);
			Mat3x3 modelRotation = ei::rotation(m_model->GetRotation());
			Vec3 forward = modelRotation * Vec3(0.0f, 0.0f, 1.0f);
			Vec3 left = modelRotation * Vec3(1.0f, 0.0f, 0.0f);
			Vec3 up = modelRotation * Vec3(0.0f, 1.0f, 0.0f);
			if (glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_W))
				force += forward * m_zForce[0];
			if (glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_S))
				force += forward * m_zForce[1];
			if (glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_D))
				force += left * m_xForce[0];
			if (glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_A))
				force += left * m_xForce[1];
			if (glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_SPACE))
				force += up * m_yForce[0];
			if (glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_LEFT_SHIFT))
				force += up * m_yForce[1];
			// since we have newtons second axiom: F = m * a => a = F / m
			Vec3 acceleration = force / m_model->GetWeight();
			// and since delta_v = a * delta_t
			Vec3 deltaVelocity = acceleration * _deltaTime;
			// calculate the friction
			Vec3 friction = m_model->GetVelocity() * m_friction * _deltaTime;
			// Apply the velocity to the player ship
			m_model->SetVelocity(m_model->GetVelocity() + deltaVelocity - friction);
		}

		// mouse rotation
		//if (m_mouseMovement.x + m_mouseMovement.y == 0.f) return;

		float dx = m_mouseSensitivity.x * _deltaTime * m_mouseMovement.x;
		float dy = m_mouseSensitivity.y * _deltaTime * m_mouseMovement.y;

		//First yaw, then pitch
		/*m_model->Yaw(dx);
		m_model->Pitch(dy);*/
		//Yaw the target rotation
		Vec3 localY = ei::rotation(m_targetRotation) * Vec3(0.0f, 1.0f, 0.0f);
		localY *= dx;
		m_targetRotation = Quaternion(localY.x, localY.y, localY.z) * m_targetRotation;

		//Pitch the target rotation
		Vec3 localX = ei::rotation(m_targetRotation) * Vec3(1.0f, 0.0f, 0.0f);
		localX *= dy;
		m_targetRotation = Quaternion(localX.x, localX.y, localX.z) * m_targetRotation;

		//Calculate the smooth new rotation
		//todo: try fix jumping from 360 to 0 in slerp function
		m_model->SetRotation(ei::slerp(m_model->GetRotation(), m_targetRotation, 0.001f));

		m_mouseMovement = Vec2(0.f);
	}
}