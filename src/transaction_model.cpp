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
	return Transaction::get_field_names().size() + 1;
}

QVariant TransactionModel::data(const QModelIndex& index, int role) const
{
	if (role == Qt::DisplayRole)
	{
		if (index.column() == Transaction::get_field_names().size())
		{
			if (std::dynamic_pointer_cast<TransactionGroup>(get_transaction(index.row()))) return "×";
			else return " ";
		}
		return get_transaction(index.row())->get_field_view(Transaction::get_field_names().at(index.column()));
	}
	if (role == Qt::BackgroundRole)
	{
		if (index.column() < Transaction::get_field_names().size() && Transaction::get_field_names().at(index.column()) == TransactionFieldNames::Category)
		{
			return QBrush(category_colors.at(get_transaction(index.row())->category));
		}
		else
		{
			const int intensity = std::min((std::abs(get_transaction(index.row())->amount.value)) / 40 + 20, 255);
			if (get_transaction(index.row())->amount.is_negative()) return QBrush(QColor(intensity, 0, 0, 150));
			else return QBrush(QColor(0, intensity, 0, 150));
		}
	}
	if (role == Qt::TextAlignmentRole)
	{
		if (index.column() == Transaction::get_field_names().size()) return {Qt::AlignCenter | Qt::AlignVCenter};
		else return {Qt::AlignLeft | Qt::AlignVCenter};
	}
	return QVariant();
}

QVariant TransactionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole || orientation != Qt::Horizontal) return QVariant();
	if (section == Transaction::get_field_names().size()) return "Group";
	return Transaction::get_field_names().at(section);
}

void TransactionModel::remove_transaction(uint32_t idx)
{
	transactions.erase(transactions.begin() + get_transaction_index(filtered_transactions.at(idx)));
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

std::shared_ptr<Transaction> TransactionModel::get_transaction(uint32_t idx) const
{
	return filtered_transactions.at(idx);
}

void TransactionModel::set_transaction(uint32_t idx, const std::shared_ptr<Transaction>& transaction)
{
	remove_transaction(idx);
	add(transaction);
}

const std::vector<std::shared_ptr<Transaction>>& TransactionModel::get_unfiltered_transactions() const
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

void TransactionModel::set_filter_active(bool active)
{
	filter.active = active;
	reset();
}

void TransactionModel::set_filter(const TransactionFilter& filter)
{
	this->filter = filter;
	reset();
}

const TransactionFilter& TransactionModel::get_filter() const
{
	return filter;
}

TransactionFilter& TransactionModel::get_filter()
{
	return filter;
}

bool TransactionModel::is_empty() const
{
	return transactions.size() == 0;
}

QStringList TransactionModel::get_unique_value_list(const QString& fieldName) const
{
	std::set<QString> unique_values;
	for (const std::shared_ptr<Transaction> transaction : transactions) unique_values.emplace(transaction->get_field(fieldName));
	QStringList values;
	for (const QString& value : unique_values) values.push_back(value);
	return values;
}

bool TransactionModel::get_auto_complete_transaction(const QString& description, std::shared_ptr<const Transaction>& completedTransaction) const
{
	completedTransaction = nullptr;
	for (const std::shared_ptr<const Transaction> transaction : transactions)
	{
		if (transaction->get_field(TransactionFieldNames::Description) == description)
		{
			completedTransaction = transaction;
			return true;
		}
	}
	return false;
}

void TransactionModel::add_category(const QString& name, const QColor& color)
{
	category_names.push_back(name);
	category_colors.emplace(name, color);
}

const QStringList& TransactionModel::get_category_names() const
{
	return category_names;
}

const std::unordered_map<QString, QColor>& TransactionModel::get_category_colors() const
{
	return category_colors;
}

void TransactionModel::set_categories(const QStringList& categoryNames, const std::unordered_map<QString, QColor>& categoryColors)
{
	this->category_names = categoryNames;
	this->category_colors = categoryColors;
}

uint32_t TransactionModel::get_transaction_index(std::shared_ptr<Transaction> transaction)
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
