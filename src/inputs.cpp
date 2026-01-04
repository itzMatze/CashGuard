#include "inputs.hpp"
#include "imgui.h"
#include "util/log.hpp"
#include "util/utils.hpp"

void StringInput::init(const std::string& initial_text)
{
	std::snprintf(buffer.data(), buffer.size(), "%s", initial_text.c_str());
	active = false;
}

void StringInput::update(const std::string& new_text)
{
	std::snprintf(buffer.data(), buffer.size(), "%s", new_text.c_str());
}

bool StringInput::draw(const std::string& label, const char* hint, bool set_focus)
{
	if (set_focus) ImGui::SetKeyboardFocusHere();
	ImGui::InputTextWithHint(label.c_str(), hint, buffer.data(), buffer.size());
	return lost_active(ImGui::IsItemActive(), active);
}

std::string StringInput::get_result()
{
	return std::string(buffer.data(), strnlen(buffer.data(), buffer.size()));
}

bool StringInput::is_active() const
{
	return active;
}

void DateInput::init(const Date& initial_date)
{
	day = uint32_t(initial_date.day());
	month = uint32_t(initial_date.month());
	year = int32_t(initial_date.year());
	active = false;
}

void DateInput::update(const Date& new_date)
{
	day = uint32_t(new_date.day());
	month = uint32_t(new_date.month());
	year = int32_t(new_date.year());
}

bool DateInput::draw(const std::string& label, const char* hint, bool set_focus)
{
	bool any_is_active = false;
	ImGui::PushItemWidth(50.0f);
	if (set_focus) ImGui::SetKeyboardFocusHere();
	ImGui::InputInt(std::string(label + "##Day").c_str(), &day, 0, 0);
	any_is_active |= ImGui::IsItemActive();
	ImGui::SameLine();
	ImGui::Text(".");
	ImGui::SameLine();
	ImGui::InputInt(std::string(label + "##Month").c_str(), &month, 0, 0);
	any_is_active |= ImGui::IsItemActive();
	ImGui::SameLine();
	ImGui::Text(".");
	ImGui::SameLine();
	ImGui::PopItemWidth();
	ImGui::PushItemWidth(100.0f);
	ImGui::InputInt(std::string(label + "##Year").c_str(), &year, 0, 0);
	any_is_active |= ImGui::IsItemActive();
	ImGui::PopItemWidth();
	return lost_active(any_is_active, active);
}

Date DateInput::get_result()
{
	// clamp date
	month = std::clamp(month, 1, 12);
	Date new_date = to_date(day, month, year);
	std::chrono::year_month_day_last day_last(new_date.year(), std::chrono::month_day_last(new_date.month()));
	day = std::clamp(day, 1, int32_t(uint32_t(day_last.day())));
	return to_date(day, month, year);
}

bool DateInput::is_active() const
{
	return active;
}

void AmountInput::init(const Amount& initial_amount)
{
	input.init(initial_amount.to_string_view());
}

void AmountInput::update(const Amount& new_amount)
{
	input.update(new_amount.to_string_view());
}

bool AmountInput::draw(const std::string& label, const char* hint, bool set_focus)
{
	return input.draw(label, "", set_focus);
}

Amount AmountInput::get_result()
{
	std::string result = input.get_result();
	Amount amount;
	to_amount(result, amount);
	input.init(amount.to_string_view());
	return amount;
}

bool AmountInput::is_active() const
{
	return input.is_active();
}

void Dropdown::init(const std::vector<std::string>& options, int32_t initial_option)
{
	this->options.clear();
	this->options.reserve(options.size());
	for (const std::string& option : options) this->options.push_back(option);
	current = initial_option;
	active = false;
}

void Dropdown::init(const std::vector<std::string>& options, const std::string& initial_option)
{
	for (int32_t i = 0; i < options.size(); i++)
	{
		if (options[i] == initial_option)
		{
			init(options, i);
			return;
		}
	}
	init(options);
}

void Dropdown::update(const std::string& new_selected_option)
{
	for (int32_t i = 0; i < options.size(); i++)
	{
		if (options[i] == new_selected_option)
		{
			current = i;
			return;
		}
	}
}

bool Dropdown::draw(const std::string& label, const char* hint)
{
	const bool current_valid = (current >= 0 && current < options.size());
	bool changed = false;
	if (ImGui::BeginCombo(label.c_str(), (current_valid ? options[current].c_str() : "")))
	{
		if (ImGui::IsWindowAppearing())
		{
			ImGui::SetKeyboardFocusHere();
			filter.Clear();
		}
		ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F);
		filter.Draw("##Filter", -FLT_MIN);
		for (int i = 0; i < options.size(); ++i)
		{
			if (filter.PassFilter(options[i].c_str()))
			{
				bool selected = (i == current);
				if (ImGui::Selectable((options[i] + "##" + std::to_string(i)).c_str(), selected))
				{
					changed = true;
					current = i;
				}
				if (selected) ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	return changed;
}

int32_t Dropdown::get_result()
{
	return current;
}

std::string Dropdown::get_result_string()
{
	CG_ASSERT(current >= 0 && current < options.size(), "Invalid current element!");
	if (current < 0 || current >= options.size()) return std::string();
	return options[current];
}

int32_t input_callback(ImGuiInputTextCallbackData* data)
{
	int32_t* p_selected_index = (int32_t*)(data->UserData);
	if (ImGui::IsKeyDown(ImGuiKey_UpArrow))
	{
		(*p_selected_index)--;
		return 1;
	}
	else if (ImGui::IsKeyDown(ImGuiKey_DownArrow))
	{
		(*p_selected_index)++;
		return 1;
	}
	return 0;
}

void CompletionInput::init(const std::vector<std::string>& completion_items, const std::string& initial_text)
{
	this->completion_items.clear();
	filtered_completion_items.clear();
	std::snprintf(buffer.data(), buffer.size(), "%s", initial_text.c_str());
	active = false;
	filtered_completion_items.reserve(result_count);
	for (const std::string& item : completion_items) this->completion_items.push_back(item);
	update_filter();
}

bool CompletionInput::draw(const std::string& label, const char* hint)
{
	ImVec2 input_pos = ImGui::GetCursorScreenPos();
	int32_t local_selected_index = selected_index;
	bool input_changed = ImGui::InputTextWithHint(label.c_str(), hint, buffer.data(), buffer.size(), ImGuiInputTextFlags_CallbackHistory, &input_callback, (void*)(&local_selected_index));
	selected_index = std::max(std::min(local_selected_index, int32_t(filtered_completion_items.size()) - 1), -1);
	bool is_input_active = ImGui::IsItemActive();

	if (input_changed)
	{
		timer.restart();
		is_open = true;
		is_filter_updated = false;
		selected_index = -1;
	}

	if (lost_active(is_input_active, active))
	{
		if (selected_index != -1 && !filtered_completion_items.empty()) std::snprintf(buffer.data(), buffer.size(), "%s", filtered_completion_items[selected_index].c_str());
		is_open = false;
		return true;
	}

	if (is_open)
	{
		// get the size of the input box to match the popup width
		ImVec2 input_size = ImGui::GetItemRectSize();
		ImGui::SetNextWindowPos(ImVec2(input_pos.x, input_pos.y + input_size.y));
		ImGui::SetNextWindowSize(ImVec2(input_size.x, 0.0f));
		ImGui::BeginTooltip();
		if (timer.elapsed() > filter_delay && !is_filter_updated) update_filter();

		for (int32_t i = 0; i < filtered_completion_items.size(); i++)
		{
			bool is_selected = (selected_index == i);
			if (is_selected) ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%s", filtered_completion_items[i].c_str());
			else ImGui::Text("%s", filtered_completion_items[i].c_str());
		}

		if (filtered_completion_items.empty()) ImGui::TextDisabled("No results");
		if (!is_input_active && !ImGui::IsWindowFocused()) is_open = false;
		ImGui::EndTooltip();
	}
	return false;
}

std::string CompletionInput::get_result()
{
	return std::string(buffer.data(), strnlen(buffer.data(), buffer.size()));
}

void CompletionInput::update_filter()
{
	std::string current_text(buffer.data(), strnlen(buffer.data(), buffer.size()));
	filtered_completion_items.clear();
	for (int32_t i = 0; i < completion_items.size() && filtered_completion_items.size() < result_count; i++)
	{
		if (current_text.empty() || contains_substring_case_insensitive(completion_items[i], current_text)) filtered_completion_items.push_back(completion_items[i]);
	}
	is_filter_updated = true;
}
