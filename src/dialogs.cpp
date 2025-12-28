#include "dialogs.hpp"
#include "imgui_internal.h"
#include "util/random_generator.hpp"

void TransactionDialog::init(const TransactionModel& transaction_model, const Transaction& transaction)
{
	this->transaction = transaction;
	date_input.init(transaction.date);
	description_input.init(transaction.description);
	amount_input.init(transaction.amount);
	category_dropdown.init(transaction_model.get_category_names(), transaction.category);
}

DialogResult TransactionDialog::draw(const std::string& label)
{
	if (date_input.draw("##TransactionDialogDate")) transaction.date = date_input.get_result();
	if (description_input.draw("##TransactionDialogDescription", "Description")) transaction.description = description_input.get_result();
	if (amount_input.draw("##TransactionDialogAmount")) transaction.amount = amount_input.get_result();
	if (category_dropdown.draw("##TransactionDialogCategory")) transaction.category = category_dropdown.get_result_string();
	if (ImGui::Button("OK##TransactionDialog")) return DialogResult::Accept;
	ImGui::SameLine();
	if (ImGui::Button("Cancel##TransactionDialog")) return DialogResult::Cancel;
	return DialogResult::None;
}

Transaction TransactionDialog::get_transaction() const
{
	return transaction;
}

void TransactionGroupDialog::init(const TransactionModel& transaction_model, const TransactionGroup& transaction_group)
{
	this->transaction_group = transaction_group;
	date_input.init(transaction_group.date);
	description_input.init(transaction_group.description);
	category_dropdown.init(transaction_model.get_category_names(), transaction_group.category);
}

DialogResult TransactionGroupDialog::draw(const std::string& label, const TransactionModel& transaction_model)
{
	ImGui::PushFont(NULL, 32.0f);
	ImGui::Text(" %s", transaction_group.amount.to_string_view().c_str());
	ImGui::PopFont();
	if (date_input.draw("##TransactionGroupDialogDate")) transaction_group.date = date_input.get_result();
	if (description_input.draw("##TransactionGroupDialogDescription", "Description")) transaction_group.description = description_input.get_result();
	if (category_dropdown.draw("##TransactionGroupDialogCategory")) transaction_group.category = category_dropdown.get_result_string();
	ImGui::SeparatorText("Transactions");
	draw_transaction_table(transaction_model);
	const bool row_valid = selected_group_row > -1 && selected_group_row < transaction_group.get_transactions().size();

	// Add
	if (ImGui::Button("Add##TransactionGroupDialog"))
	{
		Transaction new_transaction = Transaction();
		new_transaction.category = transaction_group.category;
		new_transaction.date = transaction_group.date;
		member_dialog.init(transaction_model, new_transaction);
		ImGui::OpenPopup("Transaction Add##MemberDialog");
	}
	if (ImGui::BeginPopupModal("Transaction Add##MemberDialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		DialogResult result = member_dialog.draw("Member Transaction Dialog");
		if (result == DialogResult::Accept)
		{
			Transaction new_transaction = member_dialog.get_transaction();
			new_transaction.id = rng::random_int64();
			new_transaction.added = DateTime(Clock::now());
			new_transaction.edited = DateTime(Clock::now());
			transaction_group.add_transaction(new_transaction);
			ImGui::CloseCurrentPopup();
		}
		else if (result == DialogResult::Cancel) ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
	ImGui::SameLine();

	// Edit
	if (ImGui::Button("Edit##TransactionGroupDialog") && row_valid)
	{
		member_dialog.init(transaction_model, transaction_group.get_transactions()[selected_group_row]);
		ImGui::OpenPopup("Transaction Edit##MemberDialog");
	}
	if (ImGui::BeginPopupModal("Transaction Edit##MemberDialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		DialogResult result = member_dialog.draw("Member Transaction Dialog");
		if (result == DialogResult::Accept)
		{
			Transaction new_transaction = member_dialog.get_transaction();
			new_transaction.edited = DateTime(Clock::now());
			transaction_group.set_transaction(selected_group_row, new_transaction);
			ImGui::CloseCurrentPopup();
		}
		else if (result == DialogResult::Cancel) ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
	ImGui::SameLine();

	// Remove
	if (ImGui::Button("Remove##TransactionGroupDialog") && row_valid) ImGui::OpenPopup("Transaction Remove##MemberDialog");
	if (ImGui::BeginPopupModal("Transaction Remove##MemberDialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Remove transaction with id %zu?", transaction_group.get_transactions()[selected_group_row].id);
		if (ImGui::Button("OK##TransactionGroupDialogRemove"))
		{
			transaction_group.remove_transaction(selected_group_row);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel##TransactionGroupDialogRemove")) ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
	if (ImGui::Button("OK##TransactionGroupDialog")) return DialogResult::Accept;
	ImGui::SameLine();
	if (ImGui::Button("Cancel##TransactionGroupDialog")) return DialogResult::Cancel;
	return DialogResult::None;
}

TransactionGroup TransactionGroupDialog::get_transaction_group() const
{
	return transaction_group;
}

void TransactionGroupDialog::draw_transaction_table(const TransactionModel& transaction_model)
{
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 6.0f));
	constexpr ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_NoHostExtendX;
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.0f , 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4( 0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4( 0.0f, 0.0f, 0.0f, 0.0f));
	const std::vector<std::string>& field_names = Transaction::get_field_names();
	if (ImGui::BeginTable("Transactions##TransactionGroupDialog", field_names.size(), flags))
	{
		ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(0, 0, 0, 255));
		ImGui::TableSetupScrollFreeze(0, 1);
		for (const std::string& field_name : field_names)
		{
			ImGui::TableSetupColumn(field_name.c_str(), ImGuiTableColumnFlags_None);
		}
		ImGui::TableHeadersRow();
		for (int32_t row = 0; row < transaction_group.get_transactions().size(); row++)
		{
			ImGui::TableNextRow(ImGuiTableRowFlags_None);
			const Transaction& transaction = transaction_group.get_transactions()[row];
			ImU32 background_color = IM_COL32(0, 0, 0, 255);
			const uint32_t intensity = std::min(uint64_t(std::abs(transaction.amount.value)) / 40ull + 20ull, 255ull);
			if (transaction.amount.is_negative()) background_color = IM_COL32(intensity, 0, 0, 150);
			else background_color = IM_COL32(0, intensity, 0, 150);
			bool hovered = false;
			bool selected = false;
			for (int32_t column = 0; column < field_names.size(); column++)
			{
				const std::string& field_name = field_names[column];
				ImGui::TableSetColumnIndex(column);
				if (field_name == TransactionFieldNames::Category) ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, transaction_model.get_category_colors().at(transaction.category).get_ImU32());
				else ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, background_color);
				// set up selection and highlighting
				if (column == 0)
				{
					selected = (row == selected_group_row);
					if (ImGui::Selectable(transaction.get_field_view(field_name).c_str(), selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap | ImGuiSelectableFlags_DontClosePopups)) selected_group_row = row;
					hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
				}
				else ImGui::Text("%s", transaction.get_field_view(field_name).c_str());
			}
			if (selected || hovered)
			{
				ImU32 highlight_color = IM_COL32(0, 255, 255, 128);
				if (selected) highlight_color = IM_COL32(0, 255, 255, 255);
				ImGuiTable* table = ImGui::GetCurrentTable();
				ImVec2 min(ImGui::TableGetCellBgRect(table, 0).Min);
				// last column is the group column which is not contained in field_names
				ImVec2 max(ImGui::TableGetCellBgRect(table, field_names.size() - 1).Max);
				constexpr float border_thickness = 4.0f;
				ImDrawList* dl = ImGui::GetWindowDrawList();
				dl->AddRect(ImVec2(min.x + border_thickness / 2.0f, min.y), ImVec2(max.x - border_thickness / 2.0f, max.y), highlight_color, 0.0f, 0, border_thickness);
			}
		}
		ImGui::EndTable();
	}
	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar();
}
