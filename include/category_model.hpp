#pragma once

#include "util/color.hpp"
#include <string>
#include <unordered_map>
#include <vector>

class CategoryModel
{
public:
	CategoryModel() = default;

	void add(const std::string& name, const Color& color);
	void clear();
	const std::vector<std::string>& get_names() const;
	const std::unordered_map<std::string, Color>& get_colors() const;

	mutable bool dirty = false;

private:
	std::vector<std::string> category_names;
	std::unordered_map<std::string, Color> category_colors;
};
