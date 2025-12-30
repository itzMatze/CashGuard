#include "transaction_table.hpp"
#include "imgui_internal.h"
#include "transaction_model.hpp"

void TransactionTable::draw(ImVec2 available_space, const TransactionModel& transaction_model)
{
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 6.0f));
	constexpr ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_NoHostExtendX;
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.0f , 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4( 0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4( 0.0f, 0.0f, 0.0f, 0.0f));

	const std::vector<std::string>& field_names = Transaction::get_field_names();
	// show additional group field
	if (ImGui::BeginTable("Transactions", field_names.size() + 1, flags, available_space))
	{
		ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(0, 0, 0, 255));
		ImGui::TableSetupScrollFreeze(0, 1);
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
				const std::shared_ptr<const Transaction> transaction = transaction_model.at(row);
				ImU32 background_color = IM_COL32(0, 0, 0, 255);
				const uint32_t intensity = std::min(uint64_t(std::abs(transaction->amount.value)) / 40ull + 20ull, 255ull);
				if (transaction->amount.is_negative()) background_color = IM_COL32(intensity, 0, 0, 150);
				else background_color = IM_COL32(0, intensity, 0, 150);
				bool hovered = false;
				bool selected = false;
				for (int32_t column = 0; column < field_names.size(); column++)
				{
					const std::string& field_name = field_names[column];
					ImGui::TableSetColumnIndex(column);
					if (field_name == TransactionFieldNames::Category) ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, transaction_model.get_category_colors().at(transaction->category).get_ImU32());
					else ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, background_color);
					// set up selection and highlighting
					if (column == 0)
					{
						selected = (row == selected_row);
						if (ImGui::Selectable(transaction->get_field_view(field_name).c_str(), selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap)) selected_row = row;
						hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
						ImGui::SameLine();
					}
					else ImGui::Text("%s", transaction->get_field_view(field_name).c_str());
				}
				ImGui::TableSetColumnIndex(field_names.size());
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, background_color);
				ImGui::Text("%s", std::dynamic_pointer_cast<const TransactionGroup>(transaction_model.at(row)) ? "x" : " ");
				if (selected || hovered)
				{
					ImGuiTable* table = ImGui::GetCurrentTable();
					ImVec2 min(ImGui::TableGetCellBgRect(table, 0).Min);
					// last column is the group column which is not contained in field_names
					ImVec2 max(ImGui::TableGetCellBgRect(table, field_names.size()).Max);
					constexpr float border_thickness = 4.0f;
					ImDrawList* dl = ImGui::GetWindowDrawList();
					ImU32 color = IM_COL32(0, 255, 255, 128);
					if (selected) color = IM_COL32(0, 255, 255, 255);
					dl->AddRect(ImVec2(min.x + border_thickness / 2.0f, min.y), ImVec2(max.x - border_thickness / 2.0f, max.y), color, 0.0f, 0, border_thickness);
				}
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
