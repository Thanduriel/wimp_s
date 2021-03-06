#pragma once

#include <memory>

#include "graphic/core/framebuffer.hpp"
#include <jofilelib.hpp>
#include <memory>

namespace GameStates{
	class GameState;
}

class Wimp_s
{
public:
	Wimp_s();
	~Wimp_s();

	void Run();

	void SaveConfig();

	static float GetGameTime(){ return m_gameTime; }
	static void SetTargetFPS(float _fps);
	// only call this from the main thread
	static Jo::Files::MetaFileWrapper& GetConfig() { return m_instance->m_config; }
private:
	void BuildDefaultConfig();

	Jo::Files::MetaFileWrapper m_config;

	std::vector < std::unique_ptr< GameStates::GameState > > m_gameStates;

	Graphic::Texture* m_sceneDepthTexture;
	Graphic::Texture* m_sceneColorTexture; ///< Main color target for the 3D scene.
	Graphic::Texture* m_sceneNormalTexture;
	Graphic::Texture* m_sceneLightStencilTexture;
	Graphic::Framebuffer* m_sceneFramebuffer; ///< Framebuffer into which the entire 3D scene is rendered before it is passed to the postprocessing module.

	static float m_gameTime;
	static int m_targetFrameTime;
	static Wimp_s* m_instance;
};