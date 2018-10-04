#pragma once

#include "audiocomponent.hpp"
#include "lifetimecomponent.hpp"
#include "gameplay/core/actor.hpp"
#include "clunk/sample.h"

namespace Game {

	class SingleSoundActor : public Actor
	{
	public:
		SingleSoundActor(const ei::Vec3& _position, const clunk::Sample& _sound, int _id = -1, bool _loop = false)
			: Actor(_position),
			m_audio(THISACTOR),
			m_lifeTime(THISACTOR, _sound.length())
		{
			m_audio.Play(_sound, _id, _loop);
		}

		void RegisterComponents(class SceneGraph& _sceneGraph) override
		{
			_sceneGraph.RegisterComponent(m_audio);
			_sceneGraph.RegisterComponent(m_lifeTime);
		}
	private:
		AudioComponent m_audio;
		LifeTimeComponent m_lifeTime;
	};
}