#pragma once

#include <vector>

namespace Jo{
 namespace Memory{
	template < typename _T, size_t _NumElements>
	class PoolAllocator
	{
		constexpr static size_t TypeSize = sizeof(_T);
		constexpr static size_t BlockSize = TypeSize * _NumElements;
	public:
		PoolAllocator()
		{
			newBlock();
			bp = 0;
		}
		
		~PoolAllocator()
		{
			for (auto block : memBlocks)
				free(block);
		}

		//allocate a block with the given amounts of bytes
		_T* Alloc()
		{
			if (sp - (char*)memBlocks[bp] + TypeSize > BlockSize) newBlock();
			
			auto ptr = sp;
			sp += TypeSize;

			return reinterpret_cast<_T*>(ptr);
		}

		void Delete(void* ptr)
		{
		}

		//throws away extra blocks and resets to the default state
		void Reset()
		{
			for (size_t i = 1; i < memBlocks.size(); ++i)
				free(memBlocks[i]);

			memBlocks.resize(1);
		}
	private:
		void newBlock()
		{
			memBlocks.push_back(malloc(BlockSize));
			bp++;
			sp = (char*)memBlocks.back();
		}

		char* sp; // stack pointer
		size_t bp; //block pointer
		std::vector< void* > memBlocks;
	};
} // end namespace Memory
} // end namespace Jo