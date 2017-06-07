#include "game.hpp"
#include "utils/logger.hpp"

// CRT's memory leak detection
#if defined(DEBUG) || defined(_DEBUG)
#include <crtdbg.h>
#endif

int main()
{
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//	_CrtSetBreakAlloc(1020);
#endif

	Jo::Logger::g_logger.Initialize(new Jo::Logger::FilePolicy("run.log"));

	Wimp_s game;
	game.Run();

	return 0;
}