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
}