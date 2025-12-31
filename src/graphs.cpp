#include "graphs.hpp"
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
	unix_seconds_now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::sys_days{to_date(Clock::now())}.time_since_epoch()).count();
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
}

void TotalAmountGraph::draw_small_graph(ImVec2 available_space)
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
			if (ImPlot::IsPlotHovered())
			{
				ImPlotPoint mouse = ImPlot::GetPlotMousePos();
				// find mouse location index
				int32_t idx = binary_search_less_equal(time_points.cbegin(), time_points.cend(), int64_t(mouse.x));
				if (idx != -1)
				{
					ImGui::BeginTooltip();
					std::string date = std::format("{:%d.%m.%Y}", Date(to_date(DateTime{std::chrono::seconds{int64_t(mouse.x)}})));
					ImGui::Text("%s", date.c_str());
					ImGui::Text("%s", Amount(int64_t(data_points[idx] * 100.0)).to_string_view().c_str());
					ImGui::EndTooltip();
					draw_list->AddCircleFilled(ImVec2(ImPlot::PlotToPixels(mouse.x, data_points[idx])), 6.0f, IM_COL32(255, 0, 0, 255));
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
	ImPlot::PopStyleColor(2);
	ImPlot::PopStyleVar(2);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetTextLineHeight());
}
