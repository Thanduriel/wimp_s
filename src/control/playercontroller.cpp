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
#include "math/extensions.hpp"
#include "controller.hpp"
#include "gameplay/elements/factorycomponent.hpp"
#include "gameplay/elements/shipsystems/specialmove.hpp"

// test
#include "generators/weapongen.hpp"
#include "gameplay/elements/blackhole.hpp"
#include "gameplay/elements/factorycomponent.hpp"

namespace Control
{
	using namespace ei;

	const float TACTICALCAM_DIST = 52.f;

	PlayerController::PlayerController(Game::Ship& _ship, GameStates::MainHud& _hud, GameTimeControl& _params)
		: Controller(_ship, _hud),
		m_mouseSensitivity(10.0f),
		m_sliderSensitivity(100.0f),
		m_targetSpeed(10.0f),
		m_targetingMode(TargetingMode::Normal),
		m_referenceGrid(*new Game::Grid(ei::Vec3(0.f),
			Utils::Color32F(0.f, 1.f, 0.f, 0.6f), 2.f, 2.f,
			50.f, Game::GridComponent::TransitionInfo(4800.f, 0.25f))),
		m_controlParams(_params)
	{
		s_sceneGraph->Add(m_referenceGrid);
	};

	const float FOCUS_RANGE = 650.f;

	void PlayerController::ProcessComponent(float _deltaTime, const Game::SceneGraph& _sceneGraph)
	{
		// restore actual time passed for correct camera movement speed
		_deltaTime /= m_controlParams.m_timeScale;
		// Apply the input to the model
		HandleInput(_deltaTime);

		auto actor = _sceneGraph.RayCast(g_camera.GetRay(Vec2(0.f, 0.f)), FOCUS_RANGE, Game::CollisionComponent::Type::Ship);
		if (actor && actor != &m_ship)
		{
			m_hud.UpdateCrossHair(1.f);
			m_focus = actor->GetHandle();
		}
		else
			m_hud.UpdateCrossHair(0.f);

		m_hud.ShowSpecialMoveMarker(m_ship.GetSpecialMove() && m_ship.GetSpecialMove()->GetState() != Game::SpecialMove::State::Charging);
		m_hud.UpdateIndicators(GetShip().GetPosition());

		if (m_targetingMode == TargetingMode::Tactical)
		{
			Vec3 pos = m_referenceGrid.GetPosition();

			Plane plane(normalize(m_ship.GetRotationMatrix() * Vec3(0.f, 1.f, 0.f)), pos);
			Ray ray = g_camera.GetRay(InputManager::GetCursorPosScreenSpace());
			float d = dot((pos - ray.origin), plane.n) / dot(ray.direction, plane.n);
			m_ship.GetSpecialMove()->SetIndicator(ray.origin + d * ray.direction);
			m_ship.GetSpecialMove()->TestPlacement(_sceneGraph);
		}
	}

	// ************************************************************ //
	void PlayerController::MouseMove(float _dx, float _dy)
	{
		Ray ray = g_camera.GetRay(InputManager::GetCursorPosScreenSpace());
		// transform to local space
		ray.origin = GetShip().GetInverseTransformation() * ray.origin;
		ray.direction = GetShip().GetInverseRotationMatrix() * ray.direction;

		// put origin outside of the sphere and invert direction
		// so that the intersection test finds the right point
		ray.origin += ray.direction * 100000.f;
		ray.direction *= -1.f;

		GetShip().AdjustWeaponOrientation( ray );
		m_mouseMovement = Vec2(_dx, _dy);
	}

	void PlayerController::KeyClick(int _key)
	{
		if (InputManager::IsVirtualKey(_key, VirtualKey::SWITCH_TACTICAL))
		{
			if (m_targetingMode == TargetingMode::Normal)
			{
				Game::SpecialMove* sm = m_ship.GetSpecialMove();
				if (sm && sm->GetState() == Game::SpecialMove::State::Ready)
				{
					sm->Target();
					SwitchTargetingMode(TargetingMode::Tactical);
				}
			}
			else
			{
				m_ship.GetSpecialMove()->Dismiss();
				SwitchTargetingMode(TargetingMode::Normal);
			}
		}
		if (InputManager::IsVirtualKey(_key, VirtualKey::FIRE) && m_targetingMode == TargetingMode::Tactical
			&& m_ship.GetSpecialMove()->GetState() == Game::SpecialMove::State::Active)
		{
			m_ship.GetSpecialMove()->Activate();
			SwitchTargetingMode(TargetingMode::Normal);
		}

		if (InputManager::IsVirtualKey(_key, VirtualKey::BRAKE))
			m_targetSpeed = 0.f;

		// test stuff
		if (_key == GLFW_KEY_R)
		{
		//	static uint32_t count = 2;
		//	++count;
			Generators::WeaponGenerator gen;
			Game::Weapon* w = gen.Generate(10.f, 2.f);
			Game::FactoryActor::GetThreadLocalInstance().Add(*w);
			m_ship.GetInventory().Add(*w);
		}
	}

	void PlayerController::KeyDown(int _key, int _modifiers)
	{
	}

	void PlayerController::Scroll(float _dx, float _dy)
	{
		if (m_targetingMode == TargetingMode::Tactical)
		{
			g_camera.Translate(Vec3(0.f, _dy * 4.f, 0.f));
			m_referenceGrid.Translate(Vec3(0.f, _dy * 4.f, 0.f));
		}
	}

	// ************************************************************ //
	void PlayerController::HandleInput(float _deltaTime)
	{
		if (m_targetingMode == TargetingMode::Tactical)
		{
			// todo move this to a config
			const float tacticalCamSpeed = 30.f;
			Vec3 camVel(0.f);
			// todo: change this to virtual keys
			if (InputManager::IsVirtualKeyPressed(Control::VirtualKey::ACC_FORWARD))
				camVel.z += tacticalCamSpeed;
			if (InputManager::IsVirtualKeyPressed(Control::VirtualKey::ACC_BACKWARD))
				camVel.z -= tacticalCamSpeed;
			if (InputManager::IsVirtualKeyPressed(Control::VirtualKey::ACC_LEFT))
				camVel.x -= tacticalCamSpeed * m_tacticalDirSign;
			if (InputManager::IsVirtualKeyPressed(Control::VirtualKey::ACC_RIGHT))
				camVel.x += tacticalCamSpeed * m_tacticalDirSign;
			g_camera.Translate(m_ship.GetRotationMatrix() * camVel * _deltaTime);
		//	m_referenceGrid.Translate(camVel * _deltaTime);
		}
		else
		{
			if (InputManager::IsVirtualKeyPressed(Control::VirtualKey::FIRE))
			{
				if(m_focus && *m_focus) GetShip().SetWeaponTarget(**m_focus);
				GetShip().Fire();
			}

			bool approximateTargetSpeed = true;
			if (InputManager::IsVirtualKeyPressed(Control::VirtualKey::ADJUST_TARGET_ACC))
			{
				// control target speed slider
				if (InputManager::IsVirtualKeyPressed(Control::VirtualKey::ACC_FORWARD))
					m_targetSpeed = ei::min(m_targetSpeed + m_sliderSensitivity * _deltaTime, GetShip().GetMaxSpeed());
				if (InputManager::IsVirtualKeyPressed(Control::VirtualKey::ACC_BACKWARD))
					m_targetSpeed = ei::max(m_targetSpeed - m_sliderSensitivity * _deltaTime, GetShip().GetMinSpeed());
			}
			else
			{
				// control ship speed manually
				if (InputManager::IsVirtualKeyPressed(Control::VirtualKey::ACC_FORWARD))
				{
					float newSpeed = GetShip().GetSpeed() + (GetShip().GetThrust() / GetShip().GetMass()) * _deltaTime;
					GetShip().SetSpeed(newSpeed);
					approximateTargetSpeed = false;
				}
				if (InputManager::IsVirtualKeyPressed(Control::VirtualKey::ACC_BACKWARD))
				{
					GetShip().SetSpeed(GetShip().GetSpeed() - (GetShip().GetThrust() / GetShip().GetMass()) * _deltaTime);
					approximateTargetSpeed = false;
				}
			}
			// approximate target speed with ship's (target) speed
			if (approximateTargetSpeed)
			{
				if (m_targetSpeed > GetShip().GetSpeed())
				{
					// accelerate ship's (target) speed
					GetShip().SetSpeed(min(GetShip().GetSpeed() + (GetShip().GetThrust() / GetShip().GetMass()) * _deltaTime, m_targetSpeed));
				}
				else if (m_targetSpeed < GetShip().GetSpeed())
				{
					// decelerate ship's (target) speed
					GetShip().SetSpeed(max(GetShip().GetSpeed() - (GetShip().GetThrust() / GetShip().GetMass()) * _deltaTime, m_targetSpeed));
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

		GetShip().SetTargetAngularVelocity(GetShip().GetRotationMatrix() * Vec3(-cursor[1], cursor[0], z));

		// update stats on the hud
		// this might not be the best place for this
		static int frameCount = 0;
		++frameCount; // prevent this label from flickering between two numbers
		if(frameCount % 4 == 0) m_hud.UpdateSpeedLabel(GetShip().GetCurrentSpeed());
		m_hud.UpdateSpeedBar(GetShip().GetCurrentSpeed(), GetShip().GetMaxSpeed());
		m_hud.UpdateTargetSpeedSlider(m_targetSpeed, GetShip().GetMaxSpeed());
		m_hud.GetEnergyBar().SetFillLevel(GetShip().GetEnergy() / GetShip().GetMaxEnergy());
		m_hud.GetHealthBar().SetFillLevel(GetShip().GetHealth() / GetShip().GetMaxHealth());
		m_hud.GetShieldBar().SetFillLevel(GetShip().GetShield() / GetShip().GetMaxShield());
		m_hud.UpdateObjectives();
	//	m_hud.UpdateCrossHair(m_ship->GetSprayRadius());
	}

	void PlayerController::SwitchTargetingMode(TargetingMode _newMode)
	{
		m_targetingMode = _newMode;
		if (_newMode == TargetingMode::Tactical)
		{
			const float angle = PI / 3.2f;

			m_tacticalDirSign = GetShip().GetPosition().y < g_camera.GetPosition().y ? 1.f : -1.f;
			Quaternion rot = m_tacticalDirSign < 0.f ? Quaternion(Vec3(0.f, 0.f, 1.f), PI) : qidentity();
			// shift the camera back so that the player is in the center
	//		g_camera.FixRotation(ei::Quaternion(Vec3(1.f, 0.f, 0.f), m_tacticalDirSign * angle) * rot,
	//			GetShip().GetPosition() + Vec3(0.f, m_tacticalDirSign * TACTICALCAM_DIST, -TACTICALCAM_DIST / tan(angle)));

			Vec3 camPos = m_ship.GetPosition() + m_ship.GetRotationMatrix() * Vec3(0.f, TACTICALCAM_DIST, -TACTICALCAM_DIST / tan(angle));

			g_camera.FixRotation(Quaternion(m_ship.GetRotationMatrix() * Vec3(0.f, 0.f, 1.f),
				normalize(m_ship.GetPosition() - camPos)) * m_ship.GetRotation(), camPos);

			m_referenceGrid.SetPosition(GetShip().GetPosition());
			m_referenceGrid.SetRotation(m_ship.GetRotation());
			component_cast<Game::GridComponent>(m_referenceGrid).ReverseTransition();
			m_controlParams.m_timeScale = 0.02f;
		}
		else
		{
			component_cast<Game::GridComponent>(m_referenceGrid).ReverseTransition();
			m_mouseMovement = Vec2(0.f);
			g_camera.Attach(GetShip());
			m_controlParams.m_timeScale = 1.f;
		}
	}
}