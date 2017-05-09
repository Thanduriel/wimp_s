#include "game.hpp"
#include "utils/logger.hpp"

int main()
{
	Jo::Logger::g_logger.Initialize(new Jo::Logger::FilePolicy("run.log"));

	Game game;
	game.Run();

	return 0;
}