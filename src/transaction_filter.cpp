#include "transaction_filter.hpp"
#include "category_model.hpp"
#include "imgui_internal.h"
#include "util/utils.hpp"

TransactionFilter::TransactionFilter()
{}

void TransactionFilter::init(const CategoryModel& category_model)
{
	for (const std::string& category : category_model.get_names())
	{
		if (category.empty()) continue;
		category_entries.push_back(CategoryEntry(category, category_model.get_colors().at(category), true));
	}
	reset();
}

void TransactionFilter::reset()
{
	search_phrases_table.init();
	ignore_phrases_table.init();
	for (CategoryEntry& entry : category_entries) entry.selected = true;
	date_range_table.init();
	amount_range_table.init();
}

DialogResult TransactionFilter::draw(const std::string& label)
{
	ImVec2 window_size(1000.0f, 800.0f);
	static constexpr float description_filter_relative_size = 0.2f;
	static constexpr float category_filter_relative_size = 0.4f;
	static constexpr float date_filter_relative_size = 0.2f;
	static constexpr float amount_filter_relative_size = 0.2f;
	ImGui::SeparatorText("Description Filter");
	ImGui::BeginChild("Search Phrases Child", ImVec2(window_size.x / 2.0f, window_size.y * description_filter_relative_size));
	search_phrases_table.draw("Search phrases");
	ImGui::EndChild();
	ImGui::SameLine();
	ImGui::BeginChild("Ignore Phrases Child", ImVec2(window_size.x / 2.0f, window_size.y * description_filter_relative_size));
	ignore_phrases_table.draw("Ignore phrases");
	ImGui::EndChild();
	ImGui::SeparatorText("Category Filter");
	ImGui::BeginChild("Category Child", ImVec2(window_size.x, window_size.y * category_filter_relative_size));
	draw_category_table("Category Table##" + label);
	ImGui::EndChild();
	if (ImGui::Button(("Select All##" + label).c_str()))
	{
		for (CategoryEntry& entry : category_entries) entry.selected = true;
	}
	ImGui::SameLine();
	if (ImGui::Button(("Deselect All##" + label).c_str()))
	{
		for (CategoryEntry& entry : category_entries) entry.selected = false;
	}
	ImGui::SeparatorText("Date Filter");
	ImGui::BeginChild("Date Child", ImVec2(window_size.x, window_size.y * date_filter_relative_size));
	date_range_table.draw("Date Ranges##" + label);
	ImGui::EndChild();
	ImGui::SeparatorText("Amount Filter");
	ImGui::BeginChild("Amount Child", ImVec2(window_size.x, window_size.y * amount_filter_relative_size));
	amount_range_table.draw("Amount Ranges##" + label);
	ImGui::EndChild();
	ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_O);
	if (ImGui::Button("OK##TransactionFilter"))
	{
		search_phrases_table.obtain_results();
		ignore_phrases_table.obtain_results();
		date_range_table.obtain_results();
		amount_range_table.obtain_results();
		return DialogResult::Accept;
	}
	ImGui::SameLine();
	ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_C);
	if (ImGui::Button("Cancel##TransactionFilter")) return DialogResult::Cancel;
	ImGui::SameLine();
	ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_R);
	if (ImGui::Button("Reset##TransactionFilter")) reset();
	return DialogResult::None;
}

bool TransactionFilter::check(const std::shared_ptr<const Transaction> transaction) const
{
	bool search_phrase_match = false;
	if (search_phrases_table.strings.empty()) search_phrase_match = true;
	for (const std::string& s : search_phrases_table.strings)
	{
		if (contains_substring_case_insensitive(transaction->description, s))
		{
			search_phrase_match = true;
			break;
		}
	}
	bool ignore_phrase_match = false;
	for (const std::string& s : ignore_phrases_table.strings)
	{
		if (contains_substring_case_insensitive(transaction->description, s))
		{
			ignore_phrase_match = true;
			break;
		}
	}
	bool category_match = false;
	if (transaction->category.empty()) category_match = true;
	else
	{
		for (const CategoryEntry& entry : category_entries)
		{
			if (entry.selected && entry.name == transaction->category)
			{
				category_match = true;
				break;
			}
		}
	}
	bool date_range_match = false;
	if (date_range_table.date_ranges.empty()) date_range_match = true;
	for (const DateRangeTable::DateRange& date_range : date_range_table.date_ranges)
	{
		if (transaction->date >= date_range.begin && transaction->date <= date_range.end )
		{
			date_range_match = true;
			break;
		}
	}
	bool amount_range_match = false;
	if (amount_range_table.amount_ranges.empty()) amount_range_match = true;
	for (const AmountRangeTable::AmountRange& amount_range : amount_range_table.amount_ranges)
	{
		if (transaction->amount.value >= amount_range.lower.value && transaction->amount.value <= amount_range.upper.value)
		{
			amount_range_match = true;
			break;
		}
	}
	return search_phrase_match && !ignore_phrase_match && category_match && date_range_match && amount_range_match;
}

void TransactionFilter::StringEditTable::init()
{
	strings.clear();
	input.init();
	selected_row = -1;
	opened_row = -1;
	set_focus = false;
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
				dl->AddRect(ImVec2(min.x + border_thickness, min.y + border_thickness), ImVec2(max.x - border_thickness, min.y + row_height - border_thickness), highlight_color, 0.0f, 0, border_thickness);
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

void TransactionFilter::draw_category_table(const std::string& label)
{
	constexpr ImVec2 padding(8.0f, 4.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, padding);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
	const float row_height = ImGui::GetFrameHeight() + padding.y * 2.0f;
	constexpr ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_NoHostExtendX;
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.0f , 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4( 0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4( 0.0f, 0.0f, 0.0f, 0.0f));

	if (ImGui::BeginTable(("Categories##Table" + label).c_str(), 1, flags))
	{
		ImGui::TableSetupScrollFreeze(0, 1);
		ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(0, 0, 0, 255));
		ImGui::TableSetupColumn(("Categories##" + label).c_str(), ImGuiTableColumnFlags_None);
		ImGui::TableHeadersRow();
		for (int32_t row = 0; row < category_entries.size(); row++)
		{
			ImGui::TableNextRow(ImGuiTableRowFlags_None, row_height);
			ImGui::TableSetColumnIndex(0);
			CategoryEntry& entry = category_entries[row];
			// set up selection and highlighting
			if (ImGui::Selectable((entry.name + "##" + std::to_string(row)).c_str(), entry.selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap | ImGuiSelectableFlags_DontClosePopups)) entry.selected = !entry.selected;
			if (entry.selected)
			{
				ImU32 highlight_color = IM_COL32(0, 255, 255, 128);
				if (entry.selected) highlight_color = IM_COL32(0, 255, 255, 255);
				ImGuiTable* table = ImGui::GetCurrentTable();
				ImVec2 min(ImGui::TableGetCellBgRect(table, 0).Min);
				ImVec2 max(ImGui::TableGetCellBgRect(table, 0).Max);
				constexpr float border_thickness = 4.0f;
				ImDrawList* dl = ImGui::GetWindowDrawList();
				dl->AddRect(ImVec2(min.x + border_thickness, min.y + border_thickness), ImVec2(max.x - border_thickness, min.y + row_height - border_thickness), highlight_color, 0.0f, 0, border_thickness);
			}
		}
		ImGui::EndTable();
	}
	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar(2);
}

void TransactionFilter::DateRangeTable::init()
{
	date_ranges.clear();
	begin_input.init();
	end_input.init();
	active = false;
	selected_row = -1;
	opened_row = -1;
	set_focus = false;
}

void TransactionFilter::DateRangeTable::draw(const std::string& label)
{
	constexpr ImVec2 padding(8.0f, 6.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, padding);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, padding);
	const float row_height = ImGui::GetFrameHeight() + padding.y * 2.0f;
	constexpr ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable | ImGuiTableFlags_NoHostExtendX;
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.0f , 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4( 0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4( 0.0f, 0.0f, 0.0f, 0.0f));

	if (ImGui::BeginTable(("Date Ranges" + label).c_str(), 2, flags))
	{
		ImGui::TableSetupScrollFreeze(0, 1);
		ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(0, 0, 0, 255));
		ImGui::TableSetupColumn(label.c_str(), ImGuiTableColumnFlags_None);
		ImGui::TableHeadersRow();
		for (int32_t row = 0; row < date_ranges.size(); row++)
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
				if (begin_input.draw("##BeginInput" + label + std::to_string(row), "", set_focus))
				{
					date_ranges[opened_row].begin = begin_input.get_result();
				}
				set_focus = false;
			}
			else
			{
				ImGui::AlignTextToFramePadding();
				if (ImGui::Selectable((DateUtils::to_string(date_ranges[row].begin) + "##" + std::to_string(row)).c_str(), selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap | ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_DontClosePopups))
				{
					selected_row = row;
					opened_row = -1;
				}
				hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
				if (hovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					opened_row = row;
					begin_input.init(date_ranges[opened_row].begin);
					end_input.init(date_ranges[opened_row].end);
				}
			}
			ImGui::TableSetColumnIndex(1);
			if (is_edited)
			{
				ImGui::SetNextItemWidth(-FLT_MIN);
				if (end_input.draw("##EndInput" + label + std::to_string(row)))
				{
					date_ranges[row].end = end_input.get_result();
				}
			}
			else
			{
				ImGui::AlignTextToFramePadding();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetStyle().ItemInnerSpacing.x);
				ImGui::Text("%s", DateUtils::to_string(date_ranges[row].end).c_str());
			}
			if (lost_active(begin_input.is_active() || end_input.is_active(), active)) opened_row = -1;
			if (selected || hovered)
			{
				ImU32 highlight_color = IM_COL32(0, 255, 255, 128);
				if (selected) highlight_color = IM_COL32(0, 255, 255, 255);
				ImGuiTable* table = ImGui::GetCurrentTable();
				ImVec2 min(ImGui::TableGetCellBgRect(table, 0).Min);
				ImVec2 max(ImGui::TableGetCellBgRect(table, 1).Max);
				constexpr float border_thickness = 4.0f;
				ImDrawList* dl = ImGui::GetWindowDrawList();
				dl->AddRect(ImVec2(min.x + border_thickness, min.y + border_thickness), ImVec2(max.x - border_thickness, min.y + row_height - border_thickness), highlight_color, 0.0f, 0, border_thickness);
			}
		}
		ImGui::EndTable();
	}
	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar(2);
	if (ImGui::Button(("Add##" + label).c_str()))
	{
		selected_row = date_ranges.size();
		opened_row = date_ranges.size();
		date_ranges.push_back(DateRange(DateUtils::to_date(Clock::now()), DateUtils::to_date(Clock::now())));
		begin_input.init(date_ranges[opened_row].begin);
		end_input.init(date_ranges[opened_row].end);
		set_focus = true;
	}
	ImGui::SameLine();
	if (ImGui::Button(("Remove##" + label).c_str()) && selected_row >= 0 && selected_row < date_ranges.size())
	{
		opened_row = -1;
		date_ranges.erase(date_ranges.begin() + selected_row);
	}
}

void TransactionFilter::DateRangeTable::obtain_results()
{
	if (opened_row >= 0 && opened_row < date_ranges.size())
	{
		date_ranges[opened_row].begin = begin_input.get_result();
		date_ranges[opened_row].end = end_input.get_result();
	}
	opened_row = -1;
}

void TransactionFilter::AmountRangeTable::init()
{
	amount_ranges.clear();
	lower_input.init();
	upper_input.init();
	active = false;
	selected_row = -1;
	opened_row = -1;
	set_focus = false;
}

void TransactionFilter::AmountRangeTable::draw(const std::string& label)
{
	constexpr ImVec2 padding(8.0f, 6.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, padding);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, padding);
	const float row_height = ImGui::GetFrameHeight() + padding.y * 2.0f;
	constexpr ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable | ImGuiTableFlags_NoHostExtendX;
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.0f , 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4( 0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4( 0.0f, 0.0f, 0.0f, 0.0f));

	if (ImGui::BeginTable(("Date Ranges" + label).c_str(), 2, flags))
	{
		ImGui::TableSetupScrollFreeze(0, 1);
		ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(0, 0, 0, 255));
		ImGui::TableSetupColumn(label.c_str(), ImGuiTableColumnFlags_None);
		ImGui::TableHeadersRow();
		for (int32_t row = 0; row < amount_ranges.size(); row++)
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
				if (lower_input.draw("##LowerInput" + label + std::to_string(row), "", set_focus))
				{
					amount_ranges[opened_row].lower = lower_input.get_result();
				}
				set_focus = false;
			}
			else
			{
				ImGui::AlignTextToFramePadding();
				if (ImGui::Selectable((amount_ranges[row].lower.to_string_view() + "##" + std::to_string(row)).c_str(), selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap | ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_DontClosePopups))
				{
					selected_row = row;
					opened_row = -1;
				}
				hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
				if (hovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					opened_row = row;
					lower_input.init(amount_ranges[opened_row].lower);
					upper_input.init(amount_ranges[opened_row].upper);
				}
			}
			ImGui::TableSetColumnIndex(1);
			if (is_edited)
			{
				ImGui::SetNextItemWidth(-FLT_MIN);
				if (upper_input.draw("##UpperInput" + label + std::to_string(row)))
				{
					amount_ranges[row].upper = upper_input.get_result();
				}
			}
			else
			{
				ImGui::AlignTextToFramePadding();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetStyle().ItemInnerSpacing.x);
				ImGui::Text("%s", amount_ranges[row].upper.to_string_view().c_str());
			}
			if (lost_active(lower_input.is_active() || upper_input.is_active(), active)) opened_row = -1;
			if (selected || hovered)
			{
				ImU32 highlight_color = IM_COL32(0, 255, 255, 128);
				if (selected) highlight_color = IM_COL32(0, 255, 255, 255);
				ImGuiTable* table = ImGui::GetCurrentTable();
				ImVec2 min(ImGui::TableGetCellBgRect(table, 0).Min);
				ImVec2 max(ImGui::TableGetCellBgRect(table, 1).Max);
				constexpr float border_thickness = 4.0f;
				ImDrawList* dl = ImGui::GetWindowDrawList();
				dl->AddRect(ImVec2(min.x + border_thickness, min.y + border_thickness), ImVec2(max.x - border_thickness, min.y + row_height - border_thickness), highlight_color, 0.0f, 0, border_thickness);
			}
		}
		ImGui::EndTable();
	}
	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar(2);
	if (ImGui::Button(("Add##" + label).c_str()))
	{
		selected_row = amount_ranges.size();
		opened_row = amount_ranges.size();
		amount_ranges.push_back(AmountRange(Amount(), Amount()));
		lower_input.init(amount_ranges[opened_row].lower);
		upper_input.init(amount_ranges[opened_row].upper);
		set_focus = true;
	}
	ImGui::SameLine();
	if (ImGui::Button(("Remove##" + label).c_str()) && selected_row >= 0 && selected_row < amount_ranges.size())
	{
		opened_row = -1;
		amount_ranges.erase(amount_ranges.begin() + selected_row);
	}
}

void TransactionFilter::AmountRangeTable::obtain_results()
{
	if (opened_row >= 0 && opened_row < amount_ranges.size())
	{
		amount_ranges[opened_row].lower = lower_input.get_result();
		amount_ranges[opened_row].upper = upper_input.get_result();
	}
	opened_row = -1;
}
