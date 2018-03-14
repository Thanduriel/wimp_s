#include "customelements.hpp"

#include "gameplay/elements/audiocomponent.hpp"
#include "clunk/clunk.h"
#include "gameplay/content.hpp"

namespace Graphic {

	void MenuButton::MouseEnter()
	{
		static const clunk::Sample& sound = Game::Content::GetSound("button02");
		Game::AudioSystem::GetGlobalAudio().Play(sound);
		Button::MouseEnter();
	}

	bool MenuButton::KeyUp(int _key, int _modifiers, ei::Vec2 _pos)
	{
		static const clunk::Sample& sound = Game::Content::GetSound("button01");
		Game::AudioSystem::GetGlobalAudio().Play(sound);
		return Button::KeyUp(_key, _modifiers, _pos);
	}
}