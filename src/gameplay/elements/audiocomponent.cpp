#include "audiocomponent.hpp"
#include "utils/assert.hpp"
#include "control/camera.hpp"

#include "utils/audiobackend.hpp"
#include <clunk/context.h>
#include <clunk/source.h>
#include <clunk/wav_file.h>

namespace Game {

	clunk::sdl::AudioBackend* AudioSystem::s_backend;
	AudioComponent* AudioSystem::s_audioComponent;
	float AudioSystem::s_masterVolume;
	float AudioSystem::s_fallOffFactor;

	void AudioSystem::Initialize(float _volume)
	{
		static clunk::sdl::AudioBackend backend(44100, 2, 1024);
		s_backend = &backend;

		clunk::Context &context = backend.get_context();
		clunk::DistanceModel dm(clunk::DistanceModel::Exponent, false);
		dm.rolloff_factor = 0.55f;
		dm.same_sounds_limit = 4;
		dm.doppler_factor = 0.f;
		dm.speed_of_sound = 1000.f;

		context.set_distance_model(dm);
		context.get_listener()->update_view(clunk::v3f(0.f, 0.f, 1.f), clunk::v3f(0.f,1.f, 0.f));
		context.set_max_sources(16);

		SetVolume(_volume);
		context.set_fx_volume(_volume);

		backend.start();

		s_audioComponent = new AudioComponent(Control::g_camera);
	}

	void AudioSystem::Close()
	{
		delete s_audioComponent;
		s_backend->stop();
	}

	clunk::Context& AudioSystem::GetContext()
	{
		Assert(s_backend, "The audio system needs to be initialized first.");
		return s_backend->get_context();
	}

	void AudioSystem::SetVolume(float _volume)
	{
		s_masterVolume = _volume;
		s_backend->get_context().set_fx_volume(s_masterVolume);
	}

	void AudioSystem::Mute3DSounds()
	{
		s_fallOffFactor = GetContext().get_distance_model().rolloff_factor;
		GetContext().get_distance_model().rolloff_factor = 100.f;
	}

	void AudioSystem::Enable3DSounds()
	{
		GetContext().get_distance_model().rolloff_factor = s_fallOffFactor;
	}

	// ****************************************************** //
	AudioComponent::AudioComponent(const Actor& _actor, const ei::Vec3& _position)
		: ConstActorComponent(_actor),
		Transformation(_position),
		m_object(AudioSystem::GetContext().create_object()),
		m_curId(0),
		m_isMute(false)
	{
	}

	AudioComponent::~AudioComponent()
	{
		m_object->cancel_all(true,0.3f);
		delete m_object;
		//m_object->autodelete();
	}

	clunk::Source* AudioComponent::Play(const clunk::Sample& _sound, int _id, bool _loop, const ei::Vec3& _position)
	{
		if (m_isMute) return nullptr;

		clunk::Source* source = new clunk::Source(&_sound, _loop);
		source->delta_position = clunk::v3f(_position.x, _position.y, _position.z);
		UpdateSourcePos(); // make sure that the position is already correct when the first sample is heard
		if (_id == -1)
		{
			m_curId = m_curId + 1 % 16;
			m_object->play(m_curId, source);
		}
		else m_object->play(_id, source);

		if(_loop) m_source = source;
		return source;
	}

	clunk::Object* AudioComponent::GetObject()
	{
		return m_object;
	}

	void AudioComponent::Stop(float _fadeOut)
	{
		m_object->cancel_all(true, _fadeOut);
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