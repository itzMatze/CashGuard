#pragma once

#include "transaction.hpp"
#include <QAbstractTableModel>

struct Account
{
	QString name;
	Amount amount;
};

class AccountModel : public QAbstractTableModel
{
	Q_OBJECT;

public:
	explicit AccountModel(QObject* parent);

	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role) override;
	void add(const Account& account);
	void remove_account(int row);
	const std::vector<Account>& get_data() const;
	Amount get_total_amount() const;

private:
	std::vector<Account> accounts;
};
