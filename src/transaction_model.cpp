#include "transaction_model.hpp"
#include <QFile>
#include <QMessageBox>
#include <qabstractitemmodel.h>


TransactionModel::TransactionModel(const QString& filePath, QObject* parent) : filePath(filePath), QAbstractTableModel(parent), dirty(false)
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
	if (role == Qt::DisplayRole) return transactions[index.row()].getFieldView(Transaction::getFieldNames().at(index.column()));
	return QVariant();
}

QVariant TransactionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole || orientation != Qt::Horizontal) return QVariant();
	return Transaction::getFieldNames().at(section);
}

bool TransactionModel::loadFromFile()
{
	beginResetModel();
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
	transactions.clear();
	QTextStream in(&file);
	// skip header
	QStringList fields = in.readLine().split(',');
	while (!in.atEnd())
	{
		QStringList cells = in.readLine().split(',');
		Transaction transaction;
		for (uint32_t i = 0; i < fields.size(); i++)
		{
			transaction.setField(fields.at(i), cells.at(i));
		}
		transactions.push_back(transaction);
	}
	std::sort(transactions.begin(), transactions.end(), [](const Transaction& a, const Transaction& b){ return !(a < b); });

	file.close();
	endResetModel();
	return true;
}

bool TransactionModel::saveToFile()
{
	if (!dirty) return true;
	QFile file(filePath);
	if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text)) return false;
	QTextStream out(&file);

	QStringList fields = Transaction::getFieldNames();
	QStringList outLine;
	for (const QString& field : fields) outLine.append(field);
	out << outLine.join(',') << "\n";
	for (const Transaction& t : transactions)
	{
		outLine.clear();
		for (uint32_t i = 0; i < fields.size(); i++) outLine.append(t.getField(fields.at(i)));
		out << outLine.join(',') << "\n";
	}

	file.close();
	dirty = false;
	return true;
}

void TransactionModel::removeTransaction(uint32_t idx)
{
	beginRemoveRows(QModelIndex(), idx, idx);
	transactions.erase(transactions.begin() + idx);
	endRemoveRows();
	dirty = true;
}

void TransactionModel::add(const Transaction& transaction)
{
	uint32_t idx = 0;
	while (idx < transactions.size() && transaction < transactions[idx]) idx++;
	beginInsertRows(QModelIndex(), idx, idx);
	transactions.insert(transactions.begin() + idx, transaction);
	endInsertRows();
	dirty = true;
}

Transaction TransactionModel::getTransaction(uint32_t idx) const
{
	return transactions.at(idx);
}

void TransactionModel::setTransaction(uint32_t idx, const Transaction& transaction)
{
	removeTransaction(idx);
	add(transaction);
	dirty = true;
}

bool TransactionModel::isDirty()
{
	return dirty;
}
