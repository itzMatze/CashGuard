#pragma once

#include "transaction.hpp"
#include <string>
#include <vector>

struct Account
{
	std::string name;
	Amount amount;
};

class AccountModel
{
public:
	AccountModel() = default;

	int32_t count() const;
	const Account& at(int32_t index) const;
	void set(int32_t index, const Account& account);
	void add(const Account& account);
	void remove(int32_t index);
	const std::vector<Account>& get_data() const;
	Amount get_total_amount() const;

private:
	std::vector<Account> accounts;
};
