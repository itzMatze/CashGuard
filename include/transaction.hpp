#pragma once

#include <cstdint>
#include <qcolor.h>
#include <qobject.h>
#include <QDate>

struct Category
{
public:
	enum CategoryEnum : int
	{
		Cosmetics = 0,
		Cash = 1,
		Clothing = 2,
		Education = 3,
		Gaming = 4,
		Groceries = 5,
		Health = 6,
		Internet = 7,
		Leisure = 8,
		Living = 9,
		Other_Income = 10,
		Other_Outgoing = 11,
		Present = 12,
		Public_Transport = 13,
		Restaurant = 14,
		Salary = 15,
		Saving = 16,
		Sport = 17,
		Transport = 18,
		Vacation = 19,
		None = 20
	};

	static QStringList getCategoryNames();
	Category(const QString& name);
	Category(CategoryEnum value);

	CategoryEnum getType() const;
	QString getName() const;
	QColor getColor() const;

private:
	static QColor getColor(CategoryEnum value);
	static CategoryEnum getTypeFromName(const QString& name);

	CategoryEnum type;
	QString name;
	QColor color;
};

bool operator==(const Category& a, const Category& b);

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
	Category category;
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
