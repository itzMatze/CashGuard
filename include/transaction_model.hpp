#pragma once

#include "transaction.hpp"
#include <qabstractitemmodel.h>
#include <QAbstractTableModel>
#include <vector>

class TransactionModel : public QAbstractTableModel
{
    Q_OBJECT

public:
	explicit TransactionModel(const QString& filePath, QObject* parent = nullptr);
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	[[nodiscard]] bool saveToFile();
	[[nodiscard]] bool loadFromFile();
	void add(const Transaction& transaction);
	void removeTransaction(uint32_t idx);
	Transaction getTransaction(uint32_t idx) const;
	void setTransaction(uint32_t idx, const Transaction& transaction);
	bool isDirty();

private:
	std::vector<Transaction> transactions;
	QString filePath;
	bool dirty = false;
};
