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

	void add(const Category& category);
	void clear();
	const Category& get_category(uint64_t id) const;
	const std::vector<Category>& get_categories() const;

	mutable bool dirty = false;

private:
	std::vector<Category> categories;
};
