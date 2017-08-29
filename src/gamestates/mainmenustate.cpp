#include "mainmenustate.hpp"
#include "mainstate.hpp"
#include "huds/mainmenuhud.hpp"
#include "graphic/interface/hud.hpp"
#include "graphic/interface/pixelcoords.hpp"
#include "graphic/interface/hudelements.hpp"
#include "control/input.hpp"

namespace GameStates
{
	using namespace Graphic;
	using namespace Control;

	MainMenuState::MainMenuState()
		: m_grid(ei::Vec3(0.f,0.f, 30.f), Utils::Color32F(0.f,1.f,0.f, 0.5f), 3.5f, 3.5f, 80.f),
		m_blackHole(ei::Vec3(1.f), 5.f)
	{
		m_grid.Rotate(ei::Quaternion(ei::normalize(ei::Vec3(1.f, 0.f, 0.f)), ei::PI * 0.5f));
		m_grid.Process(0.f);
		m_blackHole.Process(0.f);

		using namespace Graphic;
		m_hud.m_startButton->SetOnMouseUp([&]() { m_newState = new MainState(); });
		m_hud.m_exitButton->SetOnMouseUp([&]() { m_isFinished = true; });
	}

	MainMenuState::~MainMenuState()
	{
	}

	void MainMenuState::Process(float _deltaTime)
	{
		static float sum = 0.f;
		sum += _deltaTime;
		m_blackHole.SetPosition(7.6f * Vec3(cos(sum), sin(sum), 0.f));
	}

	void MainMenuState::Draw(float _deltaTime)
	{
		m_grid.Draw();
		m_hud.Draw(_deltaTime);

		Texture& tex = *Device::GetCurrentFramebufferBinding()->GetColorAttachments().begin()->pTexture;
		Device::BindFramebuffer(nullptr);
		Device::SetEffect(Resources::GetEffect(Effects::SCREEN_OUTPUT));
		Device::SetTexture(tex, 0);
		Device::DrawFullscreen();

		m_blackHole.Draw();
	}

	void MainMenuState::OnActivate()
	{
		using namespace Control;
		g_camera.SetPosition(ei::Vec3(0.f, 0.f, -25.f));
		g_camera.SetRotation(ei::qidentity());
		g_camera.FixRotation(g_camera.GetRotation(), g_camera.GetPosition());
		g_camera.Process(0.f);

		// updates the cursor position
		MouseMove(0.f, 0.f);
	}

	void MainMenuState::Dispose()
	{
	}

	// ******************************************************* //
	void MainMenuState::MouseMove(float _dx, float _dy)
	{
		m_hud.MouseMove(_dx, _dy);
	}
	void MainMenuState::Scroll(float _dx, float _dy)
	{
		if (m_hud.Scroll(_dx, _dy)) return;
	}
	void MainMenuState::KeyDown(int _key, int _modifiers)
	{
		if (m_hud.KeyDown(_key, _modifiers)) return;
	}
	void MainMenuState::KeyRelease(int _key)
	{
		if (m_hud.KeyUp(_key, 0)) return;
	}
	void MainMenuState::KeyClick(int _key)
	{

	}
	void MainMenuState::KeyDoubleClick(int _key)
	{

	}
}