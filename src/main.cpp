#include "game.hpp"
#include "utils/logger.hpp"

#include <clunk/context.h>
#include <clunk/backend/sdl/backend.h>
#include <clunk/source.h>
#include <clunk/wav_file.h>

// CRT's memory leak detection
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

// sdl has a main macro
#undef main

int main()
{
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//	_CrtSetBreakAlloc(541);
#endif

	Jo::Logger::g_logger.Initialize(new Jo::Logger::FilePolicy("run.log"));

	Wimp_s game;
	game.Run();
	
	game.SaveConfig();
	return 0;
}