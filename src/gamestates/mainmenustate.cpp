#include "mainmenustate.hpp"
#include "transitionstate.hpp"
#include "settingsstate.hpp"
#include "huds/mainmenuhud.hpp"
#include "graphic/interface/hud.hpp"
#include "graphic/interface/pixelcoords.hpp"
#include "graphic/interface/hudelements.hpp"
#include "control/input.hpp"

#include "gameplay/elements/audiocomponent.hpp"
#include "gameplay/content.hpp"
namespace GameStates
{
	using namespace Graphic;
	using namespace Control;
	using namespace ei;

	MainMenuState::MainMenuState()
		: m_blackHole(ei::Vec3(1.f), 5.f)
	{
		m_blackHole.Process(0.f);

		using namespace Graphic;
	
		m_hud.m_startButton->SetOnMouseUp([&]() { m_newState = new TransitionState(Game::Level::Act01, nullptr); });
		m_hud.m_optionsButton->SetOnMouseUp([&]() {m_newState = new SettingsState();});
		m_hud.m_exitButton->SetOnMouseUp([&]() { m_isFinished = true; });
	}

	MainMenuState::~MainMenuState()
	{
	}

	void MainMenuState::Process(float _deltaTime)
	{
		BasicMenuState<MainMenuHud>::Process(_deltaTime);
		static float sum = 0.f;
		sum += _deltaTime * 0.4f;
		Quaternion rot(normalize(Vec3(0.2f, 0.f, 1.f)), sum);
		m_blackHole.SetPosition(transform(Vec3(13.f,0.f,0.f), rot));
	}

	void MainMenuState::Draw(float _deltaTime)
	{
		BasicMenuState<MainMenuHud>::Draw(_deltaTime);
		m_hud.Draw(_deltaTime);

		Device::DrawFramebufferToBackbuffer();

		m_blackHole.Draw();
	}

	void MainMenuState::OnActivate()
	{
		BasicMenuState<MainMenuHud>::OnActivate();
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