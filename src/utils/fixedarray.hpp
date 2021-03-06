#pragma once

#include <memory>
#include "assert.hpp"

template<typename T>
class FixedArray
{
public:
	FixedArray(size_t _numElements)
		: m_data(reinterpret_cast<T*>(new char[sizeof(T) * _numElements])),
		m_capacity(_numElements),
		m_size(0)
	{}

	~FixedArray()
	{
		for (size_t i = 0; i < m_size; ++i)
			m_data[i].~T();

		delete[] reinterpret_cast<char*>(m_data);
	}

	template<typename... Args>
	void emplace(Args&&... _args)
	{
		Assert(m_size < m_capacity, "This array is already full.");
		new (&m_data[m_size]) T(std::forward <Args>(_args)...);
		++m_size;
	}

	T& operator[](size_t _ind) { Assert(_ind < m_size, "Index access out of bounds.");  return m_data[_ind]; }
	const T& operator[](size_t _ind) const { Assert(_ind < m_size, "Index access out of bounds.");  return m_data[_ind]; }

	size_t size() const { return m_size; }
	size_t capacity() const { return m_capacity; }
	T* begin() { return m_data; }
	T* end() { return m_data + m_size; }

	const T* begin() const { return m_data; }
	const T* end() const { return m_data + m_size; }
private:
	T* m_data;
	size_t m_size;
	size_t m_capacity;
};