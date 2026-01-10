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
	std::string to_string_view() const;
	bool is_negative() const;

	// amount in cents
	int64_t value;
};

bool to_amount(const std::string& string_value, Amount& amount);

bool operator<(const Amount& a, const Amount& b);
bool operator>(const Amount& a, const Amount& b);

enum TransactionFieldIndices : uint32_t
{
	TRANSACTION_FIELD_ID = 0,
	TRANSACTION_FIELD_DATE = 1,
	TRANSACTION_FIELD_CATEGORY = 2,
	TRANSACTION_FIELD_AMOUNT = 3,
	TRANSACTION_FIELD_DESCRIPTION = 4,
	TRANSACTION_FIELD_ADDED = 5,
	TRANSACTION_FIELD_EDITED = 6,
	TRANSACTION_FIELD_COUNT
};

using Clock = std::chrono::system_clock;
using Date = std::chrono::year_month_day;
using DateTime = Clock::time_point;

namespace DateUtils
{
	std::string to_string(const Date& date);
	std::string to_string(const DateTime& time_point);
	Date to_date(DateTime time_point);
	Date to_date(int32_t day, int32_t month, int32_t year);
}

class Transaction
{
public:
	Transaction();
	virtual ~Transaction() = default;
	std::string get_field(int32_t field_index) const;
	void set_field(int32_t field_index, const std::string& value);

	uint64_t id;
	Date date;
	uint64_t category_id;
	Amount amount;
	std::string description;
	DateTime added;
	DateTime edited;
};

bool operator<(const Transaction& a, const Transaction& b);
bool operator>(const Transaction& a, const Transaction& b);
bool operator==(const Transaction& a, const Transaction& b);

class TransactionGroup : public Transaction
{
public:
	TransactionGroup() = default;
	TransactionGroup(const Transaction& transaction);
	void set_transaction(int32_t index, const Transaction& transaction);
	void add_transaction(const Transaction& transaction);
	void remove_transaction(int32_t index);
	const std::vector<Transaction>& get_transactions() const;
	void update_amount();

private:
	std::vector<Transaction> transactions;
};

bool operator==(const TransactionGroup& a, const TransactionGroup& b);
