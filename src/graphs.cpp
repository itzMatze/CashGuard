#include "graphs.hpp"
#include "implot.h"

constexpr int64_t year_second_count = 31536000;

void SmallTotalAmountGraph::update_data(const TransactionModel& transaction_model)
{
	int64_t previous_amount = 0;
	int64_t amount = 0;
	min_amount = std::numeric_limits<double>::max();
	max_amount = std::numeric_limits<double>::lowest();
	time_points.clear();
	data_points.clear();
	unix_seconds_now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::sys_days{to_date(Clock::now())}.time_since_epoch()).count();
	for (int32_t i = transaction_model.count() - 1; i >= 0; i--)
	{
		std::shared_ptr<const Transaction> transaction = transaction_model.at(i);
		amount += transaction->amount.value;
		int64_t unix_seconds = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::sys_days{transaction->date}.time_since_epoch()).count();
		// only plot if not older than a year
		if (unix_seconds_now - unix_seconds <= year_second_count)
		{
			// draw line to old value to prevent diagonal lines
			time_points.push_back(unix_seconds);
			time_points.push_back(unix_seconds);
			data_points.push_back(double(previous_amount) / 100.0f);
			double amount_data = double(amount) / 100.0f;
			data_points.push_back(amount_data);
			min_amount = std::min(min_amount, amount_data);
			max_amount = std::max(max_amount, amount_data);
		}
		previous_amount = amount;
	}
	// make sure the min and max amount are visible
	min_amount -= (std::abs(min_amount) * 0.05f);
	max_amount += (std::abs(max_amount) * 0.05f);
}

void SmallTotalAmountGraph::draw(ImVec2 available_space)
{
	ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0.0f, 0.0f));
	ImPlot::PushStyleVar(ImPlotStyleVar_LabelPadding, ImVec2(0.0f, 0.0f));
	// keep space for custom drawn labels
	available_space.y -= ImGui::GetTextLineHeight();
	if (ImPlot::BeginPlot("##Small Total Amount Plot", available_space, ImPlotFlags_NoInputs | ImPlotFlags_NoLegend | ImPlotFlags_NoMenus))
	{
		ImPlot::SetupAxes("##Time", "##Amount", ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_NoTickLabels, ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_NoTickLabels);
		ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
		ImPlot::SetupAxesLimits(double(unix_seconds_now - year_second_count), double(unix_seconds_now), min_amount, max_amount, ImGuiCond_Always);
		ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(0.0f, 1.0f, 1.0f, 1.0f));
		ImPlot::PlotLine( "##Amount", time_points.data(), data_points.data(), time_points.size());
		ImPlot::PopStyleColor();
		// draw labels
		ImDrawList* draw_list = ImPlot::GetPlotDrawList();
		Date current_date = to_date(Clock::now());
		std::chrono::year_month current_month_date = current_date.year() / current_date.month();
		for (int32_t i = 11; i >= 0; i--)
		{
			Date first_day{(current_month_date - std::chrono::months{i}) / std::chrono::day{1}};
			std::chrono::sys_seconds tp{std::chrono::sys_days{first_day}};
			double x = tp.time_since_epoch().count();
			double y = min_amount;
			ImVec2 screen_pos = ImPlot::PlotToPixels(x, y);
			draw_list->AddText(ImVec2(screen_pos.x + 2, screen_pos.y), IM_COL32(255, 255, 255, 255), std::format("{:%B}", tp).c_str());
		}
		ImPlot::EndPlot();
	}
	ImPlot::PopStyleVar(2);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetTextLineHeight());
}
