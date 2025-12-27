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
