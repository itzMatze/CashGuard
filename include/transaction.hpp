#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

struct Amount
{
	Amount() = default;
	Amount(int64_t value);
	std::string to_string() const;
	bool is_negative() const;

	// amount in cents
	int64_t value;
};

bool to_amount(const std::string& string_value, Amount& amount);

bool operator<(const Amount& a, const Amount& b);
bool operator>(const Amount& a, const Amount& b);

namespace TransactionFieldNames
{
	constexpr std::string ID = "ID";
	constexpr std::string Date = "Date";
	constexpr std::string Category = "Category";
	constexpr std::string Amount = "Amount";
	constexpr std::string Description = "Description";
	constexpr std::string Added = "Added";
	constexpr std::string Edited = "Edited";
}

using Clock = std::chrono::system_clock;
using Date = std::chrono::year_month_day;
using DateTime = Clock::time_point;

Date to_date(DateTime time_point);

class Transaction
{
public:
	Transaction();
	virtual ~Transaction() = default;
	static std::vector<std::string> get_field_names();
	std::string get_field(const std::string& field_name) const;
	std::string get_field_view(const std::string& field_name) const;
	void set_field(const std::string& field_name, const std::string& value);
	virtual std::string to_string() const;

	size_t id;
	Date date;
	std::string category;
	Amount amount;
	std::string description;
	DateTime added;
	DateTime edited;
};

bool operator<(const Transaction& a, const Transaction& b);
bool operator==(const Transaction& a, const Transaction& b);

class TransactionGroup : public Transaction
{
public:
	TransactionGroup() = default;
	TransactionGroup(const Transaction& transaction);
	std::string to_string() const override;

	std::vector<std::shared_ptr<Transaction>> transactions;
};

bool operator==(const TransactionGroup& a, const TransactionGroup& b);
