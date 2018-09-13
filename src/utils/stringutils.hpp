#pragma once

#include <string>

namespace Utils
{
	/// \brief Returns _num as string with exactly _digits chars after the point.
	/// Pads the whole string with leading spaces to have _numDigits size.
	template < typename _T >
	std::string ToConstDigit(_T _num, unsigned int _digits, unsigned int _numDigits = 0)
	{
		std::string ret = ToFixPoint(_num, _digits);

		const auto i = _numDigits ? _numDigits : ret.size();
		const auto s = ret.size();
		if (s > _numDigits) _numDigits = (unsigned)s;

		std::string pading; 
		pading.resize(_numDigits - s);
		for (auto& c : pading)
			c = ' ';

		return pading + ret;
	}


	/// \brief Returns _num as string with up to _digits after the point.
	template < typename _T >
	std::string ToFixPoint(_T _num, unsigned int _digits)
	{
		std::string ret = std::to_string(_num);

		const size_t i = ret.find('.');

		if(i != std::string::npos) ret.resize(i + _digits + 1);

		return std::move(ret);
	}

	template< typename... Args>
	std::string FormatString(const char* _string, Args&&... _args)
	{
		char buf[1024];
		sprintf_s(buf, _string, std::forward<Args>(_args)...);
		return std::string(buf);
	}

} 