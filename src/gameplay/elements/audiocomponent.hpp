#pragma once

#include "gameplay/core/component.hpp"
#include "math/transformation.hpp"

namespace clunk {
	namespace sdl { class Backend; }

	class Object;
	class Sample;
	class Context;
}


namespace Game {

	class AudioSystem
	{
	public:
		static void Initialize();
		static void Close();

		static clunk::Context& GetContext();
	private:

		static clunk::sdl::Backend* s_backend;

		friend class AudioComponent;
		friend class Wimp_s;
		friend class Content;
	};

	class AudioComponent : public ConstActorComponent, public Transformation
	{
	public:
		AudioComponent(const Actor& _actor, const clunk::Sample& _sound);
		~AudioComponent();

		void ProcessComponent(float _deltaTime) override;

		void Play();
		void Stop();
	private:
		clunk::Object* m_object;
		const clunk::Sample& m_sound;
	};
}