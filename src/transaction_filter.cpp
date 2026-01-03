#include "transaction_filter.hpp"
#include "imgui_internal.h"
#include "util/utils.hpp"

TransactionFilter::TransactionFilter()
{
}

DialogResult TransactionFilter::draw(const std::string& label)
{
	ImVec2 child_size(500.0f, 500.0f);
	ImGui::BeginChild("Search Phrases Child", child_size);
	search_phrases_table.draw("Search phrases");
	ImGui::EndChild();
	ImGui::SameLine();
	ImGui::BeginChild("Ignore Phrases Child", child_size);
	ignore_phrases_table.draw("Ignore phrases");
	ImGui::EndChild();
	ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_O);
	if (ImGui::Button("OK##TransactionDialog"))
	{
		search_phrases_table.obtain_results();
		ignore_phrases_table.obtain_results();
		return DialogResult::Accept;
	}
	ImGui::SameLine();
	ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_C);
	if (ImGui::Button("Cancel##TransactionDialog")) return DialogResult::Cancel;
	return DialogResult::None;
}

bool TransactionFilter::check(const std::shared_ptr<const Transaction> transaction) const
{
	bool valid = false;
	if (search_phrases_table.strings.empty()) valid = true;
	for (const std::string& s : search_phrases_table.strings)
	{
		if (contains_substring_case_insensitive(transaction->description, s))
		{
			valid = true;
			break;
		}
	}
	for (const std::string& s : ignore_phrases_table.strings)
	{
		if (contains_substring_case_insensitive(transaction->description, s))
		{
			valid = false;
			break;
		}
	}
	return valid;
}

void TransactionFilter::StringEditTable::draw(const std::string& label)
{
	constexpr ImVec2 padding(8.0f, 6.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, padding);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, padding);
	const float row_height = ImGui::GetFrameHeight() + padding.y * 2.0f;
	constexpr ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_NoHostExtendX;
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.0f , 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4( 0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4( 0.0f, 0.0f, 0.0f, 0.0f));

	if (ImGui::BeginTable(("Strings" + label).c_str(), 1, flags))
	{
		ImGui::TableSetupScrollFreeze(0, 1);
		ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(0, 0, 0, 255));
		ImGui::TableSetupColumn(label.c_str(), ImGuiTableColumnFlags_None);
		ImGui::TableHeadersRow();
		for (int32_t row = 0; row < strings.size(); row++)
		{
			ImGui::TableNextRow(ImGuiTableRowFlags_None, row_height);
			ImGui::TableSetColumnIndex(0);
			// set up selection and highlighting
			bool selected = (row == selected_row);
			bool hovered = false;
			const bool is_edited = row == opened_row && opened_row == selected_row;
			if (is_edited)
			{
				ImGui::SetNextItemWidth(-FLT_MIN);
				if (input.draw("##StringInput" + label + std::to_string(row), "", set_focus))
				{
					strings[opened_row] = input.get_result();
					opened_row = -1;
				}
				set_focus = false;
			}
			else
			{
				ImGui::AlignTextToFramePadding();
				if (ImGui::Selectable((strings[row] + "##" + std::to_string(row)).c_str(), selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap | ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_DontClosePopups))
				{
					selected_row = row;
					opened_row = -1;
				}
				hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
				if (hovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					opened_row = row;
					input.init(strings[opened_row]);
				}
			}
			if (selected || hovered)
			{
				ImU32 highlight_color = IM_COL32(0, 255, 255, 128);
				if (selected) highlight_color = IM_COL32(0, 255, 255, 255);
				ImGuiTable* table = ImGui::GetCurrentTable();
				ImVec2 min(ImGui::TableGetCellBgRect(table, 0).Min);
				ImVec2 max(ImGui::TableGetCellBgRect(table, 0).Max);
				constexpr float border_thickness = 4.0f;
				ImDrawList* dl = ImGui::GetWindowDrawList();
				dl->AddRect(ImVec2(min.x + border_thickness / 2.0f, min.y), ImVec2(max.x - border_thickness / 2.0f, min.y + row_height), highlight_color, 0.0f, 0, border_thickness);
			}
		}
		ImGui::EndTable();
	}
	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar(2);
	if (ImGui::Button(("Add##" + label).c_str()))
	{
		input.init();
		selected_row = strings.size();
		opened_row = strings.size();
		strings.push_back("");
		set_focus = true;
	}
	ImGui::SameLine();
	if (ImGui::Button(("Remove##" + label).c_str()) && selected_row >= 0 && selected_row < strings.size())
	{
		opened_row = -1;
		strings.erase(strings.begin() + selected_row);
	}
}

void TransactionFilter::StringEditTable::obtain_results()
{
	if (opened_row >= 0 && opened_row < strings.size()) strings[opened_row] = input.get_result();
	opened_row = -1;
}
