#include "transaction.hpp"
#include <cstdint>
#include "util/log.hpp"

Amount::Amount(int64_t value) : value(value)
{}

std::string Amount::to_string() const
{
	return (is_negative() ? "-" : "+") + std::format("{}.{:02}", (std::abs(value) / 100), (std::abs(value) % 100));
}

std::string Amount::to_string_view() const
{
	return to_string() + std::string(" €");
}

bool Amount::is_negative() const
{
	return value < 0;
}

bool get_euros(const std::string& string_value, int32_t& position, int64_t& euros)
{
	int32_t old_position = position;
	euros = 0;
	std::from_chars_result result = std::from_chars(string_value.data() + position, string_value.data() + string_value.size(), euros);
	position = result.ptr - string_value.data();
	return position > old_position;
}

bool get_cents(const std::string& string_value, int32_t& position, int64_t& cents)
{
	int32_t old_position = position;
	cents = 0;
	std::from_chars_result result = std::from_chars(string_value.data() + position, string_value.data() + string_value.size(), cents);
	position = result.ptr - string_value.data();
	if (position - old_position == 1) cents *= 10;
	return (position - old_position < 3 && position != old_position);
}

bool to_amount(const std::string& string_value, Amount& amount)
{
	if (string_value.size() == 0)
	{
		amount.value = 0;
		return true;
	}
	amount.value = 0;
	int32_t position = 0;
	bool negative = true;
	// if we are able to parse something the input is considered valid
	bool has_digits = false;
	// look for sign
	if (string_value[position] == '+')
	{
		negative = false;
		position++;
	}
	else if (string_value[position] == '-') position++;
	// parse euros
	int64_t euros = 0;
	has_digits = get_euros(string_value, position, euros);
	position++;
	// parse cents
	int64_t cents = 0;
	if (get_cents(string_value, position, cents))
	{
		has_digits = true;
		amount.value += cents;
	}
	amount.value += euros * 100;
	if (negative) amount.value = -amount.value;
	return has_digits;
}

bool operator<(const Amount& a, const Amount& b)
{
	return a.value < b.value;
}

bool operator>(const Amount& a, const Amount& b)
{
	return a.value > b.value;
}

namespace DateUtils
{
	std::string to_string(const Date& date)
	{
		return std::format("{:%d.%m.%Y}", date);
	}

	std::string to_string(const DateTime& time_point)
	{
		return std::format("{:%d.%m.%Y %H:%M:%S}", std::chrono::time_point_cast<std::chrono::seconds>(time_point));
	}

	Date to_date(DateTime time_point)
	{
		return std::chrono::floor<std::chrono::days>(time_point);
	}

	Date to_date(int32_t day, int32_t month, int32_t year)
	{
		return Date(std::chrono::year{year}, std::chrono::month{uint32_t(month)}, std::chrono::day{uint32_t(day)});
	}
}

Transaction::Transaction() :
	id(0),
	date(DateUtils::to_date(Clock::now())),
	category_id(0),
	amount(0),
	description(""),
	added(Clock::now()),
	edited(Clock::now())
{}

std::string Transaction::get_field(int32_t field_index) const
{
	if (field_index == TRANSACTION_FIELD_ID) return std::to_string(id);
	else if (field_index == TRANSACTION_FIELD_DATE) return DateUtils::to_string(date);
	else if (field_index == TRANSACTION_FIELD_CATEGORY) return std::to_string(category_id);
	else if (field_index == TRANSACTION_FIELD_AMOUNT) return amount.to_string();
	else if (field_index == TRANSACTION_FIELD_DESCRIPTION) return description;
	else if (field_index == TRANSACTION_FIELD_ADDED) return DateUtils::to_string(added);
	else if (field_index == TRANSACTION_FIELD_EDITED) return DateUtils::to_string(edited);
	else CG_THROW("Invalid transaction field name!");
}

void Transaction::set_field(int32_t field_index, const std::string& value)
{
	if (field_index == TRANSACTION_FIELD_ID) id = std::stoull(value);
	else if (field_index == TRANSACTION_FIELD_DATE)
	{
		std::istringstream iss(value);
		DateTime date_time;
		iss >> std::chrono::parse("%d.%m.%Y", date_time);
		if (!iss.fail()) date = DateUtils::to_date(date_time);
	}
	else if (field_index == TRANSACTION_FIELD_CATEGORY) category_id = std::stoull(value);
	else if (field_index == TRANSACTION_FIELD_AMOUNT)
	{
		Amount new_amount;
		if (to_amount(value, new_amount)) amount = new_amount;
	}
	else if (field_index == TRANSACTION_FIELD_DESCRIPTION) description = value;
	else if (field_index == TRANSACTION_FIELD_ADDED)
	{ 
		std::istringstream iss(value);
		DateTime date_time;
		iss >> std::chrono::parse("%d.%m.%Y %H:%M:%S", date_time);
		if (!iss.fail()) added = date_time;
	}
	else if (field_index == TRANSACTION_FIELD_EDITED)
	{
		std::istringstream iss(value);
		DateTime date_time;
		iss >> std::chrono::parse("%d.%m.%Y %H:%M:%S", date_time);
		if (!iss.fail()) edited = date_time;
	}
	else CG_THROW("Invalid transaction field name!");
}

bool operator<(const Transaction& a, const Transaction& b)
{
	if (a.date == b.date)
	{
		if (a.added == b.added) return a.id < b.id;
		else return a.added < b.added;
	}
	else return a.date < b.date;
}

bool operator>(const Transaction& a, const Transaction& b)
{
	if (a.date == b.date)
	{
		if (a.added == b.added) return a.id > b.id;
		else return a.added > b.added;
	}
	else return a.date > b.date;
}

bool operator==(const Transaction& a, const Transaction& b)
{
	for (int32_t i = 0; i < TRANSACTION_FIELD_COUNT; i++)
	{
		if (a.get_field(i) != b.get_field(i)) return false;
	}
	return true;
}

TransactionGroup::TransactionGroup(const Transaction& transaction) : Transaction(transaction)
{}

void TransactionGroup::add_transaction(const Transaction& transaction)
{
	int32_t index = 0;
	while (index < transactions.size() && transaction < transactions[index]) index++;
	transactions.insert(transactions.begin() + index, transaction);
	update_amount();
}

void TransactionGroup::remove_transaction(int32_t index)
{
	transactions.erase(transactions.begin() + index);
	update_amount();
}

void TransactionGroup::set_transaction(int32_t index, const Transaction& transaction)
{
	remove_transaction(index);
	add_transaction(transaction);
}

const std::vector<Transaction>& TransactionGroup::get_transactions() const
{
	return transactions;
}

void TransactionGroup::update_amount()
{
	amount.value = 0;
	for (const Transaction& transaction : transactions) amount.value += transaction.amount.value;
}

bool operator==(const TransactionGroup& a, const TransactionGroup& b)
{
	for (int32_t i = 0; i < TRANSACTION_FIELD_COUNT; i++)
	{
		if (a.get_field(i) != b.get_field(i)) return false;
	}
	if (a.get_transactions().size() != b.get_transactions().size()) return false;
	for (int32_t i = 0; i < a.get_transactions().size(); i++)
	{
		if (a.get_transactions()[i] != b.get_transactions()[i]) return false;
	}
	return true;
}
