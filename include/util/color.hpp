#pragma once
#include "imgui.h"
#include <cstdint>
#include <string>

class Color
{
public:
	explicit Color() : r(0.0f), g(0.0f), b(0.0f), a(0.0f)
	{}
	explicit Color(float r, float g, float b) : r(r), g(g), b(b), a(1.0f)
	{}
	explicit Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a)
	{}
	explicit Color(const std::string& hex_string);
	explicit Color(uint32_t hex_color)
	{
		a = float(hex_color & 0xff) / 255.0f;
		hex_color >>= 8;
		b = float(hex_color & 0xff) / 255.0f;
		hex_color >>= 8;
		g = float(hex_color & 0xff) / 255.0f;
		hex_color >>= 8;
		r = float(hex_color) / 255.0f;
	}

	uint32_t get_hex_color() const;
	ImU32 get_ImU32() const;
	std::string to_string() const;
	float r;
	float g;
	float b;
	float a;
};
