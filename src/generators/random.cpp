#include "random.hpp"

namespace Generators {

	thread_local RandomGenerator g_random(0xa226bf0f);

	uint32_t Xorshift32Impl::operator()()
	{
		m_state ^= m_state << 13;
		m_state ^= m_state >> 17;
		m_state ^= m_state << 5;
		return m_state;
	}

	// ************************************************************** //
	void Xorshift128Impl::init()
	{
		m_state[0] = 0xaffeaffe ^ m_seed;
		m_state[1] = 0xf9b2a750 ^ (m_seed * 0x804c8a24 + 0x68f699be);
		m_state[2] = 0x485eac66 ^ (m_seed * 0x0fe56638 + 0xc917c8ce);
		m_state[3] = 0xcbd02714 ^ (m_seed * 0x57571dae + 0xce2b3bd1);
	}

	uint32_t Xorshift128Impl::operator()()
	{
		uint32 tmp = m_state[0] ^ (m_state[0] << 11);
		m_state[0] = m_state[1];
		m_state[1] = m_state[2];
		m_state[2] = m_state[3];
		m_state[3] ^= (m_state[3] >> 19) ^ tmp ^ (tmp >> 8);

		return m_state[3];
	}
}