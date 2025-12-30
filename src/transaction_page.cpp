#include "transaction_page.hpp"
#include "account_model.hpp"
#include "transaction_model.hpp"
#include "util/random_generator.hpp"

void TransactionPage::init(TransactionModel& transaction_model, AccountModel& account_model)
{
	small_total_amount_graph.update_data(transaction_model);
}

constexpr float graph_relative_height = 0.2f;
constexpr float table_relative_height = 0.75f;
constexpr float buttons_relative_height = 1.0f - graph_relative_height - table_relative_height;

void TransactionPage::draw(ImVec2 available_space, TransactionModel& transaction_model, AccountModel& account_model)
{
	const int32_t row_index = transaction_table.get_selected_row();
	const bool row_valid = row_index > -1 && row_index < transaction_model.count();
	ImGui::PushFont(NULL, 64.0f);
	// center text vertically
	float cursor_pos_y = ImGui::GetCursorPosY();
	const float text_height = ImGui::GetFrameHeight();
	ImGui::SetCursorPosY((available_space.y * graph_relative_height - text_height) / 2.0f);
	ImGui::Text(" %s", transaction_model.get_filtered_total_amount().to_string_view().c_str());
	ImGui::PopFont();
	ImGui::SameLine();
	ImGui::SetCursorPosY(cursor_pos_y);
	small_total_amount_graph.draw(ImVec2(-1.0f, available_space.y * graph_relative_height));
	transaction_table.draw(ImVec2(available_space.x, available_space.y * table_relative_height - ImGui::GetStyle().ItemSpacing.y), transaction_model);
	constexpr int32_t button_count = 5;
	ImVec2 button_size(available_space.x * (1.0f / float(button_count)) - ImGui::GetStyle().ItemSpacing.x * float(button_count - 1) / float(button_count), available_space.y * buttons_relative_height - ImGui::GetStyle().ItemSpacing.y);
	constexpr ImVec4 button_color(0.0f, 0.4f, 0.4f, 1.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, button_color);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(button_color.x + 0.1f, button_color.y + 0.1f, button_color.z + 0.1f, button_color.w));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(button_color.x - 0.1f, button_color.y - 0.1f, button_color.z - 0.1f, button_color.w));

	// Add
	ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_N);
	if (ImGui::Button("Add", button_size))
	{
		transaction_dialog.init(transaction_model);
		ImGui::OpenPopup("Transaction Add##Dialog");
	}
	if (ImGui::BeginPopupModal("Transaction Add##Dialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		DialogResult result = transaction_dialog.draw("Transaction Dialog", transaction_model);
		if (result == DialogResult::Accept)
		{
			Transaction new_transaction = transaction_dialog.get_transaction();
			new_transaction.id = rng::random_int64();
			new_transaction.added = DateTime(Clock::now());
			new_transaction.edited = DateTime(Clock::now());
			transaction_model.add(std::make_shared<Transaction>(new_transaction));
			ImGui::CloseCurrentPopup();
		}
		else if (result == DialogResult::Cancel) ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
	ImGui::SameLine();

	// Add Group
	ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_G);
	if (ImGui::Button("Add Group", button_size))
	{
		transaction_group_dialog.init(transaction_model);
		ImGui::OpenPopup("Transaction Group Add##Dialog");
	}
	if (ImGui::BeginPopupModal("Transaction Group Add##Dialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		DialogResult result = transaction_group_dialog.draw("Transaction Group Dialog", transaction_model);
		if (result == DialogResult::Accept)
		{
			TransactionGroup new_transaction_group = transaction_group_dialog.get_transaction_group();
			new_transaction_group.id = rng::random_int64();
			new_transaction_group.added = DateTime(Clock::now());
			new_transaction_group.edited = DateTime(Clock::now());
			transaction_model.add(std::make_shared<TransactionGroup>(new_transaction_group));
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
		const std::shared_ptr<const Transaction> transaction = transaction_model.at(row_index);
		if (const std::shared_ptr<const TransactionGroup> transaction_group = std::dynamic_pointer_cast<const TransactionGroup>(transaction))
		{
			transaction_group_dialog.init(transaction_model, *transaction_group);
			ImGui::OpenPopup("Transaction Group Edit##Dialog");
		}
		else
		{
			transaction_dialog.init(transaction_model, *transaction);
			ImGui::OpenPopup("Transaction Edit##Dialog");
		}
	}
	if (ImGui::BeginPopupModal("Transaction Group Edit##Dialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		DialogResult result = transaction_group_dialog.draw("Transaction Group Dialog", transaction_model);
		if (result == DialogResult::Accept)
		{
			TransactionGroup new_transaction_group = transaction_group_dialog.get_transaction_group();
			new_transaction_group.edited = DateTime(Clock::now());
			transaction_model.set(row_index, std::make_shared<TransactionGroup>(new_transaction_group));
			ImGui::CloseCurrentPopup();
		}
		else if (result == DialogResult::Cancel) ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
	if (ImGui::BeginPopupModal("Transaction Edit##Dialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		DialogResult result = transaction_dialog.draw("Transaction Dialog", transaction_model);
		if (result == DialogResult::Accept)
		{
			Transaction new_transaction = transaction_dialog.get_transaction();
			new_transaction.edited = DateTime(Clock::now());
			transaction_model.set(row_index, std::make_shared<Transaction>(new_transaction));
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
		ImGui::Text("Remove transaction with id %zu?", transaction_model.at(row_index)->id);
		ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_O);
		if (ImGui::Button("OK"))
		{
			transaction_model.remove(row_index);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_C);
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
	ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_A);
	if (ImGui::Button("Accounts", button_size))
	{
		accounts_dialog.init(account_model, transaction_total_amount);
		ImGui::OpenPopup("Accounts##Dialog");
	}
	ImGui::PopStyleColor(3);
	if (ImGui::BeginPopupModal("Accounts##Dialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) accounts_dialog.draw(account_model);
	ImGui::PopStyleColor(3);
}
