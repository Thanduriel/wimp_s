#include "transitionstate.hpp"
#include "control/input.hpp"
#include "graphic/core/device.hpp"
#include "mainstate.hpp"

#include "graphic/resources.hpp"
#include "gameplay/content.hpp"
#include "gameplay/events/01_intro/act01.hpp"

namespace GameStates {

	struct LevelDescription {
		std::string title;
		std::string subtitle;
		std::string introduction;
	};

	const std::array< LevelDescription, (size_t)Game::Level::COUNT> LEVEL_DESCRIPTIONS =
	{ {
		{"Act I", "business as usual", ""},
		{ "Act II", "business as usual", "" },
		{ "Act III", "business as usual", "" },
		{ "Act IV", "business as usual", "" },
		{ "Act V", "business as usual", "" }
	} };

	using namespace Control;

	TransitionState::TransitionState(Game::Level _level, Game::Ship* _player)
		: m_level(_level),
		m_playerShip(_player),
		m_frameCount(0),
		m_isReady(false)
	{
		m_hud.m_titleLabel.SetText(LEVEL_DESCRIPTIONS[(int)_level].title);
		m_hud.CenterLabel(m_hud.m_titleLabel);
		m_hud.m_subtitleLabel.SetText(LEVEL_DESCRIPTIONS[(int)_level].subtitle);
		m_hud.CenterLabel(m_hud.m_subtitleLabel);
	}

	const float LOAD_STAGES = 12.f;
	void TransitionState::Process(float _deltaTime)
	{
		using namespace Graphic;
		using namespace Game;

		m_hud.m_loadBar.SetFillLevel(ei::min(1.f, m_frameCount / LOAD_STAGES));

		switch (m_frameCount)
		{
		case 0:
			m_hud.m_descriptionLabel.SetText("compiling shaders");
			Resources::GetEffect(Effects::MESH);
			Resources::GetEffect(Effects::SHIELD);
			break;
		case 1:
			Resources::GetEffect(Effects::DEFFERED_LIGHT);
			Resources::GetEffect(Effects::BACKGROUNDSTAR);
			break;
		case 2:
			Resources::GetEffect(Effects::BLACKHOLE);
			Resources::GetEffect(Effects::SUN);
			break;
		case 3:
			Resources::GetEffect(Effects::PARTICLE_BLOB);
			Resources::GetEffect(Effects::PARTICLE_TEXQAUD);
			break;
		case 4:
			m_hud.m_descriptionLabel.SetText("setting up uniform buffers");
			Resources::GetUBO(UniformBuffers::GLOBAL);
			Resources::GetUBO(UniformBuffers::CAMERA);
			Resources::GetUBO(UniformBuffers::SIMPLE_OBJECT);
			Resources::GetUBO(UniformBuffers::OBJECT_LINES);
			Resources::GetUBO(UniformBuffers::OBJECT_GEOMETRY);
			Resources::GetUBO(UniformBuffers::OBJECT_SHIELD);
			break;
		case 5:
			m_hud.m_descriptionLabel.SetText("loading textures");
			Resources::GetTexture("mist");
			Resources::GetTexture("shock");
			Resources::GetTexture("spacestationtex");
			Resources::GetTexture("stone01");
			break;
		case 6:
			Resources::GetTexture("cratetex");
			Resources::GetTexture("bolt");
			Resources::GetTexture("dronetex");
			Resources::GetTexture("dark_fighter_6_color");
			break;
		case 7:
			m_hud.m_descriptionLabel.SetText("loading meshes");
			Resources::GetMesh("asteroid");
			Resources::GetMesh("asteroid02");
			Resources::GetMesh("crate");
			Resources::GetMesh("missile");
			Resources::GetMesh("default_ship");
			Resources::GetMesh("spacestation");
			Resources::GetMesh("dark_fighter");
			break;
		case 8:
			Content::GetBoundingMesh("asteroidbm");
			Content::GetBoundingMesh("asteroid02bm");
			Content::GetBoundingMesh("default_shipbm");
			Content::GetBoundingMesh("spacestationbm");
			break;
		case 9:
			m_hud.m_descriptionLabel.SetText("creating stars");
			if (!m_playerShip) m_playerShip = &MainState::CreatePlayerShip();
			m_createdState = new MainState(*m_playerShip);
			break;
		case 10:
		{
			m_hud.m_descriptionLabel.SetText("setting up map");
			Map& map = CreateLevel(m_level);
			m_createdState->SetMap(map);
			break;
		}
		case 11:
			break;
		case 12:
			m_hud.m_descriptionLabel.SetVisible(false);
			m_isReady = true;
			m_hud.m_finishLabel.SetVisible(true);
			break;
		};

		++m_frameCount;
	}

	void TransitionState::Draw(float _deltaTime)
	{
		Graphic::Device::DrawFramebufferToBackbuffer();

		m_hud.Draw(_deltaTime);
	}

	// ******************************************************* //
	void TransitionState::MouseMove(float _dx, float _dy)
	{
		m_hud.MouseMove(_dx, _dy);
	}
	void TransitionState::Scroll(float _dx, float _dy)
	{
		if (m_hud.Scroll(_dx, _dy)) return;
	}
	void TransitionState::KeyDown(int _key, int _modifiers)
	{
		if (m_hud.KeyDown(_key, _modifiers)) return;
	}
	void TransitionState::KeyRelease(int _key)
	{
		if (m_hud.KeyUp(_key, 0)) return;
	}
	void TransitionState::KeyClick(int _key)
	{
		// make sure that only one key event is registered
		if (m_isReady && !m_newState)
		{
			m_isFinished = true;
			m_newState = m_createdState;
		}
	}
	void TransitionState::KeyDoubleClick(int _key)
	{

	}

	using namespace Game;
	Map& TransitionState::CreateLevel(Level _level)
	{
		Map* map;
		switch (_level)
		{
		case Level::Act01: map = new Acts::Act01(m_createdState->GetSceneGraph(), *m_playerShip, m_createdState->GetHud());
			break;
		default:
			Assert(false, "This level is unknown.");
			break;
		}

		return *map;
	}
}