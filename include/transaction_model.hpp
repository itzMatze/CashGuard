#pragma once

#include "transaction.hpp"
#include "transaction_filter.hpp"
#include <qabstractitemmodel.h>
#include <QAbstractTableModel>
#include <vector>

class TransactionModel : public QAbstractTableModel
{
    Q_OBJECT;

public:
	explicit TransactionModel(QObject* parent = nullptr);
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	void add(const std::shared_ptr<Transaction>& transaction);
	void removeTransaction(uint32_t idx);
	std::shared_ptr<Transaction> getTransaction(uint32_t idx) const;
	void setTransaction(uint32_t idx, const std::shared_ptr<Transaction>& transaction);
	const std::vector<std::shared_ptr<Transaction>>& getUnfilteredTransactions() const;
	void clear();
	void setFilterActive(bool active);
	void setFilter(const TransactionFilter& newFilter);
	const TransactionFilter& getFilter() const;
	// use with caution, changes to the filter will not trigger a filter update
	TransactionFilter& getFilter();
	bool isEmpty() const;

private:
	std::vector<std::shared_ptr<Transaction>> transactions;
	std::vector<std::shared_ptr<Transaction>> filtered_transactions;
	TransactionFilter filter;

	uint32_t getTransactionIndex(std::shared_ptr<Transaction> transaction);
	void reset();
};
