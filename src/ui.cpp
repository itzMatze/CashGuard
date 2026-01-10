#include "ui.hpp"
#include "account_model.hpp"
#include "transaction_model.hpp"
#include "category_model.hpp"
#include "util/random_generator.hpp"

void UI::init(const TransactionModel& transaction_model, const AccountModel& account_model, const CategoryModel& category_model)
{
	open_existing_file = false;
	create_new_file = false;
	total_amount_graph.update_data(transaction_model);
	transaction_filter.init(category_model);
	filtered_transaction_model.clear();
	for (const std::shared_ptr<const Transaction> t : transaction_model.get_transactions())
	{
		if (transaction_filter.check(t)) filtered_transaction_model.add(t);
	}
}

void UI::draw(ImVec2 available_space, TransactionModel& transaction_model, AccountModel& account_model, CategoryModel& category_model, bool valid_file)
{
	static constexpr ImVec4 button_color(0.0f, 0.4f, 0.4f, 1.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, button_color);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(button_color.x + 0.2f, button_color.y + 0.2f, button_color.z + 0.2f, button_color.w));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(button_color.x + 0.2f, button_color.y + 0.2f, button_color.z + 0.2f, button_color.w));
	ImGui::PushStyleColor(ImGuiCol_Tab, button_color);
	ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(button_color.x + 0.2f, button_color.y + 0.2f, button_color.z + 0.2f, button_color.w));
	ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(button_color.x + 0.2f, button_color.y + 0.2f, button_color.z + 0.2f, button_color.w));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Border, button_color);
	ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_TitleBg, button_color);
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, button_color);
	ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, button_color);
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
	available_space.y -= ImGui::GetFrameHeightWithSpacing();
	if (ImGui::BeginTabBar("Main Tabs"))
	{
		if (ImGui::BeginTabItem("Transactions"))
		{
			draw_transaction_tab(available_space, transaction_model, account_model, category_model, valid_file);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Graph"))
		{
			draw_graph_tab(available_space, transaction_model, account_model, category_model);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::PopStyleColor(14);
}

void UI::draw_transaction_tab(ImVec2 available_space, TransactionModel& transaction_model, AccountModel& account_model, CategoryModel& category_model, bool valid_file)
{
	static constexpr float graph_relative_height = 0.2f;
	static constexpr float table_relative_height = 0.7f;
	static constexpr float buttons_relative_height = 1.0f - graph_relative_height - table_relative_height;

	ImGui::PushFont(NULL, 64.0f);
	// center text vertically
	const float text_height = ImGui::GetFrameHeight();
	float cursor_pos_y = ImGui::GetCursorPosY();
	ImGui::SetCursorPosY(cursor_pos_y + (available_space.y * graph_relative_height - text_height) / 2.0f);
	if (show_amounts) ImGui::Text(" %s", filtered_transaction_model.get_total_amount().to_string_view().c_str());
	else ImGui::Text(" X €");
	ImGui::PopFont();
	ImGui::SameLine();
	ImGui::SetCursorPosY(cursor_pos_y);
	total_amount_graph.draw_small_graph(ImVec2(-1.0f, available_space.y * graph_relative_height), show_amounts);
	transaction_table.draw(ImVec2(available_space.x, available_space.y * table_relative_height - ImGui::GetStyle().ItemSpacing.y), filtered_transaction_model, category_model, show_amounts);
	const int32_t row_index = transaction_table.get_selected_row();
	const bool row_valid = row_index > -1 && row_index < filtered_transaction_model.count() && transaction_table.get_selected_transaction() != nullptr;
	constexpr int32_t button_count_x = 4;
	constexpr int32_t button_count_y = 2;
	ImVec2 button_size(available_space.x * (1.0f / float(button_count_x)) - ImGui::GetStyle().ItemSpacing.x * float(button_count_x - 1) / float(button_count_x), (available_space.y * buttons_relative_height - ImGui::GetStyle().ItemSpacing.y) / float(button_count_y));

	if (!valid_file) ImGui::BeginDisabled();
	// Add
	ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_N);
	if (ImGui::Button("Add", button_size))
	{
		transaction_dialog.init(filtered_transaction_model, category_model, Transaction());
		ImGui::OpenPopup("Transaction Add##Dialog");
	}
	if (ImGui::BeginPopupModal("Transaction Add##Dialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		DialogResult result = transaction_dialog.draw("Transaction Dialog", filtered_transaction_model, category_model);
		if (result == DialogResult::Accept)
		{
			std::shared_ptr<Transaction> new_transaction = transaction_dialog.get_transaction();
			new_transaction->id = rng::random_int64();
			new_transaction->added = DateTime(Clock::now());
			new_transaction->edited = DateTime(Clock::now());
			transaction_model.add(new_transaction);
			if (transaction_filter.check(new_transaction))
			{
				filtered_transaction_model.add(new_transaction);
				total_amount_graph.update_data(filtered_transaction_model);
				filtered_transaction_model.dirty = false;
			}
			ImGui::CloseCurrentPopup();
		}
		else if (result == DialogResult::Cancel) ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
	ImGui::SameLine();

	// Edit
	ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_E);
	if (ImGui::Button("Edit", button_size) && row_valid)
	{
		const std::shared_ptr<const Transaction> transaction = transaction_table.get_selected_transaction();
		if (std::shared_ptr<const TransactionGroup> transaction_group = std::dynamic_pointer_cast<const TransactionGroup>(transaction))
		{
			transaction_dialog.init(filtered_transaction_model, category_model, *transaction_group);
		}
		else
		{
			transaction_dialog.init(filtered_transaction_model, category_model, *transaction);
		}
		ImGui::OpenPopup("Transaction Edit##Dialog");
	}
	if (ImGui::BeginPopupModal("Transaction Edit##Dialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		DialogResult result = transaction_dialog.draw("Transaction Dialog", filtered_transaction_model, category_model);
		if (result == DialogResult::Accept)
		{
			std::shared_ptr<Transaction> new_transaction = transaction_dialog.get_transaction();
			new_transaction->edited = DateTime(Clock::now());
			transaction_model.set(transaction_table.get_selected_transaction(), new_transaction);
			filtered_transaction_model.remove(transaction_table.get_selected_transaction());
			if (transaction_filter.check(new_transaction)) filtered_transaction_model.add(new_transaction);
			if (filtered_transaction_model.dirty)
			{
				total_amount_graph.update_data(filtered_transaction_model);
				filtered_transaction_model.dirty = false;
			}
			ImGui::CloseCurrentPopup();
		}
		else if (result == DialogResult::Cancel) ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
	ImGui::SameLine();

	// Remove
	ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_R);
	if (ImGui::Button("Remove", button_size) && row_valid) ImGui::OpenPopup("Transaction Remove##Dialog");
	if (ImGui::BeginPopupModal("Transaction Remove##Dialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Remove transaction with id %zu?", transaction_table.get_selected_transaction()->id);
		ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_O);
		if (ImGui::Button("OK"))
		{
			transaction_model.remove(transaction_table.get_selected_transaction());
			filtered_transaction_model.remove(transaction_table.get_selected_transaction());
			if (filtered_transaction_model.dirty)
			{
				total_amount_graph.update_data(filtered_transaction_model);
				filtered_transaction_model.dirty = false;
			}
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_C);
		if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
	ImGui::SameLine();

	// Categories
	if (ImGui::Button("Categories", button_size))
	{
		categories_dialog.init(category_model);
		ImGui::OpenPopup("Categories##Dialog");
	}
	if (ImGui::BeginPopupModal("Categories##Dialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) categories_dialog.draw(category_model, transaction_model.get_category_usage_counts());
	if (!valid_file) ImGui::EndDisabled();

	// Open File
	if (ImGui::Button("Select File", button_size)) ImGui::OpenPopup("File##Dialog");
	if (ImGui::BeginPopupModal("File##Dialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (ImGui::Button("New File"))
		{
			create_new_file = true;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Open File"))
		{
			open_existing_file = true;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
	ImGui::SameLine();

	if (!valid_file) ImGui::BeginDisabled();
	// Filter
	ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F);
	if (ImGui::Button(std::format("Filter ({} / {})", filtered_transaction_model.count(), transaction_model.count()).c_str(), button_size))
	{
		ImGui::OpenPopup("Filter##Dialog");
	}
	if (ImGui::BeginPopupModal("Filter##Dialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		DialogResult result = transaction_filter.draw("Transaction Filter");
		if (result == DialogResult::Accept || result == DialogResult::Cancel)
		{
			filtered_transaction_model.clear();
			for (const std::shared_ptr<const Transaction> t : transaction_model.get_transactions())
			{
				if (transaction_filter.check(t)) filtered_transaction_model.add(t);
			}
			total_amount_graph.update_data(filtered_transaction_model);
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	ImGui::SameLine();

	// show / hide amounts
	ImGui::SetNextItemShortcut(ImGuiKey_Space);
	const char* label = show_amounts ? "Hide Amounts" : "Show Amounts";
	if (ImGui::Button(label, button_size))
	{
		show_amounts = !show_amounts;
	}
	ImGui::SameLine();

	// Accounts
	ImVec4 account_button_color(0.0f, 0.7f, 0.0f, 1.0f);
	const int64_t account_total_amount = account_model.get_total_amount().value;
	const int64_t transaction_total_amount = transaction_model.get_total_amount().value;
	if (account_total_amount != transaction_total_amount) account_button_color = ImVec4(0.7f, 0.0f, 0.0f, 1.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, account_button_color);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(account_button_color.x + 0.2f, account_button_color.y + 0.2f, account_button_color.z + 0.2f, account_button_color.w));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(account_button_color.x + 0.2f, account_button_color.y + 0.2f, account_button_color.z + 0.2f, account_button_color.w));
	ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_A);
	if (ImGui::Button("Accounts", button_size))
	{
		accounts_dialog.init(account_model, transaction_total_amount);
		ImGui::OpenPopup("Accounts##Dialog");
	}
	ImGui::PopStyleColor(3);
	if (ImGui::BeginPopupModal("Accounts##Dialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) accounts_dialog.draw(account_model);
	if (!valid_file) ImGui::EndDisabled();
}

void UI::draw_graph_tab(ImVec2 available_space, const TransactionModel& transaction_model, const AccountModel& account_model, const CategoryModel& category_model)
{
	total_amount_graph.draw_large_graph(ImVec2(-1.0f, available_space.y));
}
