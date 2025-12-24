#include "transaction_page.hpp"
#include "account_model.hpp"
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
	if (ImGui::Button("Add", button_size))
	{
		opened_transaction = Transaction();
		ImGui::OpenPopup("Transaction Add##Dialog");
	}
	if (ImGui::BeginPopupModal("Transaction Add##Dialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) transaction_dialog(transaction_model, -1);
	ImGui::SameLine();
	ImGui::Button("Add Group", button_size);
	ImGui::SameLine();
	if (ImGui::Button("Edit", button_size) && row_valid)
	{
		opened_transaction = *transaction_model.at(row_index);
		ImGui::OpenPopup("Transaction Edit##Dialog");
	}
	if (ImGui::BeginPopupModal("Transaction Edit##Dialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		transaction_dialog(transaction_model, row_index);
	}
	ImGui::SameLine();
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
	ImGui::PopStyleColor(3);
	ImGui::SameLine();
	ImVec4 account_button_color(0.0f, 0.7f, 0.0f, 1.0f);
	if (account_model.get_total_amount().value != transaction_model.get_global_total_amount().value) account_button_color = ImVec4(0.7f, 0.0f, 0.0f, 1.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, account_button_color);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(account_button_color.x + 0.1f, account_button_color.y + 0.1f, account_button_color.z + 0.1f, account_button_color.w));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(account_button_color.x - 0.1f, account_button_color.y - 0.1f, account_button_color.z - 0.1f, account_button_color.w));
	ImGui::Button("Accounts", button_size);
	ImGui::PopStyleColor(3);
}

void TransactionPage::transaction_dialog(TransactionModel& transaction_model, int32_t transaction_index)
{
	int32_t day = uint32_t(opened_transaction.date.day());
	int32_t month = uint32_t(opened_transaction.date.month());
	int32_t year = int32_t(opened_transaction.date.year());
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
	opened_transaction.date = to_date(day, month, year);
	std::chrono::year_month_day_last day_last(opened_transaction.date.year(), std::chrono::month_day_last(opened_transaction.date.month()));
	day = std::clamp(day, 1, int32_t(uint32_t(day_last.day())));
	opened_transaction.date = to_date(day, month, year);
	std::array<char, 1024> description_input;
	std::snprintf(description_input.data(), description_input.size(), "%s", opened_transaction.description.c_str());
	ImGui::InputTextWithHint("##Description", "Description", description_input.data(), 1024);
	opened_transaction.description = std::string(description_input.data(), strnlen(description_input.data(), description_input.size()));
	std::array<char, 1024> amount_input;
	std::snprintf(amount_input.data(), amount_input.size(), "%s", opened_transaction.amount.to_string().c_str());
	ImGui::InputTextWithHint("##Amount", "Amount", amount_input.data(), 1024);
	bool parse_success = to_amount(std::string(amount_input.data(), strnlen(amount_input.data(), amount_input.size())), opened_transaction.amount);
	// category dropdown
	if (ImGui::BeginCombo("##Category", opened_transaction.category.c_str()))
	{
		const std::vector<std::string>& category_names = transaction_model.get_category_names();
		for (int i = 0; i < category_names.size(); ++i)
		{
			bool selected = (category_names[i] == opened_transaction.category);
			if (ImGui::Selectable(category_names[i].c_str(), selected)) opened_transaction.category = category_names[i];
			if (selected) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	if (ImGui::Button("OK"))
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
	if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
	ImGui::EndPopup();
}
