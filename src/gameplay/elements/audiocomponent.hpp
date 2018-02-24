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
		AudioComponent(const Actor& _actor, const ei::Vec3& _position = ei::Vec3(0.f));
		~AudioComponent();

		void ProcessComponent(float _deltaTime) override;

		void Play(const clunk::Sample& _sound, int _id = 0, bool _loop = false);
		clunk::Object* GetObject();

		void Stop();
	private:
		void UpdateSourcePos();
		clunk::Object* m_object;
		int m_curId;
	};
}