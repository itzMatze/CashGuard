#include "account_model.hpp"

int32_t AccountModel::count() const
{
	return static_cast<int32_t>(accounts.size());
}

const Account& AccountModel::at(int32_t index) const
{
	return accounts[index];
}

void AccountModel::set(int32_t index, const Account& account)
{
	accounts[index] = account;
	dirty = true;
}

void AccountModel::set_name(int32_t index, const std::string& name)
{
	accounts[index].name = name;
	dirty = true;
}

void AccountModel::set_amount(int32_t index, const Amount& amount)
{
	accounts[index].amount = amount;
	dirty = true;
}

void AccountModel::add(const Account& account)
{
	accounts.push_back(account);
	dirty = true;
}

void AccountModel::remove(int32_t index)
{
	if (index < 0 || index >= accounts.size()) return;
	accounts.erase(accounts.begin() + index);
	dirty = true;
}

void AccountModel::clear()
{
	accounts.clear();
	dirty = true;
}

const std::vector<Account>& AccountModel::get_data() const
{
	return accounts;
}

Amount AccountModel::get_total_amount() const
{
	int32_t amount = 0;
	for (const Account& account : accounts) amount += account.amount.value;
	return Amount(amount);
}
