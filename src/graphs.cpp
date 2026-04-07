#include "graphs.hpp"
#include "implot.h"
#include "implot_internal.h"
#include "util/utils.hpp"
#include <map>

constexpr int64_t year_second_count = 31536000;

void TotalAmountGraph::update_data(const TransactionModel& transaction_model)
{
	int64_t amount = 0;
	min_amount = std::numeric_limits<double>::max();
	max_amount = std::numeric_limits<double>::lowest();
	time_points.clear();
	data_points.clear();
	unix_seconds_now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::sys_days{DateUtils::to_date(Clock::now())}.time_since_epoch()).count();
	for (int32_t i = transaction_model.count() - 1; i >= 0; i--)
	{
		std::shared_ptr<const Transaction> transaction = transaction_model.at(i);
		amount += transaction->amount.value;
		int64_t unix_seconds = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::sys_days{transaction->date}.time_since_epoch()).count();
		// draw line to old value to prevent diagonal lines
		time_points.push_back(unix_seconds);
		double amount_data = double(amount) / 100.0;
		data_points.push_back(amount_data);
		min_amount = std::min(min_amount, amount_data);
		max_amount = std::max(max_amount, amount_data);
	}
	if (transaction_model.is_empty())
	{
		time_points.push_back(unix_seconds_now);
		data_points.push_back(0.0);
		min_amount = 0.0;
		max_amount = 0.0;
	}
}

void TotalAmountGraph::draw_small_graph(ImVec2 available_space, bool show_amounts)
{
	ImDrawList* draw_list = ImPlot::GetPlotDrawList();
	ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0.0f, 0.0f));
	ImPlot::PushStyleVar(ImPlotStyleVar_LabelPadding, ImVec2(0.0f, 0.0f));
	ImPlot::PushStyleColor(ImPlotCol_PlotBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
	ImPlot::PushStyleColor(ImPlotCol_AxisBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
	// keep space for custom drawn labels and spacing below them
	available_space.y -= (ImGui::GetTextLineHeight() + ImGui::GetStyle().ItemSpacing.y);
	if (ImPlot::BeginPlot("##Small Total Amount Plot", available_space, ImPlotFlags_NoLegend | ImPlotFlags_NoMenus | ImPlotFlags_NoMouseText))
	{
		ImPlot::SetupAxes("##Time", "##Amount", ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_NoTickLabels, ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_NoTickLabels);
		ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
		ImPlot::SetupAxesLimits(double(unix_seconds_now - year_second_count), double(unix_seconds_now), min_amount - std::abs(min_amount) * 0.05f, max_amount + std::abs(max_amount) * 0.05f, ImGuiCond_Always);
		if (ImPlot::BeginItem("##Small Total Amount Plot Item"))
		{
			if (show_amounts)
			{
				if (ImPlot::IsPlotHovered())
				{
					ImPlotPoint mouse = ImPlot::GetPlotMousePos();
					// find mouse location index
					int32_t idx = binary_search_less_equal(time_points.cbegin(), time_points.cend(), int64_t(mouse.x));
					if (idx != -1)
					{
						ImGui::BeginTooltip();
						std::string date = DateUtils::to_string(DateUtils::to_date(DateTime{std::chrono::seconds{int64_t(mouse.x)}}));
						ImGui::Text("%s", date.c_str());
						ImGui::Text("%s", Amount(int64_t(data_points[idx] * 100.0)).to_string_view().c_str());
						ImGui::EndTooltip();
						draw_list->AddCircleFilled(ImVec2(ImPlot::PlotToPixels(mouse.x, data_points[idx])), 6.0f, IM_COL32(255, 0, 0, 255));
					}
				}
				ImVec2 previous_pos(0.0, 0.0);
				for (int i = 0; i < time_points.size(); i++)
				{
					ImVec2 current_pos(time_points[i], data_points[i]);
					if (previous_pos.x > 0.0) draw_list->AddLine(ImPlot::PlotToPixels(previous_pos), ImPlot::PlotToPixels(current_pos.x, previous_pos.y), IM_COL32(0, 255, 255, 255), 2.0f);
					draw_list->AddLine(ImPlot::PlotToPixels(current_pos.x, previous_pos.y), ImPlot::PlotToPixels(current_pos), IM_COL32(0, 255, 255, 255), 2.0f);
					previous_pos = current_pos;
				}
			}
			ImPlot::EndItem();
		}
		Date current_date = DateUtils::to_date(Clock::now());
		std::chrono::year_month current_month_date = current_date.year() / current_date.month();
		for (int32_t i = 11; i >= 0; i--)
		{
			Date first_day{(current_month_date - std::chrono::months{i}) / std::chrono::day{1}};
			std::chrono::sys_seconds tp{std::chrono::sys_days{first_day}};
			double x = tp.time_since_epoch().count();
			double y = ImPlot::GetPlotLimits().Min().y;
			ImVec2 screen_pos = ImPlot::PlotToPixels(x, y);
			draw_list->AddText(ImVec2(screen_pos.x + 2, screen_pos.y), IM_COL32(255, 255, 255, 255), std::format("{:%B}", tp).c_str());
		}
		ImPlot::EndPlot();
	}
	ImPlot::PopStyleColor(2);
	ImPlot::PopStyleVar(2);
	// space for custom labels, remove spacing above
	ImGui::Dummy(ImVec2(0.0f, ImGui::GetTextLineHeight() - ImGui::GetStyle().ItemSpacing.y));
}

void TotalAmountGraph::draw_large_graph(ImVec2 available_space)
{
	static constexpr ImVec4 color(0.0f, 0.4f, 0.4f, 1.0f);
	ImDrawList* draw_list = ImPlot::GetPlotDrawList();
	ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0.0f, 0.0f));
	ImPlot::PushStyleVar(ImPlotStyleVar_LabelPadding, ImVec2(0.0f, 0.0f));
	ImPlot::PushStyleColor(ImPlotCol_PlotBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
	ImPlot::PushStyleColor(ImPlotCol_AxisBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
	ImPlot::PushStyleColor(ImPlotCol_LegendBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
	ImPlot::PushStyleColor(ImPlotCol_FrameBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
	ImPlot::PushStyleColor(ImPlotCol_AxisBgHovered, ImVec4(color.x + 0.2f, color.y + 0.2f, color.z + 0.2f, color.w));
	ImPlot::PushStyleColor(ImPlotCol_AxisBgActive, ImVec4(color.x + 0.2f, color.y + 0.2f, color.z + 0.2f, color.w));
	if (ImPlot::BeginPlot("##Total Amount Plot", available_space, ImPlotFlags_NoLegend | ImPlotFlags_NoMenus | ImPlotFlags_NoMouseText))
	{
		ImPlot::SetupAxes("##Time", "##Amount", ImPlotAxisFlags_NoLabel, ImPlotAxisFlags_NoLabel);
		ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
		ImPlot::SetupAxesLimits(double(unix_seconds_now - year_second_count), double(unix_seconds_now), min_amount - std::abs(min_amount) * 0.05f, max_amount + std::abs(max_amount) * 0.05f, ImGuiCond_Once);
		if (ImPlot::BeginItem("##Total Amount Plot Item"))
		{
			if (ImPlot::IsPlotHovered())
			{
				ImPlotPoint mouse = ImPlot::GetPlotMousePos();
				// find mouse location index
				int32_t idx = binary_search_less_equal(time_points.cbegin(), time_points.cend(), int64_t(mouse.x));
				if (idx != -1)
				{
					ImGui::BeginTooltip();
					std::string date = DateUtils::to_string(DateUtils::to_date(DateTime{std::chrono::seconds{int64_t(mouse.x)}}));
					ImGui::Text("%s", date.c_str());
					ImGui::Text("%s", Amount(int64_t(data_points[idx] * 100.0)).to_string_view().c_str());
					ImGui::EndTooltip();
					draw_list->AddCircleFilled(ImVec2(ImPlot::PlotToPixels(mouse.x, data_points[idx])), 8.0f, IM_COL32(255, 0, 0, 255));
				}
			}
			// fit data if requested
			if (ImPlot::FitThisFrame())
			{
				for (int32_t i = 0; i < time_points.size(); i++)
				{
					ImPlot::FitPoint(ImPlotPoint(time_points[i], data_points[i]));
				}
			}
			ImVec2 previous_pos(0.0, 0.0);
			for (int i = 0; i < time_points.size(); i++)
			{
				ImVec2 current_pos(time_points[i], data_points[i]);
				if (previous_pos.x > 0.0) draw_list->AddLine(ImPlot::PlotToPixels(previous_pos), ImPlot::PlotToPixels(current_pos.x, previous_pos.y), IM_COL32(0, 255, 255, 255), 2.0f);
				draw_list->AddLine(ImPlot::PlotToPixels(current_pos.x, previous_pos.y), ImPlot::PlotToPixels(current_pos), IM_COL32(0, 255, 255, 255), 2.0f);
				previous_pos = current_pos;
			}
			ImPlot::EndItem();
		}
		Date current_date = DateUtils::to_date(Clock::now());
		std::chrono::year_month current_month_date = current_date.year() / current_date.month();
		for (int32_t i = 11; i >= 0; i--)
		{
			Date first_day{(current_month_date - std::chrono::months{i}) / std::chrono::day{1}};
			std::chrono::sys_seconds tp{std::chrono::sys_days{first_day}};
			double x = tp.time_since_epoch().count();
			double y = ImPlot::GetPlotLimits().Min().y;
			ImVec2 screen_pos = ImPlot::PlotToPixels(x, y);
		}
		ImPlot::EndPlot();
	}
	ImPlot::PopStyleColor(6);
	ImPlot::PopStyleVar(2);
}

void TotalAmountGraph::draw_bar_spending_graph(const TransactionModel& transaction_model, ImVec2 available_space)
{
	static const char* timeframe_names[] = { "Day", "Week", "Month", "Year" };
	static int selected_timeframe = 2; 
	
	ImGui::SetNextItemWidth(150.0f);
	bool changed = ImGui::Combo("Interval##Bars", &selected_timeframe, timeframe_names, IM_ARRAYSIZE(timeframe_names));

	// Use map to group transactions by their period start date
	std::map<int64_t, double> bucket_data;
	double max_val = 0.0;

	for (int32_t i = 0; i < transaction_model.count(); ++i)
	{
		const std::shared_ptr<const Transaction> t = transaction_model.at(i);
		Date date = t->date;

		// Clamp the date to the beginning of the period
		if (selected_timeframe == 1)
		{ // Week
			std::chrono::sys_days sys_d = std::chrono::sys_days{date};
			// sunday is 0 - saturday is 6
			unsigned int weekday = std::chrono::weekday{sys_d}.c_encoding();
			
			// Clamp to monday
			int days_to_subtract = (weekday == 0 ? 6 : (int)weekday - 1);
			date = Date{sys_d - std::chrono::days{days_to_subtract}};
		} 
		else if (selected_timeframe == 2)
		{
			date = date.year() / date.month() / 1; // Month
		} 
		else if (selected_timeframe == 3)
		{
			date = date.year() / 1 / 1; // Year
		}

		int64_t time_stamp = std::chrono::system_clock::to_time_t(std::chrono::sys_days{date});
		double amount = std::abs(static_cast<double>(t->amount.value) / 100.0);
		
		bucket_data[time_stamp] += amount;
		// Set the highest peak 
		if (bucket_data[time_stamp] > max_val) max_val = bucket_data[time_stamp];
	}

	if (bucket_data.empty()) return;

	// Convert map to vectors for ImPlot
	std::vector<double> xs, ys;
	for (const std::pair<const int64_t, double>& entry : bucket_data)
	{
		xs.push_back(static_cast<double>(entry.first));
		ys.push_back(entry.second);
	}

	static constexpr ImVec4 theme_color(0.0f, 0.4f, 0.4f, 1.0f);
	ImDrawList* draw_list = ImPlot::GetPlotDrawList();

	ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0.0f, 0.0f));
	ImPlot::PushStyleVar(ImPlotStyleVar_LabelPadding, ImVec2(0.0f, 0.0f));
	
	ImPlot::PushStyleColor(ImPlotCol_PlotBg, ImVec4(0,0,0,1));
	ImPlot::PushStyleColor(ImPlotCol_AxisBg, ImVec4(0,0,0,1));
	ImPlot::PushStyleColor(ImPlotCol_FrameBg, ImVec4(0,0,0,1));
	ImPlot::PushStyleColor(ImPlotCol_AxisBgHovered, ImVec4(theme_color.x + 0.2f, theme_color.y + 0.2f, theme_color.z + 0.2f, theme_color.w));
	ImPlot::PushStyleColor(ImPlotCol_AxisBgActive, ImVec4(theme_color.x + 0.2f, theme_color.y + 0.2f, theme_color.z + 0.2f, theme_color.w));

	if (ImPlot::BeginPlot("##SpendingBarPlot", available_space, ImPlotFlags_NoLegend | ImPlotFlags_NoMenus | ImPlotFlags_NoMouseText)) 
	{
		ImPlot::SetupAxes("##Time", "##Amount", ImPlotAxisFlags_NoLabel, ImPlotAxisFlags_NoLabel);
		ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);

		// Set the width of the time axis for each bar
		double width_seconds = 86400.0; // 1 Day
		if (selected_timeframe == 1) width_seconds *= 7; // Week
		else if (selected_timeframe == 2) width_seconds *= 28; // Month
		else if (selected_timeframe == 3) width_seconds *= 365; // Year

		ImPlot::SetupAxesLimits(xs.front() - width_seconds, xs.back() + width_seconds, 0, max_val * 1.1, changed ? ImGuiCond_Always : ImGuiCond_Once);

		if (ImPlot::BeginItem("##BarItem")) 
		{
			// Draw bars
			ImPlot::PushStyleColor(ImPlotCol_Fill, theme_color);
			ImPlot::PlotBars("##Bars", xs.data(), ys.data(), (int)xs.size(), width_seconds * 0.7);
			ImPlot::PopStyleColor();

			// Hover logic
			if (ImPlot::IsPlotHovered())
			{
				ImPlotPoint mouse = ImPlot::GetPlotMousePos();
				int32_t idx = binary_search_less_equal(xs.begin(), xs.end(), mouse.x);
				
				if (idx >= 0 && idx < xs.size())
				{
					ImGui::BeginTooltip();
					std::string date_str = DateUtils::to_string(DateUtils::to_date(DateTime{std::chrono::seconds{(int64_t)xs[idx]}}));
					ImGui::Text("Period Start: %s", date_str.c_str());
					ImGui::Text("Total: %.2f €", ys[idx]);
					ImGui::EndTooltip();

					draw_list->AddCircleFilled(ImPlot::PlotToPixels(xs[idx], ys[idx]), 8.0f, IM_COL32(255, 0, 0, 255));
				}
			}
				ImPlot::EndItem();
			}
		ImPlot::EndPlot();
	}
	ImPlot::PopStyleColor(5);
	ImPlot::PopStyleVar(2);
}