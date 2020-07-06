#include <chrono>
#include <GLFW/glfw3.h>
#include "game.hpp"
#include "graphic/core/device.hpp"
#include "gamestates/mainstate.hpp"
#include "gamestates/inventorystate.hpp"
#include "gamestates/mainmenustate.hpp"
#include "graphic/core/texture.hpp"
#include "graphic/resources.hpp"
#include "control/input.hpp"
#include "control/playercontroller.hpp"
#include "graphic/effects/lightsystem.hpp"
#include "graphic/effects/particlesystem.hpp"
#include "gameplay/content.hpp"
#include "graphic/core/uniformbuffer.hpp"
#include "gameplay/elements/audiocomponent.hpp"
#include <thread>

#include "utils/loggerinit.hpp"

using namespace Graphic;
using namespace std::string_literals;

float Wimp_s::m_gameTime;
int Wimp_s::m_targetFrameTime;
Wimp_s* Wimp_s::m_instance;

Wimp_s::Wimp_s()
{
	m_instance = this;
	// Load configuration
	try {
		Jo::Files::HDDFile file("config.json");
		m_config.Read(file, Jo::Files::Format::JSON);
	}
	catch (std::string _message) {
		LOG_ERROR("Failed to load config file with message:\n" + _message + '\n' + "\nLoading default configuration.");
		BuildDefaultConfig();

		try {
			Jo::Files::HDDFile file("config.json", Jo::Files::HDDFile::OVERWRITE);
			m_config.Write(file, Jo::Files::Format::JSON);
		}
		catch (std::string _message) {
			LOG_ERROR("Failed to write config file with message:\n" + _message + '\n' + "\n");
		}
	}

	// create context
	auto& cgraphic = m_config["Graphics"];
	const int screenWidth = cgraphic["ScreenWidth"s].Get(1366);
	const int screenHeight = cgraphic["ScreenHeight"s].Get(768);
	Graphic::Device::Initialize(screenWidth, screenHeight, cgraphic["FullScreen"s].Get(false));

	m_sceneColorTexture = new Texture(Graphic::Device::GetBackbufferSize()[0], Device::GetBackbufferSize()[1],
		Texture::Format(4, 8, Texture::Format::ChannelType::UINT));
	m_sceneNormalTexture = new Texture(Graphic::Device::GetBackbufferSize()[0], Device::GetBackbufferSize()[1],
		Texture::Format(4, 32, Texture::Format::ChannelType::FLOAT));
	m_sceneDepthTexture = new Texture(Graphic::Device::GetBackbufferSize()[0], Device::GetBackbufferSize()[1],
		Texture::Format(1, 32, Texture::Format::ChannelType::FLOAT, Texture::Format::FormatType::DEPTH));
	m_sceneFramebuffer = new Framebuffer({ Framebuffer::Attachment(m_sceneColorTexture), Framebuffer::Attachment(m_sceneNormalTexture)},
		Framebuffer::Attachment(m_sceneDepthTexture));

	Control::InputManager::Initialize(Graphic::Device::GetWindow(), m_config[std::string("Input")]);//config[std::string("Input")]

	LightSystem::Initialize();
	auto& csound = m_config["Sound"];
	Game::AudioSystem::Initialize(csound["MasterVolume"s].Get(1.f));

	// load other properties
	auto& cgame = m_config["Game"];
	Control::PlayerController::HAS_AIM_ASSIST = cgame["AimAssist"s];
	Control::PlayerController::ENABLE_CHEASTS = cgame["Cheats"s];
	SetTargetFPS(cgraphic["TargetFPS"s].Get(144.f));

	m_gameStates.emplace_back(new GameStates::MainMenuState());
	m_gameStates.back()->OnActivate();

	UniformBuffer& ubo = Resources::GetUBO(UniformBuffers::BRIGHTNESS_PARAMS);
	ubo["Brightness"] = cgraphic["Brightness"s].Get(0.f);
	ubo["Contrast"] = cgraphic["Contrast"s].Get(1.f);
}

Wimp_s::~Wimp_s()
{
	Control::InputManager::Close();
	LightSystem::Close();
	ParticleSystems::Manager::Close();
	Game::AudioSystem::Close();

	Game::Content::Unload();

	delete m_sceneColorTexture;
	delete m_sceneNormalTexture;
	delete m_sceneDepthTexture;
	delete m_sceneFramebuffer;
	Graphic::Resources::Unload();

	Device::Exit();
}

void Wimp_s::Run()
{
	using namespace GameStates;
	using namespace std::chrono;

	steady_clock::time_point begin = steady_clock::now();

	while (m_gameStates.size())
	{
		// measure delta time
		steady_clock::time_point end = steady_clock::now();
		duration<float> d = duration_cast<duration<float>>(end - begin);
		begin = end;
		m_gameTime += d.count();

		GameState& current = *m_gameStates.back();

		current.Process(d.count());

		// rendering
		Graphic::Device::BindFramebuffer( m_sceneFramebuffer );
		Graphic::Device::Clear(0.f, 0.f, 0.f);
		UniformBuffer& ubo = Resources::GetUBO(UniformBuffers::GLOBAL);
		ubo["Time"] = m_gameTime;
		current.Draw(d.count());

		// Present
		glfwSwapBuffers(Graphic::Device::GetWindow());

		// delete all marked actors in the scene
		current.Dispose();

		// state managment with a stack
		bool changedState = false;
		GameState* newState = current.FetchNewState();
		// check older states to prevent another frame of them being rendered
		while (m_gameStates.size() && m_gameStates.back()->IsFinished()) { 
			m_gameStates.pop_back(); changedState = true; 
		}
		if (newState) {
			m_gameStates.emplace_back(newState); changedState = true;
		}
		if (changedState && m_gameStates.size()) m_gameStates.back()->OnActivate();

		if (m_gameStates.size())
		{
			Control::InputManager::SetGameState(m_gameStates.back().get());
			glfwPollEvents();
		}
		if (glfwWindowShouldClose(Graphic::Device::GetWindow())) m_gameStates.clear();

		// measure time required for this frame
		end = steady_clock::now();
		d = duration_cast<duration<float>>(end - begin);
		if (d.count() < m_targetFrameTime) std::this_thread::sleep_for(milliseconds(m_targetFrameTime) - d);
	}
}

void Wimp_s::SaveConfig()
{
	Control::InputManager::Save(m_config[std::string("Input")]);
	m_config["Game"s]["AimAssist"s] = Control::PlayerController::HAS_AIM_ASSIST;
	m_config["Sound"s]["MasterVolume"s] = static_cast<double>(Game::AudioSystem::GetVolume());

	try {
		Jo::Files::HDDFile file("config.json", Jo::Files::HDDFile::OVERWRITE);
		m_config.Write(file, Jo::Files::Format::JSON);
	}
	catch (std::string _message) {
		LOG_ERROR("Failed to save config file with message:\n" + _message);
	}
}

void Wimp_s::SetTargetFPS(float _fps)
{
	m_targetFrameTime = int(1000.f / _fps) - 1;
}

void Wimp_s::BuildDefaultConfig()
{
	auto& cinput = m_config[std::string("Input")];
	cinput[std::string("AccForward")][0] = GLFW_KEY_W;
	cinput[std::string("AccBackward")][0] = GLFW_KEY_S;
	cinput[std::string("AccLeft")][0] = GLFW_KEY_A;
	cinput[std::string("AccRight")][0] = GLFW_KEY_D;
	cinput[std::string("AdjustTargetAcc")][0] = GLFW_KEY_LEFT_SHIFT;
	cinput[std::string("Brake")][0] = GLFW_KEY_TAB;
	cinput[std::string("Fire")][0] = GLFW_MOUSE_BUTTON_LEFT;
	cinput[std::string("FireOther")][0] = GLFW_MOUSE_BUTTON_RIGHT;
	cinput[std::string("RollCW")][0] = GLFW_KEY_E;
	cinput[std::string("RollCCW")][0] = GLFW_KEY_Q;
	cinput[std::string("Inventory")][0] = GLFW_KEY_I;
	cinput[std::string("SwitchTactical")][0] = GLFW_KEY_SPACE;
	cinput[std::string("Pause")][0] = GLFW_KEY_P;
	cinput[std::string("Pause")][1] = GLFW_KEY_ESCAPE;
	cinput[std::string("LockTarget")][0] = GLFW_KEY_R;
	cinput[std::string("CycleTarget")][0] = GLFW_KEY_T;

	cinput["MouseSensitivity"s] = 1.0;

	auto& cgraphics = m_config[std::string("Graphics")];
	cgraphics[std::string("ScreenWidth")] = 1366;
	cgraphics[std::string("ScreenHeight")] = 768;
	cgraphics[std::string("FullScreen")] = false;
	cgraphics["TargetFPS"s] = 144.0;
	cgraphics["Brightness"s] = 0.0;
	cgraphics["Contrast"s] = 1.0;

	auto& cgame = m_config[std::string("Game")];
	cgame["AimAssist"s] = false;
	cgame["Cheats"s] = false;

	auto& csound = m_config["Sound"];
	csound["MasterVolume"s] = 1.0;
}