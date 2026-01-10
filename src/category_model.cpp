#include "category_model.hpp"
#include "util/log.hpp"

int32_t CategoryModel::count() const
{
	return categories.size();
}

void CategoryModel::add(const Category& category)
{
	categories.push_back(category);
	dirty = true;
}

void CategoryModel::set_name(int32_t index, const std::string& new_name)
{
	categories[index].name = new_name;
	dirty = true;
}

void CategoryModel::set_color(int32_t index, const Color& new_color)
{
	categories[index].color = new_color;
	dirty = true;
}

void CategoryModel::clear()
{
	categories.clear();
	dirty = true;
}

const Category& CategoryModel::get_category(uint64_t id) const
{
	for (int32_t i = 0; i < categories.size(); i++)
	{
		if (categories[i].id == id) return categories[i];
	}
	CG_THROW("Failed to find category id!");
}

const std::vector<Category>& CategoryModel::get_categories() const
{
	return categories;
}
