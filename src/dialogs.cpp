#include "dialogs.hpp"
#include "account_model.hpp"
#include "category_model.hpp"
#include "imgui_internal.h"
#include "transaction_model.hpp"
#include "util/random_generator.hpp"

void TransactionMemberDialog::init(const TransactionModel& transaction_model, const CategoryModel& category_model, const Transaction& transaction)
{
	this->transaction = transaction;
	date_input.init(transaction.date);
	description_input.init(transaction_model.get_unique_value_list(TRANSACTION_FIELD_DESCRIPTION), transaction.description);
	amount_input.init(transaction.amount);
	category_dropdown.init(category_model.get_categories(), transaction.category_id);
}

DialogResult TransactionMemberDialog::draw(const std::string& label, const TransactionModel& transaction_model)
{
	if (date_input.draw("##TransactionMemberDialogDate")) transaction.date = date_input.get_result();
	if (description_input.draw("##TransactionMemberDialogDescription", "Description"))
	{
		transaction.description = description_input.get_result();
		const bool amount_empty = transaction.amount.value == 0;
		const bool category_empty = transaction.category_id == 0;
		if (amount_empty || category_empty)
		{
			std::shared_ptr<const Transaction> matching_transaction;
			if (transaction_model.get_auto_complete_transaction(transaction.description, matching_transaction))
			{
				if (amount_empty)
				{
					transaction.amount = matching_transaction->amount;
					amount_input.update(transaction.amount);
				}
				if (category_empty)
				{
					transaction.category_id = matching_transaction->category_id;
					category_dropdown.update(transaction.category_id);
				}
			}
		}
	}
	if (amount_input.draw("##TransactionMemberDialogAmount")) transaction.amount = amount_input.get_result();
	if (category_dropdown.draw("##TransactionMemberDialogCategory")) transaction.category_id = category_dropdown.get_result().id;
	ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_O);
	if (ImGui::Button("OK##TransactionMemberDialog")) return DialogResult::Accept;
	ImGui::SameLine();
	ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_C);
	if (ImGui::Button("Cancel##TransactionMemberDialog")) return DialogResult::Cancel;
	return DialogResult::None;
}

Transaction TransactionMemberDialog::get_transaction()
{
	transaction.date = date_input.get_result();
	transaction.description = description_input.get_result();
	transaction.amount = amount_input.get_result();
	transaction.category_id = category_dropdown.get_result().id;
	return transaction;
}

void TransactionDialog::init(const TransactionModel& transaction_model, const CategoryModel& category_model, const Transaction& transaction)
{
	this->transaction = std::make_shared<Transaction>(transaction);
	date_input.init(transaction.date);
	description_input.init(transaction_model.get_unique_value_list(TRANSACTION_FIELD_DESCRIPTION), transaction.description);
	amount_input.init(transaction.amount);
	category_dropdown.init(category_model.get_categories(), transaction.category_id);
	first_draw = true;
}

void TransactionDialog::init(const TransactionModel& transaction_model, const CategoryModel& category_model, const TransactionGroup& transaction_group)
{
	this->transaction = std::make_shared<TransactionGroup>(transaction_group);
	date_input.init(transaction_group.date);
	description_input.init(transaction_model.get_unique_value_list(TRANSACTION_FIELD_DESCRIPTION), transaction_group.description);
	category_dropdown.init(category_model.get_categories(), transaction_group.category_id);
	first_draw = true;
}

DialogResult TransactionDialog::draw(const std::string& label, const TransactionModel& transaction_model, const CategoryModel& category_model)
{
	if (std::shared_ptr<TransactionGroup> transaction_group = std::dynamic_pointer_cast<TransactionGroup>(transaction))
	{
		ImGui::PushFont(NULL, 32.0f);
		ImGui::Text(" %s", transaction_group->amount.to_string_view().c_str());
		ImGui::PopFont();
		if (date_input.draw("##TransactionDialogDate")) transaction_group->date = date_input.get_result();
		if (description_input.draw("##TransactionDialogDescription", "Description"))
		{
			transaction_group->description = description_input.get_result();
			const bool category_empty = transaction_group->category_id == 0;
			if (category_empty)
			{
				std::shared_ptr<const Transaction> matching_transaction;
				if (transaction_model.get_auto_complete_transaction(transaction_group->description, matching_transaction))
				{
					if (category_empty)
					{
						transaction_group->category_id = matching_transaction->category_id;
						category_dropdown.update(transaction_group->category_id);
					}
				}
			}
		}
		if (category_dropdown.draw("##TransactionDialogCategory")) transaction_group->category_id = category_dropdown.get_result().id;
		ImGui::SeparatorText("Transactions");
		draw_transaction_table(transaction_model, category_model);
		const bool row_valid = selected_group_row > -1 && selected_group_row < transaction_group->get_transactions().size();

		// New Sub-Transaction
		ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_N);
		if (ImGui::Button("New Sub-Transaction##TransactionDialog"))
		{
			Transaction new_transaction = Transaction();
			new_transaction.category_id = transaction_group->category_id;
			new_transaction.date = transaction_group->date;
			member_dialog.init(transaction_model, category_model, new_transaction);
			ImGui::OpenPopup("New Sub-Transaction##MemberDialog");
		}
		if (ImGui::BeginPopupModal("New Sub-Transaction##MemberDialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			DialogResult result = member_dialog.draw("Member Transaction Dialog", transaction_model);
			if (result == DialogResult::Accept)
			{
				Transaction new_transaction = member_dialog.get_transaction();
				new_transaction.id = rng::random_int64();
				new_transaction.added = DateTime(Clock::now());
				new_transaction.edited = DateTime(Clock::now());
				transaction_group->add_transaction(new_transaction);
				ImGui::CloseCurrentPopup();
			}
			else if (result == DialogResult::Cancel) ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
		ImGui::SameLine();

		// Edit
		ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_E);
		if (ImGui::Button("Edit##TransactionDialog") && row_valid)
		{
			member_dialog.init(transaction_model, category_model, transaction_group->get_transactions()[selected_group_row]);
			ImGui::OpenPopup("Transaction Edit##MemberDialog");
		}
		if (ImGui::BeginPopupModal("Transaction Edit##MemberDialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			DialogResult result = member_dialog.draw("Member Transaction Dialog", transaction_model);
			if (result == DialogResult::Accept)
			{
				Transaction new_transaction = member_dialog.get_transaction();
				new_transaction.edited = DateTime(Clock::now());
				transaction_group->set_transaction(selected_group_row, new_transaction);
				ImGui::CloseCurrentPopup();
			}
			else if (result == DialogResult::Cancel) ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
		ImGui::SameLine();

		// Remove
		ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_R);
		if (ImGui::Button("Remove##TransactionDialog") && row_valid) ImGui::OpenPopup("Transaction Remove##MemberDialog");
		if (ImGui::BeginPopupModal("Transaction Remove##MemberDialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Remove transaction with id %zu?", transaction_group->get_transactions()[selected_group_row].id);
			ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_O);
			if (ImGui::Button("OK##TransactionDialogRemove"))
			{
				transaction_group->remove_transaction(selected_group_row);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_C);
			if (ImGui::Button("Cancel##TransactionDialogRemove")) ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
	}
	else
	{
		if (date_input.draw("##TransactionDialogDate", first_draw)) transaction->date = date_input.get_result();
		if (description_input.draw("##TransactionDialogDescription", "Description"))
		{
			transaction->description = description_input.get_result();
			const bool amount_empty = transaction->amount.value == 0;
			const bool category_empty = transaction->category_id == 0;
			if (amount_empty || category_empty)
			{
				std::shared_ptr<const Transaction> matching_transaction;
				if (transaction_model.get_auto_complete_transaction(transaction->description, matching_transaction))
				{
					if (amount_empty)
					{
						transaction->amount = matching_transaction->amount;
						amount_input.update(transaction->amount);
					}
					if (category_empty)
					{
						transaction->category_id = matching_transaction->category_id;
						category_dropdown.update(transaction->category_id);
					}
				}
			}
		}
		if (amount_input.draw("##TransactionDialogAmount")) transaction->amount = amount_input.get_result();
		if (category_dropdown.draw("##TransactionDialogCategory")) transaction->category_id = category_dropdown.get_result().id;
	}
	first_draw = false;

	ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_O);
	if (ImGui::Button("OK##TransactionDialog")) return DialogResult::Accept;
	ImGui::SameLine();
	ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_C);
	if (ImGui::Button("Cancel##TransactionDialog")) return DialogResult::Cancel;
	ImGui::SameLine();
	if (std::shared_ptr<TransactionGroup> transaction_group = std::dynamic_pointer_cast<TransactionGroup>(transaction))
	{
		if (ImGui::Button("Demote to Transaction"))
		{
			std::shared_ptr<Transaction> new_transaction = std::make_shared<Transaction>();
			new_transaction->id = transaction_group->id;
			new_transaction->date = transaction_group->date;
			new_transaction->category_id = transaction_group->category_id;
			new_transaction->amount = transaction_group->amount;
			new_transaction->description = transaction_group->description;
			new_transaction->added = transaction_group->added;
			new_transaction->edited = transaction_group->edited;
			transaction = new_transaction;
			update_ui();
		}
	}
	else
	{
		if (ImGui::Button("Promote to Transaction Group"))
		{
			std::shared_ptr<TransactionGroup> new_transaction_group = std::make_shared<TransactionGroup>();
			new_transaction_group->id = transaction->id;
			new_transaction_group->date = transaction->date;
			new_transaction_group->category_id = transaction->category_id;
			new_transaction_group->description = transaction->description;
			new_transaction_group->added = transaction->added;
			new_transaction_group->edited = transaction->edited;
			Transaction member_transaction;
			member_transaction.id = rng::random_int64();
			member_transaction.date = transaction->date;
			member_transaction.category_id = transaction->category_id;
			member_transaction.amount = transaction->amount;
			member_transaction.added = transaction->added;
			member_transaction.edited = transaction->edited;
			new_transaction_group->add_transaction(member_transaction);
			transaction = new_transaction_group;
			update_ui();
		}
	}
	return DialogResult::None;
}

std::shared_ptr<Transaction> TransactionDialog::get_transaction()
{
	transaction->date = date_input.get_result();
	transaction->description = description_input.get_result();
	transaction->category_id = category_dropdown.get_result().id;
	if (!std::dynamic_pointer_cast<TransactionGroup>(transaction)) transaction->amount = amount_input.get_result();
	return transaction;
}

void TransactionDialog::draw_transaction_table(const TransactionModel& transaction_model, const CategoryModel& category_model)
{
	std::shared_ptr<TransactionGroup> transaction_group = std::dynamic_pointer_cast<TransactionGroup>(transaction);
	constexpr ImVec2 padding(8.0f, 6.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, padding);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
	const float row_height = ImGui::GetFrameHeight() + padding.y * 2.0f;
	constexpr ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_NoHostExtendX;
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.0f , 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4( 0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4( 0.0f, 0.0f, 0.0f, 0.0f));
	if (ImGui::BeginTable("Transactions##TransactionDialog", TRANSACTION_FIELD_COUNT, flags))
	{
		ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(0, 0, 0, 255));
		ImGui::TableSetupScrollFreeze(0, 1);
		ImGui::TableSetupColumn(get_transation_field_name(TRANSACTION_FIELD_ID).c_str());
		ImGui::TableSetupColumn(get_transation_field_name(TRANSACTION_FIELD_DATE).c_str());
		ImGui::TableSetupColumn(get_transation_field_name(TRANSACTION_FIELD_CATEGORY).c_str());
		ImGui::TableSetupColumn(get_transation_field_name(TRANSACTION_FIELD_AMOUNT).c_str());
		ImGui::TableSetupColumn(get_transation_field_name(TRANSACTION_FIELD_DESCRIPTION).c_str());
		ImGui::TableSetupColumn(get_transation_field_name(TRANSACTION_FIELD_ADDED).c_str());
		ImGui::TableSetupColumn(get_transation_field_name(TRANSACTION_FIELD_EDITED).c_str());
		ImGui::TableHeadersRow();
		for (int32_t row = 0; row < transaction_group->get_transactions().size(); row++)
		{
			ImGui::TableNextRow(ImGuiTableRowFlags_None, row_height);
			const Transaction& transaction = transaction_group->get_transactions()[row];
			ImU32 background_color = IM_COL32(0, 0, 0, 255);
			const uint32_t intensity = std::min(uint64_t(std::abs(transaction.amount.value)) / 40ull + 20ull, 255ull);
			if (transaction.amount.is_negative()) background_color = IM_COL32(intensity, 0, 0, 150);
			else background_color = IM_COL32(0, intensity, 0, 150);
			bool hovered = false;
			bool selected = false;
			ImGui::TableSetColumnIndex(0);
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, background_color);
			selected = (row == selected_group_row);
			if (ImGui::Selectable(std::to_string(transaction.id).c_str(), selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap | ImGuiSelectableFlags_NoAutoClosePopups))
			{
				selected_group_row = row;
			}
			hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
			ImGui::TableSetColumnIndex(1);
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, background_color);
			ImGui::Text("%s", DateUtils::to_string(transaction.date).c_str());
			ImGui::TableSetColumnIndex(2);
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, category_model.get_category(transaction.category_id).color.get_ImU32());
			ImGui::Text("%s", category_model.get_category(transaction.category_id).name.c_str());
			ImGui::TableSetColumnIndex(3);
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, background_color);
			ImGui::Text("%s", transaction.amount.to_string_view().c_str());
			ImGui::TableSetColumnIndex(4);
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, background_color);
			ImGui::Text("%s", transaction.description.c_str());
			ImGui::TableSetColumnIndex(5);
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, background_color);
			ImGui::Text("%s", DateUtils::to_string(transaction.added).c_str());
			ImGui::TableSetColumnIndex(6);
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, background_color);
			ImGui::Text("%s", DateUtils::to_string(transaction.edited).c_str());
			if (selected || hovered)
			{
				ImGuiTable* table = ImGui::GetCurrentTable();
				ImVec2 min(ImGui::TableGetCellBgRect(table, 0).Min);
				ImVec2 max(ImGui::TableGetCellBgRect(table, TRANSACTION_FIELD_COUNT - 1).Max);
				constexpr float border_thickness = 4.0f;
				ImDrawList* dl = ImGui::GetWindowDrawList();
				ImU32 highlight_color = IM_COL32(0, 255, 255, 128);
				if (selected) highlight_color = IM_COL32(0, 255, 255, 255);
				dl->AddRect(ImVec2(min.x + border_thickness, min.y + border_thickness), ImVec2(max.x - border_thickness, min.y + row_height - border_thickness), highlight_color, 0.0f, 0, border_thickness);
			}
		}
		ImGui::EndTable();
	}
	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar(2);
}

void TransactionDialog::update_ui()
{
	date_input.update(transaction->date);
	description_input.update(transaction->description);
	amount_input.update(transaction->amount);
	category_dropdown.update(transaction->category_id);
}

void AccountsDialog::init(AccountModel& account_model, int64_t transaction_total_amount)
{
	this->transaction_total_amount = transaction_total_amount;
	account_total_amount = account_model.get_total_amount().value;
	selected_row = -1;
	opened_row = -1;
}

void AccountsDialog::draw(AccountModel& account_model)
{
	constexpr ImVec2 padding(8.0f, 6.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, padding);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, padding);
	const float row_height = ImGui::GetFrameHeight() + padding.y * 2.0f;
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
			ImGui::TableNextRow(ImGuiTableRowFlags_None, row_height);
			ImGui::TableSetColumnIndex(0);
			// set up selection and highlighting
			bool selected = (row == selected_row);
			const bool is_edited = row == opened_row && opened_row == selected_row;
			std::array<char, 32> label;
			std::snprintf(label.data(), label.size(), "%u", row + 1);
			ImGui::AlignTextToFramePadding();
			if (ImGui::Selectable(label.data(), selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap | ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_DontClosePopups))
			{
				selected_row = row;
				opened_row = -1;
			}
			bool hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
			if (hovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				opened_row = row;
				name_input.init(account_model.at(opened_row).name);
				amount_input.init(account_model.at(opened_row).amount);
			}
			ImGui::TableSetColumnIndex(1);
			if (is_edited)
			{
				ImGui::SetNextItemWidth(-FLT_MIN);
				if (name_input.draw("##AccountName", "Name")) account_model.set_name(opened_row, name_input.get_result());
			}
			else
			{
				ImGui::AlignTextToFramePadding();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetStyle().ItemInnerSpacing.x);
				ImGui::Text("%s", account_model.at(row).name.c_str());
			}
			ImGui::TableSetColumnIndex(2);
			if (is_edited)
			{
				ImGui::SetNextItemWidth(-FLT_MIN);
				if (amount_input.draw("##AccountAmount", "Amount"))
				{
					account_model.set_amount(opened_row, amount_input.get_result());
					account_total_amount = account_model.get_total_amount().value;
				}
			}
			else
			{
				ImGui::AlignTextToFramePadding();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetStyle().ItemInnerSpacing.x);
				ImGui::Text("%s", account_model.at(row).amount.to_string_view().c_str());
			}
			if (selected || hovered)
			{
				ImU32 highlight_color = IM_COL32(0, 255, 255, 128);
				if (selected) highlight_color = IM_COL32(0, 255, 255, 255);
				ImGuiTable* table = ImGui::GetCurrentTable();
				ImVec2 min(ImGui::TableGetCellBgRect(table, 0).Min);
				ImVec2 max(ImGui::TableGetCellBgRect(table, 2).Max);
				constexpr float border_thickness = 4.0f;
				ImDrawList* dl = ImGui::GetWindowDrawList();
				dl->AddRect(ImVec2(min.x + border_thickness / 2.0f, min.y), ImVec2(max.x - border_thickness / 2.0f, min.y + row_height), highlight_color, 0.0f, 0, border_thickness);
			}
		}
		ImGui::EndTable();
	}
	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar(2);
	ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_A);
	if (ImGui::Button("Add##AccountsDialog")) account_model.add(Account());
	ImGui::SameLine();
	ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_R);
	if (ImGui::Button("Remove##AccountsDialog")) account_model.remove(selected_row);
	ImGui::SameLine();
	ImGui::SetNextItemShortcut(ImGuiKey_Escape);
	if (ImGui::Button("Close##AccountsDialog")) ImGui::CloseCurrentPopup();
	ImGui::EndPopup();
}

void CategoriesDialog::init(CategoryModel& category_model)
{
	selected_row = -1;
	set_focus = false;
}

void CategoriesDialog::draw(CategoryModel& category_model, const std::unordered_map<uint64_t, int32_t>& category_usage_counts)
{
	for (int row = 0; row < category_model.get_categories().size(); row++)
	{
		const Category& category = category_model.get_categories()[row];
		if (category.id == 0) continue;
		ImGui::PushStyleColor(ImGuiCol_Header, category.color.get_ImU32());
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, category.color.get_ImU32());
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, category.color.get_ImU32());
		bool selected = (row == selected_row);
		if (ImGui::Selectable((category.name + "##" + std::to_string(row)).c_str(), selected, ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_DontClosePopups) && selected_row == -1)
		{
			selected_row = row;
			input.init(category.name);
			color_buffer.x = category.color.r;
			color_buffer.y = category.color.g;
			color_buffer.z = category.color.b;
			color_buffer.w = category.color.a;
			set_focus = true;
		}
		if (selected)
		{
			ImGui::SetNextItemWidth(-FLT_MIN);
			input.draw("##CategoryDialogStringInput" + std::to_string(row), "", set_focus);
			ImGui::ColorEdit4("Color Editor", (float*)(&color_buffer), ImGuiColorEditFlags_None);
			if (ImGui::Button("OK##CategoryEdit"))
			{
				category_model.set_name(selected_row, input.get_result());
				category_model.set_color(selected_row, Color(color_buffer.x, color_buffer.y, color_buffer.z, color_buffer.w));
				selected_row = -1;
			}
			ImGui::SameLine();
			const int32_t usage_count = (category_usage_counts.count(category.id) > 0) ? (category_usage_counts.at(category.id)) : 0;
			const std::string label = std::format("Remove Category (used {} times)##CategoryEdit", usage_count);
			if (usage_count > 0)
			{
				ImGui::BeginDisabled();
				ImGui::Button(label.c_str());
				ImGui::EndDisabled();
			}
			else
			{
				if (ImGui::Button(label.c_str()))
				{
					category_model.remove(selected_row);
					selected_row = -1;
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel##CategoryEdit")) selected_row = -1;
			set_focus = false;
			ImGui::Separator();
		}

		ImGui::PopStyleColor(3);
	}
	ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_A);
	if (ImGui::Button("Add##CategoryDialog"))
	{
		const Category new_category(rng::random_int64(), "", Color(0.0f, 0.0f, 0.0f, 1.0f));
		category_model.add(new_category);
		selected_row = category_model.count() - 1;
		input.init(new_category.name);
		color_buffer.x = new_category.color.r;
		color_buffer.y = new_category.color.g;
		color_buffer.z = new_category.color.b;
		color_buffer.w = new_category.color.a;
		set_focus = true;
	}
	ImGui::SameLine();
	ImGui::SetNextItemShortcut(ImGuiKey_Escape);
	if (ImGui::Button("Close##CategoryDialog")) ImGui::CloseCurrentPopup();
	ImGui::EndPopup();
}
