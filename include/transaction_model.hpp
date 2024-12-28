#pragma once

#include "transaction.hpp"
#include <qabstractitemmodel.h>
#include <QAbstractTableModel>
#include <vector>

class TransactionModel : public QAbstractTableModel
{
    Q_OBJECT

public:
	explicit TransactionModel(QObject* parent = nullptr);
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	[[nodiscard]] bool saveToFile(const QString& path) const;
	[[nodiscard]] bool loadFromFile(const QString& path);
	void add(const Transaction& transaction);

private:
	std::vector<Transaction> transactions;
};
