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

	AudioComponent::AudioComponent(const Actor& _actor, const clunk::Sample& _sound)
		: ConstActorComponent(_actor),
		Transformation(ei::Vec3(0.f)),
		m_sound(_sound),
		m_object(AudioSystem::GetContext().create_object())
	{

	}

	AudioComponent::~AudioComponent()
	{
		delete m_object;
	}

	void AudioComponent::Play()
	{
		clunk::Source* source = new clunk::Source(&m_sound, false);
		m_object->play(0, source);
	}

	void AudioComponent::Stop()
	{
		m_object->cancel_all();
	}

	void AudioComponent::ProcessComponent(float _deltaTime)
	{
		ei::Vec3 pos = ei::Vec3(Control::g_camera.GetView() * Transformation::Get(m_actor.GetTransformation()) * ei::Vec4(0.f, 0.f, 0.f, 1.f));
		// the listener seems to be ignored so transform the system from z-forward, y-up to y-forward and z-up
		m_object->set_position(clunk::v3f(-pos.x, pos.z, pos.y));
	}
}