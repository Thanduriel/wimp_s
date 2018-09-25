#pragma once

#include "gameplay/core/component.hpp"
#include "math/transformation.hpp"

namespace clunk {
	namespace sdl { class Backend; }

	class Object;
	class Sample;
	class Context;
	class Source;
}


namespace Game {

	class AudioComponent;

	class AudioSystem
	{
	public:
		static void Initialize(float _volume);
		static void Close();

		static clunk::Context& GetContext();
		static AudioComponent& GetGlobalAudio() { return *s_audioComponent; }

		static void SetVolume(float _volume);
		static float GetVolume() { return s_masterVolume; }
	private:

		static clunk::sdl::Backend* s_backend;
		static AudioComponent* s_audioComponent;
		static float s_masterVolume;

		friend class AudioComponent;
		friend class Wimp_s;
		friend class Content;
	};

	class AudioComponent : public ConstActorComponent, public Transformation
	{
	public:
		AudioComponent(const Actor& _actor, const ei::Vec3& _position = ei::Vec3(0.f));
		~AudioComponent();

		void ProcessComponent(float _deltaTime) override;

		clunk::Source* Play(const clunk::Sample& _sound, int _id = -1, bool _loop = false, const ei::Vec3& _position = {});
		clunk::Object* GetObject();
		clunk::Source* GetSource(int _id) { return m_source; } // todo allow different sources

		void Stop(float _fadeOut = 0.1f);
	private:
		void UpdateSourcePos();
		clunk::Object* m_object;
		clunk::Source* m_source;
		int m_curId;
	};
}