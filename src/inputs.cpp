#include "inputs.hpp"
#include "imgui.h"
#include "util/log.hpp"

bool lost_focus(bool has_focus, bool& had_focus)
{
	bool focus_lost;
	if (has_focus)
	{
		had_focus = true;
		focus_lost = false;
	}
	else
	{
		if (had_focus)
		{
			had_focus = false;
			focus_lost = true;
		}
		else
		{
			// never had focus
			focus_lost = false;
		}
	}
	return focus_lost;
}

void StringInput::init(const std::string& initial_text)
{
	std::snprintf(buffer.data(), buffer.size(), "%s", initial_text.c_str());
	focused = false;
}

bool StringInput::draw(const std::string& label, const char* hint)
{
	ImGui::InputTextWithHint(label.c_str(), hint, buffer.data(), buffer.size());
	return lost_focus(ImGui::IsItemFocused(), focused);
}

std::string StringInput::get_result()
{
	return std::string(buffer.data(), strnlen(buffer.data(), buffer.size()));
}

void DateInput::init(const Date& initial_date)
{
	day = uint32_t(initial_date.day());
	month = uint32_t(initial_date.month());
	year = int32_t(initial_date.year());
	focused = false;
}

bool DateInput::draw(const std::string& label, const char* hint)
{
	bool any_has_focus = false;
	ImGui::PushItemWidth(50.0f);
	ImGui::InputInt(std::string(label + "##Day").c_str(), &day, 0, 0);
	any_has_focus |= ImGui::IsItemFocused();
	ImGui::SameLine();
	ImGui::Text(".");
	ImGui::SameLine();
	ImGui::InputInt(std::string(label + "##Month").c_str(), &month, 0, 0);
	any_has_focus |= ImGui::IsItemFocused();
	ImGui::SameLine();
	ImGui::Text(".");
	ImGui::SameLine();
	ImGui::PopItemWidth();
	ImGui::PushItemWidth(100.0f);
	ImGui::InputInt(std::string(label + "##Year").c_str(), &year, 0, 0);
	any_has_focus |= ImGui::IsItemFocused();
	ImGui::PopItemWidth();
	return lost_focus(any_has_focus, focused);
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

void AmountInput::init(const Amount& initial_amount)
{
	input.init(initial_amount.to_string_view());
}

bool AmountInput::draw(const std::string& label, const char* hint)
{
	return input.draw(label);
}

Amount AmountInput::get_result()
{
	std::string result = input.get_result();
	Amount amount;
	to_amount(result, amount);
	input.init(amount.to_string_view());
	return amount;
}

void Dropdown::init(const std::vector<std::string>& options, int32_t initial_option)
{
	this->options.clear();
	this->options.reserve(options.size());
	for (const std::string& option : options) this->options.push_back(option);
	current = initial_option;
	focused = false;
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

bool Dropdown::draw(const std::string& label, const char* hint)
{
	const bool current_valid = (current >= 0 && current < options.size());
	bool changed = false;
	if (ImGui::BeginCombo("##Category", (current_valid ? options[current].c_str() : "")))
	{
		for (int i = 0; i < options.size(); ++i)
		{
			bool selected = (i == current);
			if (ImGui::Selectable(options[i].c_str(), selected))
			{
				changed = true;
				current = i;
			}
			if (selected) ImGui::SetItemDefaultFocus();
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
