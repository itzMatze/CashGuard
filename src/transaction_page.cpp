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

	// Add
	if (ImGui::Button("Add", button_size))
	{
		opened_transaction = Transaction();
		ImGui::OpenPopup("Transaction Add##Dialog");
	}
	if (ImGui::BeginPopupModal("Transaction Add##Dialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) transaction_dialog(transaction_model, -1);
	ImGui::SameLine();
	ImGui::Button("Add Group", button_size);
	ImGui::SameLine();

	// Edit
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
	ImGui::PopStyleColor(3);
	ImGui::SameLine();

	// Accounts
	ImVec4 account_button_color(0.0f, 0.7f, 0.0f, 1.0f);
	if (account_model.get_total_amount().value != transaction_model.get_global_total_amount().value) account_button_color = ImVec4(0.7f, 0.0f, 0.0f, 1.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, account_button_color);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(account_button_color.x + 0.1f, account_button_color.y + 0.1f, account_button_color.z + 0.1f, account_button_color.w));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(account_button_color.x - 0.1f, account_button_color.y - 0.1f, account_button_color.z - 0.1f, account_button_color.w));
	ImGui::Button("Accounts", button_size);
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
	bool parse_success = to_amount(string_input("##Amount", "Amount", opened_transaction.get_field_view(TransactionFieldNames::Amount)), opened_transaction.amount);
	opened_transaction.category = category_dropdown(opened_transaction.category, transaction_model.get_category_names());
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
