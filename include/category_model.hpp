#pragma once

#include "util/color.hpp"
#include <string>
#include <vector>

struct Category
{
	uint64_t id;
	std::string name;
	Color color;
};

class CategoryModel
{
public:
	CategoryModel() = default;

	int32_t count() const;
	void add(const Category& category);
	void remove(int32_t index);
	void set_name(int32_t index, const std::string& name);
	void set_color(int32_t index, const Color& new_color);
	void clear();
	const Category& get_category(uint64_t id) const;
	const std::vector<Category>& get_categories() const;

	mutable bool dirty = false;

private:
	std::vector<Category> categories;
};
