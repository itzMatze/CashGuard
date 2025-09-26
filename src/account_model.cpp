#include "account_model.hpp"

AccountModel::AccountModel(QObject *parent) : QAbstractTableModel(parent)
{}

int AccountModel::rowCount(const QModelIndex& parent) const
{
	return static_cast<int>(accounts.size());
}

int AccountModel::columnCount(const QModelIndex& parent) const
{
	return 2; // name + amount
}

QVariant AccountModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid()) return QVariant();
	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		const Account &acc = accounts[index.row()];
		if (index.column() == 0) return acc.name;
		if (index.column() == 1) return acc.amount.toString() + " €";
	}
	return QVariant();
}

QVariant AccountModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
		if (section == 0) return "Name";
		if (section == 1) return "Amount";
	}
	return QVariant();
}

Qt::ItemFlags AccountModel::flags(const QModelIndex &index) const
{
	if (!index.isValid()) return Qt::NoItemFlags;
	return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

bool AccountModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (role == Qt::EditRole && index.isValid()) {
		Account& acc = accounts[index.row()];
		if (index.column() == 0) acc.name = value.toString();
		if (index.column() == 1) acc.amount = Amount(value.toString());
		emit dataChanged(index, index);
		return true;
	}
	return false;
}

void AccountModel::add(const Account& account)
{
	beginInsertRows(QModelIndex(), accounts.size(), accounts.size());
	accounts.push_back(account);
	endInsertRows();
}

void AccountModel::removeAccount(int row)
{
	if (row < 0 || row >= static_cast<int>(accounts.size())) return;
	beginRemoveRows(QModelIndex(), row, row);
	accounts.erase(accounts.begin() + row);
	endRemoveRows();
}

const std::vector<Account>& AccountModel::getData() const
{
	return accounts;
}

Amount AccountModel::getTotalAmount() const
{
	int32_t amount = 0;
	for (const Account& account : accounts) amount += account.amount.value;
	return Amount(amount);
}
