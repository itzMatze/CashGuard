#include "diagrams.hpp"
#include "implot.h"
#include "implot_internal.h"
#include "util/utils.hpp"
#include <map>

void Diagrams::draw_pie_chart(const CategoryModel& categories, const TransactionModel& transaction_model, ImVec2 size)
{
	if (categories.count() == 0)
		return;

	const char* date_selection[] = {"1 Day", "1 Week", "1 Month", "1 Year", "All", "Custom"};
	int current_item = static_cast<int>(m_current_range);

	ImGui::SetNextItemWidth(160.0f);
	if (ImGui::Combo("Filter Period##Pie", &current_item, date_selection, IM_ARRAYSIZE(date_selection)))
	{
		m_current_range = static_cast<TimeRange>(current_item);
	}

	std::chrono::system_clock::time_point time_point = std::chrono::system_clock::now();
	std::chrono::year_month_day current_date = std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(time_point)};
	std::chrono::year_month_day min_date;

	ImGui::SameLine();
	if (m_current_range == TimeRange::All)
	{
		ImGui::TextDisabled("(Showing All Time)");
	}
	else if (m_current_range == TimeRange::Custom)
	{
		ImGui::Text("From:");
		ImGui::SameLine();
		render_date_input("##start_pie", m_custom_start);
		ImGui::SameLine();
		ImGui::Text("To:");
		ImGui::SameLine();
		render_date_input("##end_pie", m_custom_end);

		min_date = m_custom_start;
		current_date = m_custom_end;
	}
	else
	{
		std::chrono::days days_to_subtract{0};
		if (m_current_range == TimeRange::Day)
			days_to_subtract = std::chrono::days(1);
		if (m_current_range == TimeRange::Week)
			days_to_subtract = std::chrono::days(7);
		if (m_current_range == TimeRange::Month)
			days_to_subtract = std::chrono::days(30);
		if (m_current_range == TimeRange::Year)
			days_to_subtract = std::chrono::days(365);

		min_date = std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(time_point - days_to_subtract)};

		ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Range: %s to %s", DateUtils::to_string(min_date).c_str(), DateUtils::to_string(current_date).c_str());
	}

	std::vector<double> values;
	std::vector<ImU32> slice_colors;
	static std::vector<std::string> formatted_labels;
	formatted_labels.clear();
	std::vector<const char*> labels_ptr;

	for (int32_t i = 0; i < categories.count(); ++i)
	{
		const Category& category = categories.get_categories()[i];
		double total_amount = 0.0;

		for (int32_t j = 0; j < transaction_model.count(); ++j)
		{
			auto t = transaction_model.at(j);
			bool in_range = (m_current_range == TimeRange::All) || (t->date >= min_date && t->date <= current_date);

			if (in_range && t->category_id == category.id)
			{
				total_amount += std::abs(static_cast<double>(t->amount.value) / 100.0);
			}
		}

		if (total_amount > 0.0)
		{
			values.push_back(total_amount);
			slice_colors.push_back(category.color.get_ImU32());

			// Add amount to label
			char buf[128];
			snprintf(buf, sizeof(buf), "%s: %.2f €", category.name.c_str(), total_amount);
			formatted_labels.push_back(buf);
		}
	}

	if (values.empty())
	{
		ImGui::Spacing();
		ImGui::Text("No data found for this period.");
		return;
	}

	for (const std::string& str : formatted_labels)
		labels_ptr.push_back(str.c_str());

	// Colormap
	std::string map_name = "PieMap_" + std::to_string(slice_colors.size());
	ImPlotColormap cmap = ImPlot::GetColormapIndex(map_name.c_str());
	if (cmap == -1)
	{
		cmap = ImPlot::AddColormap(map_name.c_str(), slice_colors.data(), (int)slice_colors.size());
	}

	ImPlot::PushColormap(cmap);

	// Subtract UI height from available space
	float header_height = ImGui::GetFrameHeightWithSpacing() * 2.0f;
	ImVec2 plot_size(size.x, size.y - header_height);

	if (ImPlot::BeginPlot("##CategoryPie", plot_size, ImPlotFlags_Equal))
	{
		ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);

		ImPlot::SetupLegend(ImPlotLocation_West, ImPlotLegendFlags_Outside);

		ImPlot::PlotPieChart(labels_ptr.data(), values.data(), (int)values.size(), 0.5, 0.5, 1.5, "", 90.0, ImPlotPieChartFlags_Exploding);

		ImPlot::EndPlot();
	}
	ImPlot::PopColormap();
}

void Diagrams::draw_bar_group(const CategoryModel& categories, const TransactionModel& transaction_model, ImVec2 size)
{
	if (categories.count() == 0)
		return;

	const char* date_selection[] = {"1 Day", "1 Week", "1 Month", "1 Year", "All", "Custom"};
	int current_item = static_cast<int>(m_current_range);

	ImGui::SetNextItemWidth(160.0f);
	if (ImGui::Combo("Filter Period", &current_item, date_selection, IM_ARRAYSIZE(date_selection)))
	{
		m_current_range = static_cast<TimeRange>(current_item);
	}

	std::chrono::system_clock::time_point time_point = std::chrono::system_clock::now();
	std::chrono::year_month_day current_date = std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(time_point)};
	std::chrono::year_month_day min_date;

	ImGui::SameLine();
	if (m_current_range == TimeRange::All)
	{
		ImGui::SameLine();
		ImGui::TextDisabled("(Showing All Time)");
	}
	else if (m_current_range == TimeRange::Custom)
	{
		ImGui::SameLine();
		ImGui::Text("From:");
		ImGui::SameLine();

		render_date_input("##start", m_custom_start);

		ImGui::SameLine();
		ImGui::Text("To:");
		ImGui::SameLine();
		render_date_input("##end", m_custom_end);

		min_date = m_custom_start;
		current_date = m_custom_end;
	}
	else
	{
		std::chrono::days days_to_subtract{0};
		if (m_current_range == TimeRange::Day)
			days_to_subtract = std::chrono::days(1);
		if (m_current_range == TimeRange::Week)
			days_to_subtract = std::chrono::days(7);
		if (m_current_range == TimeRange::Month)
			days_to_subtract = std::chrono::days(30);
		if (m_current_range == TimeRange::Year)
			days_to_subtract = std::chrono::days(365);

		min_date = std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(time_point - days_to_subtract)};

		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Range: %s to %s", DateUtils::to_string(min_date).c_str(), DateUtils::to_string(current_date).c_str());
	}

	if (ImPlot::BeginPlot("Category Totals", size))
	{
		std::vector<const char *> category_names;
		std::vector<double> positions;

		for (int32_t i = 0; i < categories.count(); ++i)
		{
			category_names.push_back(categories.get_categories()[i].name.c_str());
			positions.push_back(static_cast<double>(i));
		}

		ImPlot::SetupAxes("Amount (€)", "", ImPlotAxisFlags_None, ImPlotAxisFlags_None);
		ImPlot::SetupAxisTicks(ImAxis_Y1, positions.data(), static_cast<int>(positions.size()), category_names.data());
		ImPlot::SetupLegend(ImPlotLocation_West, ImPlotLegendFlags_Outside);

		for (int32_t i = 0; i < categories.count(); ++i)
		{
			const Category &category = categories.get_categories()[i];
			double total_amount = 0.0;

			for (int32_t j = 0; j < transaction_model.count(); ++j)
			{
				std::shared_ptr<const Transaction> t = transaction_model.at(j);

				bool in_range = (m_current_range == TimeRange::All) || (t->date >= min_date);

				if (in_range && t->category_id == category.id)
				{
					total_amount += std::abs(static_cast<double>(t->amount.value) / 100.0);
				}
			}

			if (total_amount > 0.0)
			{
				double x_pos = static_cast<double>(i);
				ImPlot::PushStyleColor(ImPlotCol_Fill, category.color.get_ImU32());
				ImPlot::PlotBars(category.name.c_str(), &total_amount, &x_pos, 1, 0.5f, ImPlotBarsFlags_Horizontal);
				ImPlot::PopStyleColor();
			}
		}
		ImPlot::EndPlot();
	}
}

void Diagrams::render_date_input(const char* id, std::chrono::year_month_day& date)
{
	int year = static_cast<int>(date.year());
	int month = static_cast<unsigned int>(date.month());
	int day = static_cast<unsigned int>(date.day());

	ImGui::PushID(id);
	float input_width = 60.0f;
	bool changed = false;

	// Year input
	ImGui::PushItemWidth(input_width + 10.0f);
	if (ImGui::InputInt("##Year", &year, 0, 0))
		changed = true;
	ImGui::PopItemWidth();

	ImGui::SameLine(0, 2);

	// Month input
	ImGui::PushItemWidth(input_width);
	if (ImGui::InputInt("##Month", &month, 0, 0))
	{
		if (month < 1)
			month = 1;
		if (month > 12)
			month = 12;
		changed = true;
	}
	ImGui::PopItemWidth();

	ImGui::SameLine(0, 2);

	// Day input
	ImGui::PushItemWidth(input_width);
	if (ImGui::InputInt("##Day", &day, 0, 0))
	{
		if (day < 1)
			day = 1;
		if (day > 31)
			day = 31;
		changed = true;
	}
	ImGui::PopItemWidth();

	if (changed)
	{
		date = std::chrono::year_month_day{std::chrono::year{year}, std::chrono::month{static_cast<unsigned int>(month)}, std::chrono::day{static_cast<unsigned int>(day)}};
	}

	ImGui::PopID();
}