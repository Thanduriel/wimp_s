/*
MIT License

Copyright (c) 2008-2019 Netive Media Group & Vladimir Menshakov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "audiobackend.hpp"
#include <clunk/backend/sdl/sdl_ex.h>
#include <clunk/context.h>
#include <clunk/locker.h>
#include <SDL_rwops.h>
#include <SDL_audio.h>
#include <SDL.h>
#include <stdexcept>

#undef LOG_ERROR
#include "logger.hpp"

namespace clunk { namespace sdl {

AudioBackend::AudioBackend(int sample_rate, const u8 channels, int period_size) {
	LOG_LVL1(("Initializing AudioBackend (" + std::to_string(sample_rate) 
		+ ", " + std::to_string(channels) 
		+ ", " + std::to_string(period_size) + ")"));
	if (!SDL_WasInit(SDL_INIT_AUDIO)) {
		if (SDL_InitSubSystem(SDL_INIT_AUDIO) == -1)
			throw_sdl(("SDL_InitSubSystem"));
	}
	
	SDL_AudioSpec src;
	memset(&src, 0, sizeof(src));
	src.freq = sample_rate;
	src.channels = channels;
	src.format = AUDIO_S16SYS;
	src.samples = period_size;
	src.callback = &AudioBackend::callback;
	src.userdata = this;
	
	// !CHANGE! force conversion if the system does not use the targeted format
	if ( SDL_OpenAudio(&src, nullptr) < 0 )
		throw_sdl(("SDL_OpenAudio(%d, %u, %d)", sample_rate, channels, period_size));
//	if (_spec.format != AUDIO_S16SYS)
//		throw_ex(("SDL_OpenAudio(%d, %u, %d) returned format %d", sample_rate, channels, period_size, _spec.format));
//	if (_spec.channels < 2)
//		LOG_ERROR(("Could not operate on %d channels", _spec.channels));

	LOG_LVL2(("Opened audio device, sample rate: "+ std::to_string(src.freq) 
		+ ", channels: " + std::to_string(src.channels)
		+ ", period: " + std::to_string(src.samples)));
	_context.init(convert(src));
}
void AudioBackend::start()
{
	LOG_DEBUG(("starting output"));
	SDL_PauseAudio(0);
}

void AudioBackend::stop()
{
	LOG_DEBUG(("stopping output"));
	SDL_PauseAudio(0);
}

AudioBackend::~AudioBackend() {
	LOG_LVL2(("shutting down AudioBackend"));
	if (!SDL_WasInit(SDL_INIT_AUDIO))
		return;

	stop();

	SDL_CloseAudio();

	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void AudioBackend::callback(void *userdata, u8 *stream, int len) {
	AudioBackend *self = static_cast<AudioBackend *>(userdata);
	TRY {
		self->_context.process(stream, len);
	} CATCH("callback", {})
}


Sample* AudioBackend::load(const std::string &file) {
	Uint8 *buf;
	Uint32 len;
	SDL_AudioSpec spec;
	//SDL_AudioSpec * SDLCALL SDL_LoadWAV_RW(SDL_RWops *src, int freesrc, SDL_AudioSpec *spec, u8 **audio_buf, u32 *audio_len);
	if (SDL_LoadWAV(file.c_str(), &spec, &buf, &len) == NULL)
		throw_sdl(("SDL_LoadWav"));
	clunk::Buffer wav;
	wav.set_data(buf, len, true);
	Sample *sample = _context.create_sample();
	sample->init(wav, convert(spec));
	sample->name = file;
	return sample;
}

SDL_AudioSpec AudioBackend::convert(const AudioSpec &spec) {
	SDL_AudioSpec r = {};
	r.channels = spec.channels;
	r.freq = spec.sample_rate;
	switch(spec.format) {
		case AudioSpec::S8:		r.format = AUDIO_S8; break;
		case AudioSpec::U8:		r.format = AUDIO_U8; break;
		case AudioSpec::S16:	r.format = AUDIO_S16SYS; break;
		case AudioSpec::U16:	r.format = AUDIO_U16SYS; break;
		default: throw std::runtime_error("invalid audio format");
	}
	return r;
}

AudioSpec AudioBackend::convert(const SDL_AudioSpec &spec) {
	AudioSpec::Format format;
	switch(spec.format) {
		case AUDIO_S8:		format = AudioSpec::S8; break;
		case AUDIO_U8:		format = AudioSpec::U8; break;
		case AUDIO_S16SYS:	format = AudioSpec::S16; break;
		case AUDIO_U16SYS:	format = AudioSpec::U16; break;
		default: throw std::runtime_error("invalid sdl audio format");
	}
	return AudioSpec(format, spec.freq, spec.channels);
}

}}
