#include "playercontroller.hpp"
#include "graphic/core/device.hpp"
#include "graphic/core/opengl.hpp"
#include "camera.hpp"
#include "input.hpp"
#include "gameplay/elements/grid.hpp"
#include "gameplay/core/model.hpp"
#include "ei/3dintersection.hpp"
#include "gameplay/core/component.hpp"
#include "gameplay/elements/ship.hpp"
#include "gamestates/huds/mainhud.hpp"
#include "gameplay/scenegraph.hpp"

// test
#include "generators/weapongen.hpp"

namespace Control
{
	using namespace ei;

	Game::SceneGraph* PlayerController::s_sceneGraph = nullptr;

	const float TACTICALCAM_DIST = 32.f;

	PlayerController::PlayerController(Game::Ship& _ship, Game::Grid& _grid, Game::Actor& _indicator, GameStates::MainHud& _hud, GameTimeControl& _params)
		: m_ship(&_ship),
		m_hud(_hud),
		m_mouseSensitivity(10.0f),
		m_sliderSensitivity(100.0f),
		m_targetSpeed(10.0f),
		m_targetingMode(TargetingMode::Normal),
		m_grid(_grid),
		m_indicator(_indicator),
		m_controlParams(_params)
	{};

	void PlayerController::Process(float _deltaTime)
	{
		// Apply the input to the model
		HandleInput(_deltaTime);

		auto hndl = s_sceneGraph->RayCast(g_camera.GetRay(Vec2(0.f, 0.f)), 200.f, Game::CollisionComponent::Type::Ship);
		if (hndl)
		{
			m_hud.UpdateCrossHair(1.f);
			m_focus = std::move(hndl);
		}
		else
			m_hud.UpdateCrossHair(0.f);

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

				m_tacticalDirSign = m_ship->GetPosition().y < g_camera.GetPosition().y ? 1.f : -1.f;
				Quaternion rot = m_tacticalDirSign < 0.f ? Quaternion(Vec3(0.f, 0.f, 1.f), PI) : qidentity();
				// shift the camera back so that the player is in the center
				g_camera.FixRotation(ei::Quaternion(Vec3(1.f, 0.f, 0.f), m_tacticalDirSign * angle) * rot,
					m_ship->GetPosition() + Vec3(0.f, m_tacticalDirSign * TACTICALCAM_DIST, -TACTICALCAM_DIST / tan(angle)));
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

				m_grid.SetPosition(m_ship->GetPosition());
				component_cast<Game::GridComponent>(m_grid).ReverseTransition();
				m_controlParams.m_timeScale = 0.02f;
			}
			else
			{
				m_targetingMode = TargetingMode::Normal;
				component_cast<Game::GridComponent>(m_grid).ReverseTransition();
				m_mouseMovement = Vec2(0.f);
				g_camera.Attach(*m_ship);
				m_controlParams.m_timeScale = 1.f;
			}
		}

		// test stuff
		if (_key == GLFW_KEY_R)
		{
			static Graphic::TextRender& label = m_hud.CreateScreenElement<Graphic::TextRender>(Vec2(0.f),
				Graphic::ScreenPosition::Anchor(Graphic::DefP::TopLeft, &m_hud));
			label.SetDefaultSize(0.5f);
			Generators::WeaponGenerator gen;
			Game::Weapon* w = gen.Generate(5.f, 2.f);
			label.SetText(gen.GetName() + "\n-----\n" + gen.GetDescr());
			m_ship->SetWeapon(2, *w);
		}
	}

	void PlayerController::KeyDown(int _key, int _modifiers)
	{
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
			if (InputManager::IsVirtualKeyPressed(Control::VirtualKey::FIRE))
			{
				if(m_focus) m_ship->SetWeaponTarget(***m_focus);
				m_ship->Fire();
			}

			bool approximateTargetSpeed = true;
			if (InputManager::IsVirtualKeyPressed(Control::VirtualKey::ADJUST_TARGET_ACC))
			{
				// control target speed slider
				if (InputManager::IsVirtualKeyPressed(Control::VirtualKey::ACC_FORWARD))
					m_targetSpeed = ei::min(m_targetSpeed + m_sliderSensitivity * _deltaTime, m_ship->GetMaxSpeed());
				if (InputManager::IsVirtualKeyPressed(Control::VirtualKey::ACC_BACKWARD))
					m_targetSpeed = ei::max(m_targetSpeed - m_sliderSensitivity * _deltaTime, m_ship->GetMinSpeed());
			}
			else
			{
				// control ship speed manually
				if (InputManager::IsVirtualKeyPressed(Control::VirtualKey::ACC_FORWARD))
				{
					float newSpeed = m_ship->GetSpeed() + (m_ship->GetThrust() / m_ship->GetMass()) * _deltaTime;
					m_ship->SetSpeed(newSpeed);
					approximateTargetSpeed = false;
				}
				if (InputManager::IsVirtualKeyPressed(Control::VirtualKey::ACC_BACKWARD))
				{
					m_ship->SetSpeed(m_ship->GetSpeed() - (m_ship->GetThrust() / m_ship->GetMass()) * _deltaTime);
					approximateTargetSpeed = false;
				}
			}
			// approximate target speed with ship's (target) speed
			if (approximateTargetSpeed)
			{
				if (m_targetSpeed > m_ship->GetSpeed())
				{
					// accelerate ship's (target) speed
					m_ship->SetSpeed(min(m_ship->GetSpeed() + (m_ship->GetThrust() / m_ship->GetMass()) * _deltaTime, m_targetSpeed));
				}
				else if (m_targetSpeed < m_ship->GetSpeed())
				{
					// decelerate ship's (target) speed
					m_ship->SetSpeed(max(m_ship->GetSpeed() - (m_ship->GetThrust() / m_ship->GetMass()) * _deltaTime, m_targetSpeed));
				}
			}
		}

		// mouse rotation
		float z = 0.f;
		if (InputManager::IsVirtualKeyPressed(Control::VirtualKey::ROLL_CCW))
			z += 0.8f;
		if (InputManager::IsVirtualKeyPressed(Control::VirtualKey::ROLL_CW))
			z -= 0.8f;

		//if (m_mouseMovement.x + m_mouseMovement.y == 0.f) return;

		Vec2 cursor = InputManager::GetCursorPosScreenSpace();
		// constrain movement to a circle to have same behavior in both directions
		cursor.x = clamp(cursor.x * Graphic::Device::GetAspectRatio(), -1.f, 1.f);
		cursor = Vec2(sgn(cursor[0]), sgn(cursor[1])) * cursor * cursor;

		m_ship->SetTargetAngularVelocity(m_ship->GetRotationMatrix() * Vec3(-cursor[1], cursor[0], z));

		// update stats on the hud
		// this might not be the best place for this
		m_hud.UpdateSpeedLabel(m_ship->GetCurrentSpeed());
		m_hud.UpdateSpeedBar(m_ship->GetCurrentSpeed(), m_ship->GetMaxSpeed());
		m_hud.UpdateTargetSpeedSlider(m_targetSpeed, m_ship->GetMaxSpeed());
		m_hud.GetEnergyBar().SetFillLevel(m_ship->GetEnergy() / m_ship->GetMaxEnergy());
		m_hud.GetHealthBar().SetFillLevel(m_ship->GetHealth() / m_ship->GetMaxHealth());
	//	m_hud.UpdateCrossHair(m_ship->GetSprayRadius());
	}
}