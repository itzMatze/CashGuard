#pragma once

#include <cstdint>
#include <qcolor.h>
#include <qobject.h>
#include <QDate>

struct Amount
{
	Amount() = default;
	Amount(int32_t value);
	Amount(QString string_value);
	QString to_string() const;
	bool is_negative() const;

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
	static QStringList get_field_names();
	QString get_field(const QString& field_name) const;
	QString get_field_view(const QString& field_name) const;
	void set_field(const QString& field_name, const QString& value);
	virtual QString to_string() const;

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
	QString to_string() const override;

	std::vector<std::shared_ptr<Transaction>> transactions;
};

bool operator==(const TransactionGroup& a, const TransactionGroup& b);
