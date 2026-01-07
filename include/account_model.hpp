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
	void set_name(int32_t index, const std::string& name);
	void set_amount(int32_t index, const Amount& amount);
	void add(const Account& account);
	void remove(int32_t index);
	void clear();
	const std::vector<Account>& get_data() const;
	Amount get_total_amount() const;

	mutable bool dirty = false;

private:
	std::vector<Account> accounts;
};
