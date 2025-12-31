#pragma once

#include <string>

inline bool contains_substring_case_insensitive(const std::string& text, const std::string& substring)
{
	auto it = std::search(
		text.begin(), text.end(),
		substring.begin(), substring.end(),
		[](char a, char b) {
			return std::tolower((unsigned char)a)
			== std::tolower((unsigned char)b);
		}
	);
	return it != text.end();
}

template<typename It, typename T>
int32_t binary_search_less_equal(const It begin, const It end, const T& value)
{
	using diff_t = typename std::iterator_traits<It>::difference_type;
	diff_t n = std::distance(begin, end);
	diff_t l = 0;
	diff_t r = n;
	while (l < r)
	{
		diff_t m = (l + r) / 2;
		if (*(begin + m) <= value) l = m + 1;
		else r = m;
	}
	if (l == 0) return 0;
	if (l == std::distance(begin, end)) return n - 1;
	return l - 1;
}
