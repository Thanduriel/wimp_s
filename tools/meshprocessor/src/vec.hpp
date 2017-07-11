#pragma once

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
private:
	T data[N];
};

typedef Vec<float, 3> Vec3;
typedef Vec<float, 2> Vec2;