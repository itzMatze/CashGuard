#include "visualizations.hpp"
#include "implot.h"
#include "implot_internal.h"
#include "util/utils.hpp"

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

	// Bar chart
	bucket_data.clear();
	for (int timeframe_idx = 0; timeframe_idx < timeframe_names.size(); ++timeframe_idx) 
	{
		std::map<int64_t, BarTotals> temp_sums;
		BarChartData& data = bucket_data[timeframe_idx];

		data.xs.clear();
    data.incomes.clear();
    data.expenses.clear();
		data.net.clear();
		
		for (int32_t i = 0; i < transaction_model.count(); i++) 
		{
			const std::shared_ptr<const Transaction> t = transaction_model.at(i);
			Date date = t->date;
			
			// Clamp the date to the beginning of the period
			if (timeframe_idx == 1)
			{ // Week
				std::chrono::sys_days sys_d{date};
				// sunday is 0 - saturday is 6
				unsigned int weekday = std::chrono::weekday{sys_d}.c_encoding();
				// Clamp to monday
				int days_to_subtract = (weekday == 0 ? 6 : (int)weekday - 1);
				date = Date{sys_d - std::chrono::days{days_to_subtract}};
			} 
			else if (timeframe_idx == 2) { date = date.year() / date.month() / 1; } // Month
			else if (timeframe_idx == 3) { date = date.year() / 1 / 1; } //Year
			
			int64_t time_stamp = std::chrono::system_clock::to_time_t(std::chrono::sys_days{date});
			double total_amount = static_cast<double>(t->amount.value) / 100.0;
			
			if (total_amount >= 0)
			{
				temp_sums[time_stamp].temp_incomes += total_amount;
			}
			else
			{
				temp_sums[time_stamp].temp_expenses += total_amount;
			}
		}
		
		// Convert the map of sums into vectors for ImPlot
		for (std::map<int64_t, BarTotals>::const_iterator it = temp_sums.begin(); it != temp_sums.end(); ++it) 
		{
			data.xs.push_back(static_cast<double>(it->first));
			data.incomes.push_back(it->second.temp_incomes);
			data.expenses.push_back(it->second.temp_expenses);
			data.net.push_back(it->second.temp_incomes + it->second.temp_expenses);
		}
			
		// Calculate upper and lower bound to fit bars
		data.income_peak = 0.0;
		data.expense_peak = 0.0;
		for (uint32_t i = 0; i < data.incomes.size(); ++i)
		{
			// Find the absolute highest and lowest points
			if (data.incomes[i] > data.income_peak) data.income_peak = data.incomes[i];
			if (data.net[i] > data.income_peak) data.income_peak = data.net[i];
			
			if (data.expenses[i] < data.expense_peak) data.expense_peak = data.expenses[i];
			if (data.net[i] < data.expense_peak) data.expense_peak = data.net[i];
		}
		// Add padding
		double range = data.income_peak - data.expense_peak;
		if (range == 0) range = 1.0;
		data.income_peak += range * 0.1;
		data.expense_peak -= range * 0.1;
	}
}

void Diagrams::update_data(const CategoryModel& categories, const TransactionModel& transaction_model)
{
	precalculated_data.clear();

	for (int r = 0; r < (int)TimeRange::Count; ++r)
	{
		TimeRange range = static_cast<TimeRange>(r);
		Diagrams::CategoryData& category = precalculated_data[range];
		Date current_date_bound;
		Date min_date_bound;

		if (range == TimeRange::All)
		{
			min_date_bound = Date{std::chrono::year{1900}/1/1};
			current_date_bound = DateUtils::to_date(std::chrono::system_clock::now());
		} 
		else if (range == TimeRange::Custom)
		{
			min_date_bound = custom_start;
			current_date_bound = custom_end;
		}
		else
		{
			std::chrono::system_clock::time_point current_time = std::chrono::system_clock::now();
			current_date_bound = DateUtils::to_date(current_time);
			std::chrono::days time_span{0};

			if (range == TimeRange::Day)
			{
				time_span = std::chrono::days(1);
			}
			else if (range == TimeRange::Week) { time_span = std::chrono::days(7); }
			else if (range == TimeRange::Month) { time_span = std::chrono::days(30); }
			else if (range == TimeRange::Year) { time_span = std::chrono::days(365); }
				
			min_date_bound = Date{std::chrono::floor<std::chrono::days>(current_time - time_span)};
		}

		for (int i = 0; i < categories.count(); ++i)
		{
			const Category& cat = categories.get_categories()[i]; 
			double income_total = 0.0;
			double expense_total = 0.0;
			double abs_total = 0.0;

			for (int32_t j = 0; j < transaction_model.count(); ++j)
			{
				std::shared_ptr<const Transaction> t = transaction_model.at(j);
				bool in_range = (range == TimeRange::All) || (t->date >= min_date_bound && t->date <= current_date_bound);

				if (in_range && t->category_id == cat.id)
				{
					double val = static_cast<double>(t->amount.value) / 100.0;
					
					// Track income vs expenses for the bar chart
					if (val >= 0)
					{
						income_total += val;
					}
					else
					{
						expense_total += val;
					}
					
					// Track absolute total for the pie chart
					abs_total += std::abs(val);
				}
			}

			// Only add if there was any changes
			if (abs_total > 0.0)
			{
				// Pie chart data
				category.values.push_back(abs_total);
				
				// Bar chart data
				category.incomes.push_back(income_total);
				category.expenses.push_back(expense_total);
				category.net.push_back(income_total + expense_total);
				
				category.colors.push_back(cat.color.get_ImU32());
				category.names.push_back(cat.name);
				category.positions.push_back(static_cast<double>(category.positions.size()));
				
				char buf[128];
				snprintf(buf, sizeof(buf), "%s: %.2f €", cat.name.c_str(), abs_total);
				category.labels.push_back(buf);
			}
		}

		// Rebuild string pointers for ImPlot
		for (int32_t i = 0; i < category.names.size(); ++i)
		{
			category.names_ptr.push_back(category.names[i].c_str());
			category.labels_ptr.push_back(category.labels[i].c_str());
		}
	}
}	

void TotalAmountGraph::draw_small_graph(ImVec2 available_space, bool show_amounts)
{
	ImDrawList* draw_list = ImPlot::GetPlotDrawList();
	ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0.0f, 0.0f));
	ImPlot::PushStyleVar(ImPlotStyleVar_LabelPadding, ImVec2(0.0f, 0.0f));
	ImPlot::PushStyleColor(ImPlotCol_PlotBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
	ImPlot::PushStyleColor(ImPlotCol_AxisBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
	// keep space for custom drawn labels
	available_space.y -= ImGui::GetTextLineHeight();
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
	int32_t selected_timeframe  = static_cast<int>(current_selection);

	ImGui::SetNextItemWidth(150.0f);
	bool changed = ImGui::Combo("Interval##Bars", &selected_timeframe, timeframe_names.data(), static_cast<int>(timeframe_names.size()));

	available_space = ImGui::GetContentRegionAvail();

	std::map<int, BarChartData>::const_iterator it = bucket_data.find(selected_timeframe);
	if (it == bucket_data.end() || it->second.xs.empty()) return;

	const BarChartData& data = it->second;

	double width_seconds = 86400.0; 
	if (selected_timeframe == 1)
	{
		width_seconds *= 7;
	}
	else if (selected_timeframe == 2) { width_seconds *= 28; }
	else if (selected_timeframe == 3) { width_seconds *= 365; }

	if (ImPlot::BeginPlot("##SpendingBarPlot", available_space, ImPlotFlags_NoLegend | ImPlotFlags_NoMenus)) 
	{
		ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoLabel, ImPlotAxisFlags_NoLabel);
		ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
		ImPlot::SetupAxesLimits(data.xs.front() - width_seconds, data.xs.back() + width_seconds, data.expense_peak, data.income_peak, changed ? ImGuiCond_Always : ImGuiCond_Once);

		// Plot incomes
		ImPlot::SetNextFillStyle(ImVec4(0.0f, 0.7f, 0.2f, 1.0f));
		ImPlot::PlotBars("Income", data.xs.data(), data.incomes.data(), static_cast<int>(data.xs.size()), width_seconds * 0.4);

		// Plot expenses
		ImPlot::SetNextFillStyle(ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
		ImPlot::PlotBars("Expense", data.xs.data(), data.expenses.data(), static_cast<int>(data.xs.size()), width_seconds * 0.4);

		// Plot net
		ImPlot::SetNextFillStyle(ImVec4(0.3f, 0.6f, 1.0f, 1.0f));
		ImPlot::PlotBars("Net", data.xs.data(), data.net.data(), static_cast<int>(data.xs.size()), width_seconds * 0.15);

		// Tooltip logic
		if (ImPlot::IsPlotHovered())
		{
			ImPlotPoint mouse = ImPlot::GetPlotMousePos();
			int32_t idx = binary_search_less_equal(data.xs.begin(), data.xs.end(), mouse.x);

			if (idx >= 0 && idx < static_cast<int32_t>(data.xs.size()))
			{
				ImGui::BeginTooltip();
				std::string date_str = DateUtils::to_string(DateUtils::to_date(DateTime{std::chrono::seconds{(int64_t)data.xs[idx]}}));
				ImGui::Text("Period Start: %s", date_str.c_str());
				ImGui::Separator();
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Income:  %.2f €", data.incomes[idx]);
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Expense: %.2f €", data.expenses[idx]);
				ImGui::TextColored(ImVec4(0.3f, 0.6f, 1.0f, 1.0f), "Net:%.2f €", data.net[idx]);
				ImGui::EndTooltip(); 
			}
		}
		ImPlot::EndPlot();
	}
}

Diagrams::Diagrams() 
{
	std::chrono::system_clock::time_point current_time = std::chrono::system_clock::now();
	custom_start = std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(current_time)};
	custom_end = custom_start;
	current_range = TimeRange::All;
}

void Diagrams::draw_pie_chart(ImVec2 size)
{
	int32_t current_item = static_cast<int32_t>(current_range);

	ImGui::SetNextItemWidth(160.0f);
	if (ImGui::Combo("Filter Period##Pie", &current_item, range_names.data(), (int)range_names.size()))
	{
		current_range = static_cast<TimeRange>(current_item);        
	}

	CategoryData& data = precalculated_data[current_range];
	if (data.values.empty())
	{
		ImGui::Text("No data found for this period.");
		return;
	}

	// Calculate a hash for colors and values
	uint64_t current_hash = data.colors.size();
	for (int32_t i = 0; i < data.colors.size(); i++)
	{
		// Hash the color
		current_hash ^= data.colors[i] + 0x9e3779b9 + (current_hash << 6) + (current_hash >> 2);
		// Hash the value (cast to bits to hash the double)
		uint64_t val_bits = std::bit_cast<uint64_t>(data.values[i]);
		current_hash ^= val_bits + 0x9e3779b9 + (current_hash << 6) + (current_hash >> 2);
	}

	// 2. If data OR values changed, update the colormap
	if (current_hash != last_data_hash || dynamic_cmap == -1)
	{
		std::vector<ImVec4> colors;
		colors.reserve(data.colors.size());
		for (int32_t i = 0; i < data.colors.size(); i++)
		{
			ImU32 color_u32 = data.colors[i];
			// Convert to float4 and force alpha to 1.0
			ImVec4 color_vec4 = ImGui::ColorConvertU32ToFloat4(color_u32 | 0xFF000000);
			colors.push_back(color_vec4);
		}
		
		// Use the hash to create a unique name
		std::string map_name = "PieMap_" + std::to_string(current_hash);
		
		// Check if this map already exists in ImPlot
		dynamic_cmap = ImPlot::GetColormapIndex(map_name.c_str());
		if (dynamic_cmap == -1)
		{
			dynamic_cmap = ImPlot::AddColormap(map_name.c_str(), colors.data(), (int)colors.size());
		}
		
		last_data_hash = current_hash;
  }

	ImPlot::PushColormap(dynamic_cmap);

	ImPlotFlags plot_flags = ImPlotFlags_Equal | ImPlotFlags_NoMouseText | ImPlotFlags_NoInputs;
	std::string plot_id = "##CategoryPie_" + std::to_string(current_hash);
	if (ImPlot::BeginPlot(plot_id.c_str(), size, plot_flags))
	{
		// Set padding for a better fit
		float aspect = size.x / size.y;
		float y_min = -0.5f;
		float y_max = 1.5f; 
		float x_range = (y_max - y_min) * aspect;
		float x_min = 0.5f - x_range / 2.0f;
		float x_max = 0.5f + x_range / 2.0f;

		ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
		ImPlot::SetupAxesLimits(x_min, x_max, y_min, y_max, ImGuiCond_Always);

		float pie_r = 0.35f;
		ImPlot::PlotPieChart(data.labels_ptr.data(), data.values.data(), (int)data.values.size(), 0.5, 0.5, pie_r, "", 0.0, ImPlotPieChartFlags_Exploding); 

		double total_sum = 0;
		for (int32_t i = 0; i < data.values.size(); ++i)
		{
			total_sum += data.values[i];
		}

		double current_deg = 0.0; // start of the slice

		for (int32_t i = 0; i < data.values.size(); ++i)
		{
			ImVec4 cat_color = ImPlot::GetColormapColor((int)i);

			double slice_deg = (data.values[i] / total_sum) * 360.0; // width of the slice in degrees
			double mid_deg = current_deg + (slice_deg / 2.0); // the middle of the slice
			double mid_rad = mid_deg * (M_PI / 180.0);

			float cos_a = cosf((float)mid_rad);
			float sin_a = sinf((float)mid_rad);

			float line_end_r = (i % 2 == 0) ? 0.7f : 0.9f; // make one line longer than the other so the labels dont overlap
			
			// Apply category color to the line
			double line_x[2] = { 0.5 + cos_a * pie_r, 0.5 + cos_a * line_end_r }; // start of the line (edge)
			double line_y[2] = { 0.5 + sin_a * pie_r, 0.5 + sin_a * line_end_r}; // end of the line
			
			ImPlot::PushStyleColor(ImPlotCol_Line, cat_color); 
			ImPlot::PlotLine("##leader", line_x, line_y, 2);
			ImPlot::PopStyleColor();

			// Apply category color to the text
			ImVec2 offset = ImVec2(cos_a > 0 ? 4.0f : -4.0f, 0.0f);
			ImPlot::PushStyleColor(ImPlotCol_InlayText, cat_color);
			ImPlot::PlotText(data.labels_ptr[i], line_x[1], line_y[1], offset);
			ImPlot::PopStyleColor();

			current_deg += slice_deg; 
		}
		ImPlot::EndPlot();
	}
	ImPlot::PopColormap();
}

void Diagrams::draw_bar_group(ImVec2 size)
{
	int32_t current_item = static_cast<int>(current_range);
	ImGui::SetNextItemWidth(160.0f);
	if (ImGui::Combo("Filter Period##Bar", &current_item, range_names.data(), (int)range_names.size()))
	{
		current_range = static_cast<TimeRange>(current_item);
	}

	CategoryData& data = precalculated_data[current_range];
	
	if (data.names.empty() || data.incomes.size() != data.names.size())
	{
		ImGui::Text("No data available.");
		return;
	}

	ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(200.0f, 10.0f));
	if (ImPlot::BeginPlot("##CategoryTotals", size, ImPlotFlags_NoLegend | ImPlotFlags_NoMenus))
	{
		ImPlot::SetupAxes("Amount (€)", "", ImPlotAxisFlags_None, ImPlotAxisFlags_NoTickLabels);
		ImPlot::SetupAxisTicks(ImAxis_Y1, data.positions.data(), (int)data.positions.size(), nullptr);
		ImPlot::SetupAxisLimits(ImAxis_Y1, -0.7, (double)data.positions.size() - 0.3, ImGuiCond_Always);

		for (size_t i = 0; i < data.names.size(); ++i)
		{
			double pos = data.positions[i];

			if (data.incomes[i] > 0.0)
			{
				ImPlot::SetNextFillStyle(ImVec4(0.1f, 0.8f, 0.1f, 1.0f));
				ImPlot::PlotBars("##In", &data.incomes[i], &pos, 1, 0.4f, ImPlotBarsFlags_Horizontal);
			}
			if (data.expenses[i] < 0.0)
			{
				ImPlot::SetNextFillStyle(ImVec4(0.9f, 0.1f, 0.1f, 1.0f));
				ImPlot::PlotBars("##Out", &data.expenses[i], &pos, 1, 0.4f, ImPlotBarsFlags_Horizontal);
			}

			// Draw net bar
			ImPlot::SetNextFillStyle(ImVec4(0.3f, 0.6f, 1.0f, 1.0f));
			ImPlot::PlotBars("##Net", &data.net[i], &pos, 1, 0.15f, ImPlotBarsFlags_Horizontal);

			// Labels color
			ImU32 solid_color = data.colors[i] | 0xFF000000;
			ImVec2 label_pos = ImPlot::PlotToPixels(ImPlot::GetPlotLimits().Min().x, pos);
			
			float text_width = ImGui::CalcTextSize(data.names[i].c_str()).x;
			label_pos.x -= (text_width + 10.0f); 
			label_pos.y -= ImGui::GetTextLineHeight() * 0.5f;

			ImPlot::GetPlotDrawList()->AddText(label_pos, solid_color, data.names[i].c_str());
		}

		// Tooltip logic
		if (ImPlot::IsPlotHovered())
		{
			ImPlotPoint mouse = ImPlot::GetPlotMousePos();
			int idx = static_cast<int>(std::round(mouse.y));
			
			if (idx >= 0 && idx < (int)data.names.size())
			{
				ImGui::BeginTooltip();
				ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(data.colors[idx]), "%s", data.names[idx].c_str());
				ImGui::Separator();
				ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "In:  %.2f €", data.incomes[idx]);
				ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Out: %.2f €", data.expenses[idx]);
				ImGui::Separator();
				ImGui::TextColored(ImVec4(0.3f, 0.6f, 1.0f, 1.0f), "Net: %.2f €", data.net[idx]);
				ImGui::EndTooltip();
			}
		}
		ImPlot::EndPlot();
	}
}