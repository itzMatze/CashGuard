#include "util/color.hpp"
#include <algorithm>
#include <charconv>
#include <format>

Color::Color(const std::string& hex_string)
{
	int32_t begin = 0;
	if (hex_string[begin] == '#') begin++;
	int32_t length = hex_string.size() - begin;
	if (length == 6 || length == 8)
	{
		uint32_t hex_value = 0u;
		std::from_chars(hex_string.data() + begin, hex_string.data() + hex_string.size(), hex_value, 16);
		*this = Color(hex_value);
	}
}

uint32_t Color::get_hex_color() const
{
	// color values > 1.0 are allowed, but they need to be clamped when the color is converted to hex representation
	uint32_t hex_color = std::clamp(static_cast<int64_t>(r * 255.999f), int64_t(0), int64_t(255));
	hex_color <<= 8;
	hex_color += std::clamp(static_cast<int64_t>(g * 255.999f), int64_t(0), int64_t(255));
	hex_color <<= 8;
	hex_color += std::clamp(static_cast<int64_t>(b * 255.999f), int64_t(0), int64_t(255));
	hex_color <<= 8;
	hex_color += std::clamp(static_cast<int64_t>(a * 255.999f), int64_t(0), int64_t(255));
	return hex_color;
}

ImU32 Color::get_ImU32() const
{
	uint32_t hex_color = get_hex_color();
	uint8_t r = (hex_color >> 24) & 0xff;
	uint8_t g = (hex_color >> 16) & 0xff;
	uint8_t b = (hex_color >> 8) & 0xff;
	uint8_t a = hex_color & 0xff;
	return IM_COL32(r, g, b, a);
}

std::string Color::to_string() const
{
	uint32_t hex_color = get_hex_color();
	uint8_t r = (hex_color >> 24) & 0xff;
	uint8_t g = (hex_color >> 16) & 0xff;
	uint8_t b = (hex_color >> 8) & 0xff;
	uint8_t a = hex_color & 0xff;
	return std::format("#{:02X}{:02X}{:02X}{:02X}", r, g, b, a);
}
