#pragma once

#include "transaction.hpp"
#include <array>
#include <string>

template<typename T>
class Input
{
public:
	virtual void init(const T& initial_value) = 0;
	virtual bool draw(const std::string& label, const char* hint = "") = 0;
	virtual T get_result() = 0;
};

class StringInput : Input<std::string>
{
public:
	StringInput() = default;
	void init(const std::string& initial_text = "") override;
	bool draw(const std::string& label, const char* hint = "") override;
	std::string get_result() override;

private:
	std::array<char, 1024> buffer;
	bool focused;
};

class DateInput : Input<Date>
{
public:
	DateInput() = default;
	void init(const Date& initial_date = Date()) override;
	bool draw(const std::string& label, const char* hint = "") override;
	Date get_result() override;

private:
	int32_t day;
	int32_t month;
	int32_t year;
	bool focused;
};

class AmountInput : Input<Amount>
{
public:
	AmountInput() = default;
	void init(const Amount& initial_amount = Amount()) override;
	bool draw(const std::string& label, const char* hint = "") override;
	Amount get_result() override;

private:
	StringInput input;
};

class Dropdown
{
public:
	Dropdown() = default;
	void init(const std::vector<std::string>& options, int32_t initial_option = -1);
	void init(const std::vector<std::string>& options, const std::string& initial_option);
	bool draw(const std::string& label, const char* hint = "");
	int32_t get_result();
	std::string get_result_string();

private:
	std::vector<std::string> options;
	int32_t current;
	bool focused;
};
