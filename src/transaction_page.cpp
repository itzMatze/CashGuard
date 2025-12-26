#include "transaction_page.hpp"
#include "account_model.hpp"
#include "imgui_internal.h"
#include "transaction_model.hpp"
#include "util/random_generator.hpp"

void TransactionPage::draw(ImVec2 available_space, TransactionModel& transaction_model, AccountModel& account_model)
{
	const int32_t row_index = transaction_table.get_selected_row();
	const bool row_valid = row_index > -1 && row_index < transaction_model.count();
	ImGui::PushFont(NULL, 64.0f);
	available_space.y -= ImGui::GetTextLineHeight() + ImGui::GetStyle().ItemInnerSpacing.y * 2.0f;
	ImGui::Text(" %s €", transaction_model.get_filtered_total_amount().to_string().c_str());
	ImGui::PopFont();
	constexpr float table_relative_height = 0.95f;
	transaction_table.draw(ImVec2(available_space.x, available_space.y * table_relative_height), transaction_model);
	constexpr int32_t button_count = 5;
	ImVec2 button_size(available_space.x * (1.0f / float(button_count)) - ImGui::GetStyle().ItemInnerSpacing.x * float(button_count - 1) / float(button_count), available_space.y * (1.0f - table_relative_height));
	constexpr ImVec4 button_color(0.0f, 0.4f, 0.4f, 1.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, button_color);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(button_color.x + 0.1f, button_color.y + 0.1f, button_color.z + 0.1f, button_color.w));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(button_color.x - 0.1f, button_color.y - 0.1f, button_color.z - 0.1f, button_color.w));

	// Add
	if (ImGui::Button("Add", button_size))
	{
		opened_transaction = Transaction();
		ImGui::OpenPopup("Transaction Add##Dialog");
	}
	if (ImGui::BeginPopupModal("Transaction Add##Dialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) transaction_dialog(transaction_model, -1);
	ImGui::SameLine();

	// Add Group
	if (ImGui::Button("Add Group", button_size))
	{
		opened_transaction_group = TransactionGroup();
		ImGui::OpenPopup("Transaction Group Add##Dialog");
	}
	if (ImGui::BeginPopupModal("Transaction Group Add##Dialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) transaction_group_dialog(transaction_model, -1);
	ImGui::SameLine();

	// Edit
	if (ImGui::Button("Edit", button_size) && row_valid)
	{
		const std::shared_ptr<Transaction> transaction = transaction_model.at(row_index);
		if (std::shared_ptr<TransactionGroup> transaction_group = std::dynamic_pointer_cast<TransactionGroup>(transaction))
		{
			opened_transaction_group = *transaction_group;
			ImGui::OpenPopup("Transaction Group Edit##Dialog");
		}
		else
		{
			opened_transaction = *transaction;
			ImGui::OpenPopup("Transaction Edit##Dialog");
		}
	}
	if (ImGui::BeginPopupModal("Transaction Group Edit##Dialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) transaction_group_dialog(transaction_model, row_index);
	if (ImGui::BeginPopupModal("Transaction Edit##Dialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) transaction_dialog(transaction_model, row_index);
	ImGui::SameLine();

	// Remove
	if (ImGui::Button("Remove", button_size) && row_valid) ImGui::OpenPopup("Transaction Remove##Dialog");
	if (ImGui::BeginPopupModal("Transaction Remove##Dialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Remove transaction with id %zu?", transaction_model.at(row_index)->id);
		if (ImGui::Button("OK"))
		{
			transaction_model.remove(row_index);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
	ImGui::SameLine();

	// Accounts
	ImVec4 account_button_color(0.0f, 0.7f, 0.0f, 1.0f);
	const int64_t account_total_amount = account_model.get_total_amount().value;
	const int64_t transaction_total_amount = transaction_model.get_global_total_amount().value;
	if (account_total_amount != transaction_total_amount) account_button_color = ImVec4(0.7f, 0.0f, 0.0f, 1.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, account_button_color);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(account_button_color.x + 0.1f, account_button_color.y + 0.1f, account_button_color.z + 0.1f, account_button_color.w));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(account_button_color.x - 0.1f, account_button_color.y - 0.1f, account_button_color.z - 0.1f, account_button_color.w));
	if (ImGui::Button("Accounts", button_size)) ImGui::OpenPopup("Accounts##Dialog");
	ImGui::PopStyleColor(3);
	if (ImGui::BeginPopupModal("Accounts##Dialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) accounts_dialog(account_model, account_total_amount, transaction_total_amount);
	ImGui::PopStyleColor(3);
}

Date date_input(const Date& date)
{
	int32_t day = uint32_t(date.day());
	int32_t month = uint32_t(date.month());
	int32_t year = int32_t(date.year());
	ImGui::PushItemWidth(50.0f);
	ImGui::InputInt("##Day", &day, 0, 0);
	ImGui::SameLine();
	ImGui::Text(".");
	ImGui::SameLine();
	ImGui::InputInt("##Month", &month, 0, 0);
	ImGui::SameLine();
	ImGui::Text(".");
	ImGui::SameLine();
	ImGui::PopItemWidth();
	ImGui::PushItemWidth(100.0f);
	ImGui::InputInt("##Year", &year, 0, 0);
	ImGui::PopItemWidth();
	// clamp date
	month = std::clamp(month, 1, 12);
	Date new_date = to_date(day, month, year);
	std::chrono::year_month_day_last day_last(new_date.year(), std::chrono::month_day_last(new_date.month()));
	day = std::clamp(day, 1, int32_t(uint32_t(day_last.day())));
	return to_date(day, month, year);
}

std::string string_input(const char* label, const char* hint, const std::string& text)
{
	std::array<char, 1024> input;
	std::snprintf(input.data(), input.size(), "%s", text.c_str());
	ImGui::InputTextWithHint(label, hint, input.data(), 1024);
	return std::string(input.data(), strnlen(input.data(), input.size()));
}

std::string category_dropdown(const std::string& category, const std::vector<std::string>& category_names)
{
	std::string new_category = category;
	if (ImGui::BeginCombo("##Category", category.c_str()))
	{
		for (int i = 0; i < category_names.size(); ++i)
		{
			bool selected = (category_names[i] == category);
			if (ImGui::Selectable(category_names[i].c_str(), selected)) new_category = category_names[i];
			if (selected) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	return new_category;
}

void TransactionPage::transaction_dialog(TransactionModel& transaction_model, int32_t transaction_index)
{
	opened_transaction.date = date_input(opened_transaction.date);
	opened_transaction.description = string_input("##Description", "Description", opened_transaction.description);
	bool parse_success = to_amount(string_input("##Amount", "Amount", opened_transaction.amount.to_string_view()), opened_transaction.amount);
	opened_transaction.category = category_dropdown(opened_transaction.category, transaction_model.get_category_names());
	if (ImGui::Button("OK##TransactionDialog"))
	{
		if (transaction_index == -1)
		{
			opened_transaction.id = rng::random_int64();
			opened_transaction.added = DateTime(Clock::now());
			opened_transaction.edited = DateTime(Clock::now());
			transaction_model.add(std::make_shared<Transaction>(opened_transaction));
		}
		else
		{
			opened_transaction.edited = DateTime(Clock::now());
			transaction_model.set(transaction_index, std::make_shared<Transaction>(opened_transaction));
		}
		ImGui::CloseCurrentPopup();
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel##TransactionDialog")) ImGui::CloseCurrentPopup();
	ImGui::EndPopup();
}

void TransactionPage::transaction_member_dialog(const std::vector<std::string>& category_names, int32_t transaction_index)
{
	opened_transaction.date = date_input(opened_transaction.date);
	opened_transaction.description = string_input("##Description", "Description", opened_transaction.description);
	bool parse_success = to_amount(string_input("##Amount", "Amount", opened_transaction.amount.to_string_view()), opened_transaction.amount);
	opened_transaction.category = category_dropdown(opened_transaction.category, category_names);
	if (ImGui::Button("OK##TransactionMemberDialog"))
	{
		if (transaction_index == -1)
		{
			opened_transaction.id = rng::random_int64();
			opened_transaction.added = DateTime(Clock::now());
			opened_transaction.edited = DateTime(Clock::now());
			opened_transaction_group.add_transaction(opened_transaction);
		}
		else
		{
			opened_transaction.edited = DateTime(Clock::now());
			opened_transaction_group.set_transaction(transaction_index, opened_transaction);
		}
		ImGui::CloseCurrentPopup();
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel##TransactionMemberDialog")) ImGui::CloseCurrentPopup();
	ImGui::EndPopup();
}

void TransactionPage::transaction_group_dialog(TransactionModel& transaction_model, int32_t transaction_index)
{
	opened_transaction_group.amount.value = 0;
	for (const Transaction& transaction : opened_transaction_group.get_transactions()) opened_transaction_group.amount.value += transaction.amount.value;
	ImGui::PushFont(NULL, 32.0f);
	ImGui::Text(" %s", opened_transaction_group.amount.to_string_view().c_str());
	ImGui::PopFont();
	opened_transaction_group.date = date_input(opened_transaction_group.date);
	opened_transaction_group.description = string_input("##Description", "Description", opened_transaction_group.description);
	opened_transaction_group.category = category_dropdown(opened_transaction_group.category, transaction_model.get_category_names());
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 6.0f));
	constexpr ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_NoHostExtendX;
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.0f , 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4( 0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4( 0.0f, 0.0f, 0.0f, 0.0f));

	ImGui::SeparatorText("Transactions");
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
		for (int32_t row = 0; row < opened_transaction_group.get_transactions().size(); row++)
		{
			ImGui::TableNextRow(ImGuiTableRowFlags_None);
			const Transaction& transaction = opened_transaction_group.get_transactions()[row];
			ImU32 color = IM_COL32(0, 0, 0, 255);
			const uint32_t intensity = std::min(uint64_t(std::abs(transaction.amount.value)) / 40ull + 20ull, 255ull);
			if (transaction.amount.is_negative()) color = IM_COL32(intensity, 0, 0, 150);
			else color = IM_COL32(0, intensity, 0, 150);
			for (int32_t column = 0; column < field_names.size(); column++)
			{
				const std::string& field_name = field_names[column];
				ImGui::TableSetColumnIndex(column);
				if (field_name == TransactionFieldNames::Category) ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, transaction_model.get_category_colors().at(transaction.category).get_ImU32());
				else ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
				// set up selection and highlighting
				if (column == 0)
				{
					ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
					ImGuiTable* table = ImGui::GetCurrentTable();
					ImVec2 upper_left(ImGui::TableGetCellBgRect(table, 0).Min.x, cursor_pos.y - ImGui::GetStyle().ItemInnerSpacing.y);
					// last column is the group column which is not contained in field_names
					ImVec2 lower_right(ImGui::TableGetCellBgRect(table, field_names.size() - 1).Max.x, cursor_pos.y + ImGui::GetTextLineHeight() + ImGui::GetStyle().ItemInnerSpacing.y);
					bool selected = (row == selected_group_row);
					if (ImGui::Selectable(transaction.get_field_view(field_name).c_str(), selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap | ImGuiSelectableFlags_DontClosePopups)) selected_group_row = row;
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
				else ImGui::Text("%s", transaction.get_field_view(field_name).c_str());
			}
		}
		ImGui::EndTable();
	}
	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar();

	const bool row_valid = selected_group_row > -1 && selected_group_row < opened_transaction_group.get_transactions().size();

	// Add
	if (ImGui::Button("Add##TransactionGroupDialog"))
	{
		opened_transaction = Transaction();
		opened_transaction.category = opened_transaction_group.category;
		opened_transaction.date = opened_transaction_group.date;
		ImGui::OpenPopup("Transaction Add##MemberDialog");
	}
	if (ImGui::BeginPopupModal("Transaction Add##MemberDialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) transaction_member_dialog(transaction_model.get_category_names(), -1);
	ImGui::SameLine();

	// Edit
	if (ImGui::Button("Edit##TransactionGroupDialog") && row_valid)
	{
		const Transaction& transaction = opened_transaction_group.get_transactions()[selected_group_row];
		opened_transaction = transaction;
		ImGui::OpenPopup("Transaction Edit##MemberDialog");
	}
	if (ImGui::BeginPopupModal("Transaction Edit##MemberDialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		transaction_member_dialog(transaction_model.get_category_names(), selected_group_row);
	}
	ImGui::SameLine();

	// Remove
	if (ImGui::Button("Remove##TransactionGroupDialog") && row_valid) ImGui::OpenPopup("Transaction Remove##MemberDialog");
	if (ImGui::BeginPopupModal("Transaction Remove##MemberDialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Remove transaction with id %zu?", opened_transaction_group.get_transactions()[selected_group_row].id);
		if (ImGui::Button("OK##TransactionGroupDialogRemove"))
		{
			opened_transaction_group.remove_transaction(selected_group_row);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel##TransactionGroupDialogRemove")) ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
	if (ImGui::Button("OK##TransactionGroupDialog"))
	{
		if (transaction_index == -1)
		{
			opened_transaction_group.id = rng::random_int64();
			opened_transaction_group.added = DateTime(Clock::now());
			opened_transaction_group.edited = DateTime(Clock::now());
			transaction_model.add(std::make_shared<TransactionGroup>(opened_transaction_group));
		}
		else
		{
			opened_transaction_group.edited = DateTime(Clock::now());
			transaction_model.set(transaction_index, std::make_shared<TransactionGroup>(opened_transaction_group));
		}
		ImGui::CloseCurrentPopup();
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel##TransactionGroupDialog")) ImGui::CloseCurrentPopup();
	ImGui::EndPopup();
}

void TransactionPage::accounts_dialog(AccountModel& account_model, int64_t account_total_amount, int64_t transaction_total_amount)
{
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 6.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImGui::GetStyle().CellPadding);
	const float row_height = ImGui::GetFrameHeight();
	constexpr ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_NoHostExtendX;
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.0f , 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4( 0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4( 0.0f, 0.0f, 0.0f, 0.0f));

	ImGui::PushFont(NULL, 32.0f);
	if (account_total_amount == transaction_total_amount) ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Accounts match!");
	else ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Accounts don't match! Difference: %s", Amount(account_total_amount - transaction_total_amount).to_string_view().c_str());
	ImGui::PopFont();
	if (ImGui::BeginTable("Accounts", 3, flags))
	{
		ImGui::TableSetupScrollFreeze(0, 1);
		ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(0, 0, 0, 255));
		ImGui::TableSetupColumn("##Index", ImGuiTableColumnFlags_None);
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_None);
		ImGui::TableSetupColumn("Amount", ImGuiTableColumnFlags_None);
		ImGui::TableHeadersRow();
		for (int32_t row = 0; row < account_model.count(); row++)
		{
			Account& account = account_model.at(row);
			ImGui::TableNextRow(ImGuiTableRowFlags_None, row_height);
			ImGui::TableSetColumnIndex(0);
			// set up selection and highlighting
			ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
			ImGuiTable* table = ImGui::GetCurrentTable();
			ImVec2 upper_left(ImGui::TableGetCellBgRect(table, 0).Min.x, cursor_pos.y - ImGui::GetStyle().ItemInnerSpacing.y);
			// last column is the group column which is not contained in field_names
			ImVec2 lower_right(ImGui::TableGetCellBgRect(table, 2).Max.x, cursor_pos.y + row_height + ImGui::GetStyle().ItemInnerSpacing.y);
			bool selected = (row == selected_account_row);
			const bool is_edited = row == opened_account_row && opened_account_row == selected_account_row;
			std::array<char, 32> label;
			std::snprintf(label.data(), label.size(), "%u", row + 1);
			ImGui::AlignTextToFramePadding();
			if (ImGui::Selectable(label.data(), selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap | ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_DontClosePopups))
			{
				selected_account_row = row;
				opened_account_row = -1;
			}
			bool hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
			if (hovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) opened_account_row = row;
			if (selected || hovered)
			{
				constexpr float border_thickness = 4.0f;
				ImDrawList* dl = ImGui::GetWindowDrawList();
				ImU32 col = ImGui::GetColorU32(ImGuiCol_HeaderActive);
				ImU32 color = IM_COL32(0, 255, 255, 128);
				if (selected) color = IM_COL32(0, 255, 255, 255);
				dl->AddRect(ImVec2(upper_left.x + border_thickness / 2.0f, upper_left.y), ImVec2(lower_right.x - border_thickness / 2.0f, lower_right.y), color, 0.0f, 0, 4.0f);
			}
			ImGui::TableSetColumnIndex(1);
			if (is_edited)
			{
				ImGui::SetNextItemWidth(-FLT_MIN);
				account.name = string_input("##AccountName", "Name", account.name);
			}
			else
			{
				ImGui::AlignTextToFramePadding();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetStyle().ItemInnerSpacing.x);
				ImGui::Text("%s", account.name.c_str());
			}
			ImGui::TableSetColumnIndex(2);
			if (is_edited)
			{
				ImGui::SetNextItemWidth(-FLT_MIN);
				to_amount(string_input("##AccountAmount", "Amount", account.amount.to_string_view()), account.amount);
			}
			else
			{
				ImGui::AlignTextToFramePadding();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetStyle().ItemInnerSpacing.x);
				ImGui::Text("%s", account.amount.to_string_view().c_str());
			}
		}
		ImGui::EndTable();
	}
	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar(2);
	if (ImGui::Button("Add##AccountsDialog")) account_model.add(Account());
	ImGui::SameLine();
	if (ImGui::Button("Remove##AccountsDialog")) account_model.remove(selected_account_row);
	ImGui::SameLine();
	if (ImGui::Button("Close##AccountsDialog")) ImGui::CloseCurrentPopup();
	ImGui::EndPopup();
}
