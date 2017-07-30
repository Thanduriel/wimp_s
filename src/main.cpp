#include "game.hpp"
#include "utils/logger.hpp"

// CRT's memory leak detection
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

int main()
{
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//	_CrtSetBreakAlloc(189149);
#endif

	Jo::Logger::g_logger.Initialize(new Jo::Logger::FilePolicy("run.log"));

	Wimp_s game;
	game.Run();

	return 0;
}