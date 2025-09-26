#pragma once

#include "transaction.hpp"
#include <QAbstractTableModel>

struct Account
{
	QString name;
	Amount amount;
};

class AccountModel : public QAbstractTableModel {
	Q_OBJECT;
public:
	explicit AccountModel(QObject *parent = nullptr);

	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role) override;
	void add(const Account& account);
	void removeAccount(int row);
	const std::vector<Account>& getData() const;
	Amount getTotalAmount() const;

private:
	std::vector<Account> accounts;
};
