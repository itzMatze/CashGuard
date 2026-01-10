#include "transaction_table.hpp"
#include "category_model.hpp"
#include "imgui_internal.h"
#include "transaction_model.hpp"

void TransactionTable::draw(ImVec2 available_space, const TransactionModel& transaction_model, const CategoryModel& category_model, bool show_amounts)
{
	constexpr ImVec2 padding(8.0f, 6.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, padding);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
	const float row_height = ImGui::GetFrameHeight() + padding.y * 2.0f;
	constexpr ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_NoHostExtendX;
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.0f , 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4( 0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4( 0.0f, 0.0f, 0.0f, 0.0f));

	// show additional group field
	if (ImGui::BeginTable("Transactions", TRANSACTION_FIELD_COUNT + 1, flags, available_space))
	{
		ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(0, 0, 0, 255));
		ImGui::TableSetupScrollFreeze(0, 1);
		for (int32_t i = 0; i < TRANSACTION_FIELD_COUNT; i++)
		{
			ImGui::TableSetupColumn(std::to_string(i).c_str(), ImGuiTableColumnFlags_None);
		}
		ImGui::TableSetupColumn("Group", ImGuiTableColumnFlags_None);
		ImGui::TableHeadersRow();
		ImGuiListClipper clipper;
		clipper.Begin(transaction_model.count());
		while (clipper.Step())
		{
			for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
			{
				ImGui::TableNextRow(ImGuiTableRowFlags_None, row_height);
				const std::shared_ptr<const Transaction> transaction = transaction_model.at(row);
				ImU32 background_color = IM_COL32(0, 0, 0, 255);
				const uint32_t intensity = std::min(uint64_t(std::abs(transaction->amount.value)) / 40ull + 20ull, 255ull);
				if (transaction->amount.is_negative()) background_color = IM_COL32(intensity, 0, 0, 150);
				else background_color = IM_COL32(0, intensity, 0, 150);
				bool hovered = false;
				bool selected = false;
				ImGui::TableSetColumnIndex(0);
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, background_color);
				selected = (row == selected_row);
				if (ImGui::Selectable(std::to_string(transaction->id).c_str(), selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap))
				{
					selected_row = row;
					selected_transaction = transaction;
				}
				hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
				ImGui::TableSetColumnIndex(1);
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, background_color);
				ImGui::Text("%s", DateUtils::to_string(transaction->date).c_str());
				ImGui::TableSetColumnIndex(2);
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, category_model.get_category(transaction->category_id).color.get_ImU32());
				ImGui::Text("%s", category_model.get_category(transaction->category_id).name.c_str());
				ImGui::TableSetColumnIndex(3);
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, background_color);
				if (show_amounts) ImGui::Text("%s", transaction->amount.to_string_view().c_str());
				else ImGui::Text(" X €");
				ImGui::TableSetColumnIndex(4);
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, background_color);
				ImGui::Text("%s", transaction->description.c_str());
				ImGui::TableSetColumnIndex(5);
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, background_color);
				ImGui::Text("%s", DateUtils::to_string(transaction->added).c_str());
				ImGui::TableSetColumnIndex(6);
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, background_color);
				ImGui::Text("%s", DateUtils::to_string(transaction->edited).c_str());
				ImGui::TableSetColumnIndex(7);
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, background_color);
				ImGui::Text("%s", std::dynamic_pointer_cast<const TransactionGroup>(transaction_model.at(row)) ? "x" : " ");
				if (selected || hovered)
				{
					ImGuiTable* table = ImGui::GetCurrentTable();
					ImVec2 min(ImGui::TableGetCellBgRect(table, 0).Min);
					// last column is the group column which is not contained in field_names
					ImVec2 max(ImGui::TableGetCellBgRect(table, TRANSACTION_FIELD_COUNT).Max);
					constexpr float border_thickness = 4.0f;
					ImDrawList* dl = ImGui::GetWindowDrawList();
					ImU32 highlight_color = IM_COL32(0, 255, 255, 128);
					if (selected)
					{
						selected_transaction = transaction;
						highlight_color = IM_COL32(0, 255, 255, 255);
					}
					dl->AddRect(ImVec2(min.x + border_thickness, min.y + border_thickness), ImVec2(max.x - border_thickness, min.y + row_height - border_thickness), highlight_color, 0.0f, 0, border_thickness);
				}
			}
		}
		ImGui::EndTable();
	}
	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar(2);
}

int32_t TransactionTable::get_selected_row() const
{
	return selected_row;
}

std::shared_ptr<const Transaction> TransactionTable::get_selected_transaction() const
{
	return selected_transaction;
}
