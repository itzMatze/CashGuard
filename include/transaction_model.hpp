#pragma once

#include "transaction_filter/transaction_filter.hpp"
#include <QAbstractTableModel>
#include <unordered_map>
#include <vector>

class TransactionModel : public QAbstractTableModel
{
	Q_OBJECT;

public:
	explicit TransactionModel(QObject* parent);
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	void add(const std::shared_ptr<Transaction>& transaction);
	void remove_transaction(uint32_t idx);
	std::shared_ptr<Transaction> get_transaction(uint32_t idx) const;
	void set_transaction(uint32_t idx, const std::shared_ptr<Transaction>& transaction);
	const std::vector<std::shared_ptr<Transaction>>& get_unfiltered_transactions() const;
	void clear();
	void set_filter_active(bool active);
	void set_filter(const TransactionFilter& new_filter);
	const TransactionFilter& get_filter() const;
	// use with caution, changes to the filter will not trigger a filter update
	TransactionFilter& get_filter();
	bool is_empty() const;
	QStringList get_unique_value_list(const QString& field_name) const;
	bool get_auto_complete_transaction(const QString& description, std::shared_ptr<const Transaction>& completed_transaction) const;
	void add_category(const QString& name, const QColor& color);
	const QStringList& get_category_names() const;
	const std::unordered_map<QString, QColor>& get_category_colors() const;
	void set_categories(const QStringList& category_names, const std::unordered_map<QString, QColor>& category_colors);

private:
	QStringList category_names;
	std::unordered_map<QString, QColor> category_colors;
	std::vector<std::shared_ptr<Transaction>> transactions;
	std::vector<std::shared_ptr<Transaction>> filtered_transactions;
	TransactionFilter filter;

	uint32_t get_transaction_index(std::shared_ptr<Transaction> transaction);
	void reset();
};
