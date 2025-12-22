#include "transaction_table.hpp"
#include "imgui.h"
#include "transaction_model.hpp"

void TransactionTable::draw(const TransactionModel& transaction_model)
{
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 6.0f));
	constexpr ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

	if (ImGui::BeginTable("Transactions", 8, flags))
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
					if (field_name == TransactionFieldNames::Category) ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, transaction_model.get_category_colors().at(transaction->category).get_hex_color());
					else ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
					ImGui::Text("%s", transaction->get_field_view(field_name).c_str());
				}
				ImGui::TableSetColumnIndex(field_names.size());
				ImGui::Text("%s", std::dynamic_pointer_cast<TransactionGroup>(transaction_model.at(row)) ? "x" : " ");
			}
		}
		ImGui::EndTable();
	}
	ImGui::PopStyleVar();
}
