#include "audiocomponent.hpp"
#include "utils/assert.hpp"
#include "control/camera.hpp"

#include <clunk/context.h>
#include <clunk/backend/sdl/backend.h>
#include <clunk/source.h>
#include <clunk/wav_file.h>

namespace Game {

	clunk::sdl::Backend* AudioSystem::s_backend;

	void AudioSystem::Initialize()
	{
		static clunk::sdl::Backend backend(44100, 2, 1024);
		s_backend = &backend;

		clunk::Context &context = backend.get_context();
		clunk::DistanceModel dm(clunk::DistanceModel::Exponent, false);
		dm.rolloff_factor = 0.7f;
		context.set_distance_model(dm);
		context.get_listener()->update_view(clunk::v3f(0.f, 0.f, 1.f), clunk::v3f(0.f,1.f, 0.f));
		context.set_max_sources(16);

		backend.start();
	}

	void AudioSystem::Close()
	{
		s_backend->stop();
	}

	clunk::Context& AudioSystem::GetContext()
	{
		Assert(s_backend, "The audio system needs to be initialized first.");
		return s_backend->get_context();
	}

	AudioComponent::AudioComponent(const Actor& _actor, const ei::Vec3& _position)
		: ConstActorComponent(_actor),
		Transformation(_position),
		m_object(AudioSystem::GetContext().create_object()),
		m_curId(0)
	{

	}

	AudioComponent::~AudioComponent()
	{
		m_object->cancel_all(true);
		delete m_object;
	}

	void AudioComponent::Play(const clunk::Sample& _sound, int _id, bool _loop)
	{
		clunk::Source* source = new clunk::Source(&_sound, _loop);
		UpdateSourcePos(); // make sure that the position is already correct when the first sample is heard
		m_curId = m_curId + 1 % 16;
		m_object->play(m_curId, source);
	}

	clunk::Object* AudioComponent::GetObject()
	{
		return m_object;
	}

	void AudioComponent::Stop()
	{
		m_object->cancel_all();
	}

	void AudioComponent::ProcessComponent(float _deltaTime)
	{
		UpdateSourcePos();
	}

	void AudioComponent::UpdateSourcePos()
	{
		ei::Vec3 pos = ei::Vec3(Control::g_camera.GetView() * Transformation::Get(m_actor.GetTransformation()) * ei::Vec4(0.f, 0.f, 0.f, 1.f));
		// the listener seems to be ignored so transform the system from z-forward, y-up to y-forward and z-up
		m_object->set_position(clunk::v3f(-pos.x, pos.z, pos.y));
	}
}