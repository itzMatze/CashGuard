#include "transaction_model.hpp"

#include <QBrush>

#include "transaction.hpp"

TransactionModel::TransactionModel(QObject* parent) : QAbstractTableModel(parent)
{}

int TransactionModel::rowCount(const QModelIndex& parent) const
{
	return transactions.size();
}

int TransactionModel::columnCount(const QModelIndex& parent) const
{
	return Transaction::getFieldNames().size();
}

QVariant TransactionModel::data(const QModelIndex& index, int role) const
{
	if (role == Qt::DisplayRole) return transactions[index.row()]->getFieldView(Transaction::getFieldNames().at(index.column()));
	if (role == Qt::BackgroundRole)
	{
		// Check the content of a specific column
		if (transactions[index.row()]->amount.isNegative()) return QBrush(QColor(255, 0, 0, 100));
		else return QBrush(QColor(0, 255, 0, 100));
	}
	return QVariant();
}

QVariant TransactionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole || orientation != Qt::Horizontal) return QVariant();
	return Transaction::getFieldNames().at(section);
}

void TransactionModel::removeTransaction(uint32_t idx)
{
	beginRemoveRows(QModelIndex(), idx, idx);
	transactions.erase(transactions.begin() + idx);
	endRemoveRows();
}

void TransactionModel::add(const std::shared_ptr<Transaction>& transaction)
{
	uint32_t idx = 0;
	while (idx < transactions.size() && *transaction < *transactions[idx]) idx++;
	beginInsertRows(QModelIndex(), idx, idx);
	transactions.insert(transactions.begin() + idx, transaction);
	endInsertRows();
}

std::shared_ptr<const Transaction> TransactionModel::getTransaction(uint32_t idx) const
{
	return transactions.at(idx);
}

void TransactionModel::setTransaction(uint32_t idx, const std::shared_ptr<Transaction>& transaction)
{
	removeTransaction(idx);
	add(transaction);
}

const std::vector<std::shared_ptr<Transaction>>& TransactionModel::getTransactions() const
{
	return transactions;
}

void TransactionModel::clear()
{
	beginResetModel();
	transactions.clear();
	endResetModel();
}
