#include <unordered_map>

namespace std
{
	template<typename T1, typename T2> 
	struct hash<std::pair<T1, T2>>
	{
		typedef std::pair<T1, T2> argument_type;

		size_t operator()(const argument_type& s) const
		{
			size_t const h1(std::hash<T1>{}(s.first));
			size_t const h2(std::hash<T2>{}(s.second));
			return h1 ^ (h2 << 1);
		}
	};

	template<typename T1, typename T2, typename T3>
	struct hash<std::tuple<T1, T2, T3>>
	{
		typedef std::tuple<T1, T2, T3> argument_type;

		size_t operator()(const argument_type& s) const
		{
			const size_t h1(std::hash<T1>{}(s[0]));
			const size_t h2(std::hash<T2>{}(s[1]));
			const size_t h3(std::hash<T3>{}(s[2]))
			return h1 ^ (h2 << 1) ^ (h3 >> 1);
		}
	};
}