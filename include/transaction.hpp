#pragma once

#include <cstdint>
#include <qcolor.h>
#include <qobject.h>
#include <QDate>

struct Amount
{
	Amount() = default;
	Amount(int32_t value);
	Amount(QString stringValue);
	QString toString() const;
	bool isNegative() const;

	// amount in cents
	int32_t value;
};

bool operator<(const Amount& a, const Amount& b);
bool operator>(const Amount& a, const Amount& b);

namespace TransactionFieldNames
{
	const QString ID = "ID";
	const QString Date = "Date";
	const QString Category = "Category";
	const QString Amount = "Amount";
	const QString Description = "Description";
	const QString Added = "Added";
	const QString Edited = "Edited";
}

class Transaction
{
public:
	Transaction();
	virtual ~Transaction() = default;
	static QStringList getFieldNames();
	QString getField(const QString& fieldName) const;
	QString getFieldView(const QString& fieldName) const;
	void setField(const QString& fieldName, const QString& value);
	virtual QString toString() const;

	size_t id;
	QDate date;
	QString category;
	Amount amount;
	QString description;
	QDateTime added;
	QDateTime edited;
};

bool operator<(const Transaction& a, const Transaction& b);
bool operator==(const Transaction& a, const Transaction& b);

class TransactionGroup : public Transaction
{
public:
	TransactionGroup() = default;
	TransactionGroup(const Transaction& transaction);
	QString toString() const override;

	std::vector<std::shared_ptr<Transaction>> transactions;
};

bool operator==(const TransactionGroup& a, const TransactionGroup& b);
