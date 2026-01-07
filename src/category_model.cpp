#include "category_model.hpp"

void CategoryModel::add(const std::string& name, const Color& color)
{
	category_names.push_back(name);
	category_colors.emplace(name, color);
	dirty = true;
}

void CategoryModel::clear()
{
	category_names.clear();
	category_colors.clear();
	dirty = true;
}

const std::vector<std::string>& CategoryModel::get_names() const
{
	return category_names;
}

const std::unordered_map<std::string, Color>& CategoryModel::get_colors() const
{
	return category_colors;
}

