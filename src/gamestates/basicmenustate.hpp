#pragma once

#include "gamestate.hpp"
#include "gameplay/elements/grid.hpp"
#include "control/camera.hpp"

namespace GameStates {

	// background and camera handling for any menu states
	template<typename HudT>
	class BasicMenuState : public GameStateHT<HudT, void>
	{
	public:
		BasicMenuState(const Utils::Color32F& _color = Utils::Color32F(0.f, 1.f, 0.f, 0.5f))
			: m_grid(ei::Vec3(0.f, 0.f, 30.f), 
				_color, 3.5f, 3.5f, 80.f,
				Game::GridComponent::TransitionInfo(20000.f, 1.f, &Game::GridComponent::Random)),
			m_oldCamera(Control::g_camera)
		{
			m_grid.Rotate(ei::Quaternion(ei::normalize(ei::Vec3(1.f, 0.f, 0.f)), ei::PI * 0.5f));
			m_grid.Process(0.f);
		}

		~BasicMenuState()
		{
			Control::g_camera = m_oldCamera;
		}

		void Process(float _deltaTime) override
		{
			m_grid.ProcessComponent(_deltaTime);
		}

		void Draw(float _deltaTime) override
		{
			m_grid.Draw();
		}

		void OnActivate() override
		{
			using namespace Control;
			m_oldCamera = g_camera;

			g_camera.SetPosition(ei::Vec3(0.f, 0.f, -25.f));
			g_camera.SetRotation(ei::qidentity());
			g_camera.FixRotation(g_camera.GetRotation(), g_camera.GetPosition());
			g_camera.Process(0.f);
			m_grid.ReverseTransition();

			// updates the cursor position
			MouseMove(0.f, 0.f);
		}
	private:
		Game::Grid m_grid;
		Control::Camera m_oldCamera;
	};
}