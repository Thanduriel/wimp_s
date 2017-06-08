#include "game.hpp"
#include "utils/logger.hpp"

// CRT's memory leak detection
#if defined(DEBUG) || defined(_DEBUG)
#include <crtdbg.h>
#endif

template<typename T>
struct RefCount
{
	size_t count;
	T* ptr;
};

template<typename T, typename...Args>
std::shared_ptr<T> make_shared(Args&&... _args)
{
	auto rawPtr = malloc(sizeof(T) + sizeof(RefCount));
	auto objP = new (ptr + sizeof(RefCount)) T(std::forward<Args>(_args)...);
	RefCount<T>& refCount = *reinterpret_cast<RefCount<T>*>(ptr);
	refCount.ptr = objP;

	return std::shared_ptr(refCount);
}

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