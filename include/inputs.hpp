#pragma once

#include "imgui.h"
#include "transaction.hpp"
#include "util/timer.hpp"
#include <array>
#include <string>

class StringInput
{
public:
	StringInput() = default;
	void init(const std::string& initial_text = "");
	void update(const std::string& new_text);
	bool draw(const std::string& label, const char* hint = "");
	std::string get_result();

private:
	std::array<char, 1024> buffer;
	bool focused;
};

class DateInput
{
public:
	DateInput() = default;
	void init(const Date& initial_date = Date());
	void update(const Date& new_date);
	bool draw(const std::string& label, const char* hint = "");
	Date get_result();

private:
	int32_t day;
	int32_t month;
	int32_t year;
	bool focused;
};

class AmountInput
{
public:
	AmountInput() = default;
	void init(const Amount& initial_amount = Amount());
	void update(const Amount& new_amount);
	bool draw(const std::string& label, const char* hint = "");
	Amount get_result();

private:
	StringInput input;
};

class Dropdown
{
public:
	Dropdown() = default;
	void init(const std::vector<std::string>& options, int32_t initial_option = -1);
	void init(const std::vector<std::string>& options, const std::string& initial_option);
	void update(const std::string& new_selected_option);
	bool draw(const std::string& label, const char* hint = "");
	int32_t get_result();
	std::string get_result_string();

private:
	std::vector<std::string> options;
	ImGuiTextFilter filter;
	int32_t current;
	bool focused;
};

class CompletionInput
{
public:
	void init(const std::vector<std::string>& completion_items, const std::string& initial_text = "");
	bool draw(const std::string& label, const char* hint = "");
	std::string get_result();

private:
	std::array<char, 1024> buffer;
	bool focused;
	std::vector<std::string> completion_items;
	std::vector<std::string> filtered_completion_items;
	bool is_filter_updated = false;
	Timer timer;
	float filter_delay = 0.5f;
	int32_t result_count = 100;
	bool is_open = false;
	int32_t selected_index = -1;

	void update_filter();
};
