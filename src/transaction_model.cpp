#include "transaction_model.hpp"

#include <QBrush>
#include <memory>
#include <set>

#include "transaction.hpp"

TransactionModel::TransactionModel(QObject* parent) : QAbstractTableModel(parent)
{}

int TransactionModel::rowCount(const QModelIndex& parent) const
{
	return filtered_transactions.size();
}

int TransactionModel::columnCount(const QModelIndex& parent) const
{
	return Transaction::getFieldNames().size() + 1;
}

QVariant TransactionModel::data(const QModelIndex& index, int role) const
{
	if (role == Qt::DisplayRole)
	{
		if (index.column() == Transaction::getFieldNames().size())
		{
			if (std::dynamic_pointer_cast<TransactionGroup>(getTransaction(index.row()))) return "×";
			else return " ";
		}
		return getTransaction(index.row())->getFieldView(Transaction::getFieldNames().at(index.column()));
	}
	if (role == Qt::BackgroundRole)
	{
		if (index.column() < Transaction::getFieldNames().size() && Transaction::getFieldNames().at(index.column()) == TransactionFieldNames::Category)
		{
			return QBrush(categoryColors.at(getTransaction(index.row())->category));
		}
		else
		{
			const int intensity = std::min((std::abs(getTransaction(index.row())->amount.value)) / 40 + 20, 255);
			if (getTransaction(index.row())->amount.isNegative()) return QBrush(QColor(intensity, 0, 0, 150));
			else return QBrush(QColor(0, intensity, 0, 150));
		}
	}
	if (role == Qt::TextAlignmentRole)
	{
		if (index.column() == Transaction::getFieldNames().size()) return {Qt::AlignCenter | Qt::AlignVCenter};
		else return {Qt::AlignLeft | Qt::AlignVCenter};
	}
	return QVariant();
}

QVariant TransactionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole || orientation != Qt::Horizontal) return QVariant();
	if (section == Transaction::getFieldNames().size()) return "Group";
	return Transaction::getFieldNames().at(section);
}

void TransactionModel::removeTransaction(uint32_t idx)
{
	transactions.erase(transactions.begin() + getTransactionIndex(filtered_transactions.at(idx)));
	beginRemoveRows(QModelIndex(), idx, idx);
	filtered_transactions.erase(filtered_transactions.begin() + idx);
	endRemoveRows();
}

void TransactionModel::add(const std::shared_ptr<Transaction>& transaction)
{
	uint32_t idx = 0;
	while (idx < transactions.size() && *transaction < *transactions[idx]) idx++;
	transactions.insert(transactions.begin() + idx, transaction);
	if (!filter.check(*transaction)) return;
	idx = 0;
	while (idx < filtered_transactions.size() && *transaction < *filtered_transactions[idx]) idx++;
	beginInsertRows(QModelIndex(), idx, idx);
	filtered_transactions.insert(filtered_transactions.begin() + idx, transaction);
	endInsertRows();
}

std::shared_ptr<Transaction> TransactionModel::getTransaction(uint32_t idx) const
{
	return filtered_transactions.at(idx);
}

void TransactionModel::setTransaction(uint32_t idx, const std::shared_ptr<Transaction>& transaction)
{
	removeTransaction(idx);
	add(transaction);
}

const std::vector<std::shared_ptr<Transaction>>& TransactionModel::getUnfilteredTransactions() const
{
	return transactions;
}

void TransactionModel::clear()
{
	transactions.clear();
	beginResetModel();
	filtered_transactions.clear();
	endResetModel();
}

void TransactionModel::setFilterActive(bool active)
{
	filter.active = active;
	reset();
}

void TransactionModel::setFilter(const TransactionFilter& filter)
{
	this->filter = filter;
	reset();
}

const TransactionFilter& TransactionModel::getFilter() const
{
	return filter;
}

TransactionFilter& TransactionModel::getFilter()
{
	return filter;
}

bool TransactionModel::isEmpty() const
{
	return transactions.size() == 0;
}

QStringList TransactionModel::getUniqueValueList(const QString& fieldName) const
{
	std::set<QString> uniqueValues;
	for (const std::shared_ptr<Transaction> transaction : transactions) uniqueValues.emplace(transaction->getField(fieldName));
	QStringList values;
	for (const QString& value : uniqueValues) values.push_back(value);
	return values;
}

bool TransactionModel::getAutoCompleteTransaction(const QString& description, std::shared_ptr<const Transaction>& completedTransaction) const
{
	completedTransaction = nullptr;
	for (const std::shared_ptr<const Transaction> transaction : transactions)
	{
		if (transaction->getField(TransactionFieldNames::Description) == description)
		{
			completedTransaction = transaction;
			return true;
		}
	}
	return false;
}

void TransactionModel::addCategory(const QString& name, const QColor& color)
{
	categoryNames.push_back(name);
	categoryColors.emplace(name, color);
}

const QStringList& TransactionModel::getCategoryNames() const
{
	return categoryNames;
}

const std::unordered_map<QString, QColor>& TransactionModel::getCategoryColors() const
{
	return categoryColors;
}

void TransactionModel::setCategories(const QStringList& categoryNames, const std::unordered_map<QString, QColor>& categoryColors)
{
	this->categoryNames = categoryNames;
	this->categoryColors = categoryColors;
}

uint32_t TransactionModel::getTransactionIndex(std::shared_ptr<Transaction> transaction)
{
	for (uint32_t i = 0; i < transactions.size(); i++) if (transactions.at(i) == transaction) return i;
	return transactions.size();
}

void TransactionModel::reset()
{
	std::vector<std::shared_ptr<Transaction>> transactions_copy(transactions);
	clear();
	for (std::shared_ptr<Transaction> transaction : transactions_copy)
	{
		add(transaction);
	}
}
