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
	{
		using namespace Graphic;
		m_hud.GetStartButton().SetOnMouseUp([&]() { m_newState = new MainState(); });
	}

	MainMenuState::~MainMenuState()
	{
	}

	void MainMenuState::Process(float _deltaTime)
	{
		//if (InputManager::IsVirtualKeyPressed(Control::VirtualKey::INVENTORY))
		//{
		//	// Change back to main state
		//	m_newState = new GameStates::MainState();
		//}
	}

	void MainMenuState::Draw(float _deltaTime)
	{
		Texture& tex = *Device::GetCurrentFramebufferBinding()->GetColorAttachments().begin()->pTexture;
		Device::BindFramebuffer(nullptr);
		Device::SetEffect(Resources::GetEffect(Effects::SCREEN_OUTPUT));
		Device::SetTexture(tex, 0);
		Device::DrawFullscreen();
		m_hud.Draw(_deltaTime);
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