#pragma once
#include <memory>

#include "graphic/core/framebuffer.hpp"
#include "gamestates/gamestate.hpp"
#include "jofilelib.hpp"

class Wimp_s
{
public:
	Wimp_s();
	~Wimp_s();

	void Run();

	static float GetGameTime(){ return m_gameTime; }
private:
	void BuildDefaultConfig();

	Jo::Files::MetaFileWrapper m_config;

	std::vector < std::unique_ptr< GameStates::GameState > > m_gameStates;

	Graphic::Texture* m_sceneDepthTexture; ///< Main depth target for the 3D scene.
	Graphic::Texture* m_sceneColorTexture; ///< Main color target for the 3D scene.
	Graphic::Texture* m_sceneNormalTexture;
	Graphic::Framebuffer* m_sceneFramebuffer; ///< Framebuffer into which the entire 3D scene is rendered before it is passed to the postprocessing module.

	static float m_gameTime;
};