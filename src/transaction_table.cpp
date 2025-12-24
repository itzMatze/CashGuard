#include "transaction_table.hpp"
#include "imgui_internal.h"
#include "transaction_model.hpp"

void TransactionTable::draw(ImVec2 available_space, const TransactionModel& transaction_model)
{
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 6.0f));
	constexpr ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.0f , 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4( 0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4( 0.0f, 0.0f, 0.0f, 0.0f));

	if (ImGui::BeginTable("Transactions", 8, flags, available_space))
	{
		ImGui::TableSetupScrollFreeze(0, 1);
		const std::vector<std::string>& field_names = Transaction::get_field_names();
		for (const std::string& field_name : field_names)
		{
			ImGui::TableSetupColumn(field_name.c_str(), ImGuiTableColumnFlags_None);
		}
		ImGui::TableSetupColumn("Group", ImGuiTableColumnFlags_None);
		ImGui::TableHeadersRow();
		ImGuiListClipper clipper;
		clipper.Begin(transaction_model.count());
		while (clipper.Step())
		{
			for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
			{
				ImGui::TableNextRow();
				for (int32_t column = 0; column < field_names.size(); column++)
				{
					const std::string& field_name = field_names[column];
					const std::shared_ptr<Transaction> transaction = transaction_model.at(row);
					ImGui::TableSetColumnIndex(column);
					ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(0, 0, 0, 0));
					ImU32 color = IM_COL32(0, 0, 0, 255);
					const uint32_t intensity = std::min(uint64_t(std::abs(transaction->amount.value)) / 40ull + 20ull, 255ull);
					if (transaction->amount.is_negative()) color = IM_COL32(intensity, 0, 0, 150);
					else color = IM_COL32(0, intensity, 0, 150);
					if (field_name == TransactionFieldNames::Category) ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, transaction_model.get_category_colors().at(transaction->category).get_ImU32());
					else ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
					// set up selection and highlighting
					if (column == 0)
					{
						ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
						ImGuiTable* table = ImGui::GetCurrentTable();
						ImVec2 upper_left(ImGui::TableGetCellBgRect(table, 0).Min.x, cursor_pos.y - ImGui::GetStyle().ItemInnerSpacing.y);
						// last column is the group column which is not contained in field_names
						ImVec2 lower_right(ImGui::TableGetCellBgRect(table, field_names.size()).Max.x, cursor_pos.y + ImGui::GetTextLineHeight() + ImGui::GetStyle().ItemInnerSpacing.y);
						bool selected = (row == selected_row);
						if (ImGui::Selectable(std::string("##row_" + std::to_string(row)).c_str(), selected, ImGuiSelectableFlags_SpanAllColumns)) selected_row = row;
						bool hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
						if (selected || hovered)
						{
							constexpr float border_thickness = 4.0f;
							ImDrawList* dl = ImGui::GetWindowDrawList();
							ImU32 col = ImGui::GetColorU32(ImGuiCol_HeaderActive);
							ImU32 color = IM_COL32(0, 255, 255, 128);
							if (selected) color = IM_COL32(0, 255, 255, 255);
							dl->AddRect(ImVec2(upper_left.x + border_thickness / 2.0f, upper_left.y), ImVec2(lower_right.x - border_thickness / 2.0f, lower_right.y), color, 0.0f, 0, 4.0f);
						}
						ImGui::SameLine();
					}
					ImGui::Text("%s", transaction->get_field_view(field_name).c_str());
				}
				ImGui::TableSetColumnIndex(field_names.size());
				ImGui::Text("%s", std::dynamic_pointer_cast<TransactionGroup>(transaction_model.at(row)) ? "x" : " ");
			}
		}
		ImGui::EndTable();
	}
	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar();
}

int32_t TransactionTable::get_selected_row() const
{
	return selected_row;
}
