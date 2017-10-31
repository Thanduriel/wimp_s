#pragma once

#include "Structs/Vector3.hpp"

template<typename T, unsigned N>
struct Vec
{
	Vec()
	{
		for (unsigned i = 0; i < N; ++i)
			data[i] = static_cast<T>(0);
	}

	Vec(T _x, T _y, T _z)
	{
		data[0] = _x;
		data[1] = _y;
		data[2] = _z;
	}

	T& operator[](unsigned _ind) { return data[_ind]; }
	const T& operator[](unsigned _ind) const { return data[_ind]; }

	template<typename A, typename = decltype(std::declval<A>()[0])>
	Vec<T, N>& operator=(const A& _rhs)
	{
		for (unsigned i = 0; i < N; ++i)
			data[i] = _rhs[i];

		return *this;
	}

	Vec<T, 3>& operator=(const quickhull::Vector3<T>& _rhs)
	{
		data[0] = _rhs.x;
		data[1] = _rhs.y;
		data[2] = _rhs.z;

		return *this;
	}

	Vec<T, N>& operator += (const Vec<T, N>& _oth)
	{
		for (unsigned i = 0; i < N; ++i)
			data[i] += _oth[i];

		return *this;
	}

	Vec<T, N>& operator *(T _scalar)
	{
		for (unsigned i = 0; i < N; ++i)
			data[i] *= _scalar;

		return *this;
	}

	Vec<T, N>& operator *(const Vec<T,N>& _oth)
	{
		for (unsigned i = 0; i < N; ++i)
			data[i] *= _oth[i];

		return *this;
	}
private:
	T data[N];
};

typedef Vec<float, 3> Vec3;
typedef Vec<float, 2> Vec2;