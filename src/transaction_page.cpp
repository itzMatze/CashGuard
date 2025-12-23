#include "transaction_page.hpp"
#include "account_model.hpp"
#include "transaction_model.hpp"

void TransactionPage::draw(ImVec2 available_space, TransactionModel& transaction_model, AccountModel& account_model)
{
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
	ImGui::Button("Add", button_size);
	ImGui::SameLine();
	ImGui::Button("Group", button_size);
	ImGui::SameLine();
	ImGui::Button("Edit", button_size);
	ImGui::SameLine();
	ImGui::Button("Remove", button_size);
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
