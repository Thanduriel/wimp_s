#include <chrono>
#include <GLFW/glfw3.h>
#include "game.hpp"
#include "graphic/core/device.hpp"
#include "gamestates/mainstate.hpp"
#include "graphic/core/texture.hpp"
#include "graphic/resources.hpp"

#include "utils/loggerinit.hpp"

using namespace Graphic;

Wimp_s::Wimp_s()
{
	// create context
	const int screenWidth = 1366;
	const int screenHeight = 768;
	Graphic::Device::Initialize(screenWidth, screenHeight, false);

	m_sceneColorTexture = new Texture(Graphic::Device::GetBackbufferSize()[0], Device::GetBackbufferSize()[1],
		Texture::Format(4, 8, Texture::Format::ChannelType::UINT));
	m_sceneDepthTexture = new Texture(Graphic::Device::GetBackbufferSize()[0], Device::GetBackbufferSize()[1],
		Texture::Format(1, 32, Texture::Format::ChannelType::FLOAT, Texture::Format::FormatType::DEPTH));
	m_sceneFramebuffer = new Framebuffer(Framebuffer::Attachment(m_sceneColorTexture), 
		Framebuffer::Attachment(m_sceneDepthTexture));

	m_gameStates.emplace_back(new GameStates::MainState());
}

Wimp_s::~Wimp_s()
{
	Graphic::Resources::Unload();
}

void Wimp_s::Run()
{
	using namespace GameStates;
	using namespace std::chrono;

	steady_clock::time_point begin = steady_clock::now();

	while (m_gameStates.size())
	{
		// messure delta time
		steady_clock::time_point end = steady_clock::now();
		duration<float> d = duration_cast<duration<float>>(end - begin);
		begin = end;

		GameState& current = *m_gameStates.back();

		current.Process(d.count());

		// rendering
		// use hardware backbuffer for now
		Graphic::Device::BindFramebuffer( nullptr );
		Graphic::Device::Clear(0.f, 0.f, 0.f);
		current.Draw(d.count());

		// Present if not closed
		glfwSwapBuffers(Graphic::Device::GetWindow());

		// state managment with a stack
		GameState* newState = current.FetchNewState();
		if (current.IsFinished()) m_gameStates.pop_back();
		if (newState) m_gameStates.emplace_back(newState);

		// todo: move this when general input handling is implemented
		if (m_gameStates.size() && glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_ESCAPE)) 
			m_gameStates.pop_back();

		glfwPollEvents();
		if (glfwWindowShouldClose(Graphic::Device::GetWindow())) m_gameStates.clear();
	}
}