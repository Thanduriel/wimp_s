namespace ei {

	// slerp variant that always rotates around the smaller angle.
	template<typename T>
	TQuaternion<T> slerpLess(const TQuaternion<T>& _q0, TQuaternion<T> _q1, T _t) // TESTED
	{
		auto dotP = dot(_q0, _q1);
		if (dotP < 0.f)
		{
			_q1 = -_q1;
			dotP = -dotP;
		}

		T theta = acos(clamp(dotP, T(-1), T(1)));
		T so = sin(theta);
		if (approx(so, T(0)))
		{
			// Converges towards linear interpolation for small so
			return TQuaternion<T>(_q0.i + (_q1.i - _q0.i) * _t,
				_q0.j + (_q1.j - _q0.j) * _t,
				_q0.k + (_q1.k - _q0.k) * _t,
				_q0.r + (_q1.r - _q0.r) * _t);
		}

		T f0 = sin(theta * (1.0f - _t)) / so;
		T f1 = sin(theta * _t) / so;
		return TQuaternion<T>(_q0.i * f0 + _q1.i * f1,
			_q0.j * f0 + _q1.j * f1,
			_q0.k * f0 + _q1.k * f1,
			_q0.r * f0 + _q1.r * f1);
	}
}