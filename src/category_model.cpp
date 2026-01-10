#include "category_model.hpp"
#include "util/log.hpp"

void CategoryModel::add(const Category& category)
{
	int32_t index = 0;
	while (index < categories.size() && category.id < categories[index].id) index++;
	categories.insert(categories.begin() + index, category);
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
