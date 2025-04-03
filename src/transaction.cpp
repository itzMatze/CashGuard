#include "transaction.hpp"
#include <cstdint>
#include <qcolor.h>
#include <qobject.h>
#include <QDate>
#include <qregularexpression.h>
#include <qvalidator.h>
#include "util/log.hpp"

Category::Category(CategoryEnum value) : type(value), name(getCategoryNames()[value]), color(getColor(value))
{}


Category::Category(const QString& name) : type(getTypeFromName(name)), name(name), color(getColor(type))
{}

Category::CategoryEnum Category::getTypeFromName(const QString& name)
{
	const QStringList categoryNames = getCategoryNames();
	for (int i = 0; i < categoryNames.size(); i++)
	{
		if (categoryNames.at(i) == name) return CategoryEnum(i);
	}
	return CategoryEnum::None;
}

Category::CategoryEnum Category::getType() const
{
	return type;
}

QString Category::getName() const
{
	return name;
}

QColor Category::getColor() const
{
	return color;
}

QColor Category::getColor(CategoryEnum value)
{
	if (Cosmetics == value) return QColor(128, 0, 128, 128);
	else if (Cash == value) return QColor(0, 255, 0, 128);
	else if (Clothing == value) return QColor(0, 0, 255, 128);
	else if (Education == value) return QColor(0, 255, 255, 128);
	else if (Gaming == value) return QColor(255, 0, 100, 128);
	else if (Groceries == value) return QColor(255, 255, 0, 128);
	else if (Health == value) return QColor(128, 0, 0, 128);
	else if (Internet == value) return QColor(0, 128, 0, 128);
	else if (Leisure == value) return QColor(0, 128, 128, 128);
	else if (Living == value) return QColor(0, 128, 0, 128);
	else if (Other_Income == value) return QColor(0, 255, 0, 128);
	else if (Other_Outgoing == value) return QColor(255, 0, 0, 128);
	else if (Present == value) return QColor(128, 0, 255, 128);
	else if (Public_Transport == value) return QColor(255, 0, 0, 128);
	else if (Restaurant == value) return QColor(0, 0, 128, 128);
	else if (Salary == value) return QColor(0, 255, 0, 128);
	else if (Saving == value) return QColor(0, 0, 255, 128);
	else if (Sport == value) return QColor(128, 0, 0, 128);
	else if (Transport == value) return QColor(255, 0, 0, 128);
	else if (Vacation == value) return QColor(255, 0, 255, 128);
	else if (None == value) return QColor();
	return QColor();
}

QStringList Category::getCategoryNames()
{
	static QStringList categoryNames(None+1);
	categoryNames[Cosmetics] = "Cosmetics";
	categoryNames[Cash] = "Cash";
	categoryNames[Clothing] = "Clothing";
	categoryNames[Education] = "Education";
	categoryNames[Gaming] = "Gaming";
	categoryNames[Groceries] = "Groceries";
	categoryNames[Health] = "Health";
	categoryNames[Internet] = "Internet";
	categoryNames[Leisure] = "Leisure";
	categoryNames[Living] = "Living";
	categoryNames[Other_Income] = "Other Income";
	categoryNames[Other_Outgoing] = "Other Outgoing";
	categoryNames[Present] = "Present";
	categoryNames[Public_Transport] = "Public Transport";
	categoryNames[Restaurant] = "Restaurant";
	categoryNames[Salary] = "Salary";
	categoryNames[Saving] = "Saving";
	categoryNames[Sport] = "Sport";
	categoryNames[Transport] = "Transport";
	categoryNames[Vacation] = "Vacation";
	categoryNames[None] = "None";
	return categoryNames;
}

bool operator==(const Category& a, const Category& b)
{
	return a.getType() == b.getType();
}

Amount::Amount(int32_t value) : value(value)
{}

Amount::Amount(QString stringValue)
{
	stringValue.remove(QRegularExpression(" "));
	stringValue.remove(QRegularExpression("€"));
	QRegularExpressionValidator validator(QRegularExpression(R"((\+|-)?\b\d+(\.\d{1,2})?\b)"));
	int pos = 0;
	CG_ASSERT(validator.validate(stringValue, pos), "Invalid amount format!");
	bool negative = true;
	if (stringValue.startsWith("+")) negative = false;
	stringValue.remove(QRegularExpression(R"(^(\+|-))"));
	if (!stringValue.contains('.'))
	{
		value = 100 * stringValue.toInt();
	}
	else
	{
		QStringList stringValues = stringValue.split('.');
		int32_t euro = stringValues.at(0).toInt();
		int32_t cent = stringValues.at(1).leftJustified(2, '0').toInt();
		value = cent + 100 * euro;
	}
	if (negative) value = -value;
}

QString Amount::toString() const
{
	return (isNegative() ? "-" : "+") + QString::number(std::abs(value) / 100) + "." + QString::number(std::abs(value) % 100).rightJustified(2, '0');
}

bool Amount::isNegative() const
{
	return value < 0;
}

bool operator<(const Amount& a, const Amount& b)
{
	return a.value < b.value;
}

bool operator>(const Amount& a, const Amount& b)
{
	return a.value > b.value;
}

Transaction::Transaction() :
	id(0),
	date(QDate::currentDate()),
	category(Category::None),
	amount(0),
	description(""),
	added(QDateTime::currentDateTime()),
	edited(QDateTime::currentDateTime())
{}

QStringList Transaction::getFieldNames()
{
	namespace tfn = TransactionFieldNames;
	return QStringList({
		tfn::ID,
		tfn::Date,
		tfn::Category,
		tfn::Amount,
		tfn::Description,
		tfn::Added,
		tfn::Edited});
}

QString Transaction::getField(const QString& fieldName) const
{
	namespace tfn = TransactionFieldNames;
	if (fieldName == tfn::ID) return QString::number(id);
	else if (fieldName == tfn::Date) return date.toString("dd.MM.yyyy");
	else if (fieldName == tfn::Category) return category.getName();
	else if (fieldName == tfn::Amount) return amount.toString();
	else if (fieldName == tfn::Description) return description;
	else if (fieldName == tfn::Added) return added.toString("dd.MM.yyyy HH:mm:ss");
	else if (fieldName == tfn::Edited) return edited.toString("dd.MM.yyyy HH:mm:ss");
	else CG_THROW("Invalid transaction field name!");
}

QString Transaction::getFieldView(const QString& fieldName) const
{
	namespace tfn = TransactionFieldNames;
	QString output = getField(fieldName);
	if (fieldName == tfn::Amount) output += " €";
	return output;
}

void Transaction::setField(const QString& fieldName, const QString& value)
{
	namespace tfn = TransactionFieldNames;
	if (fieldName == tfn::ID) id = value.toULongLong();
	else if (fieldName == tfn::Date) date = QDate::fromString(value, "dd.MM.yyyy");
	else if (fieldName == tfn::Category) category = value;
	else if (fieldName == tfn::Amount) amount = Amount(value);
	else if (fieldName == tfn::Description) description = value;
	else if (fieldName == tfn::Added) added = QDateTime::fromString(value, "dd.MM.yyyy HH:mm:ss");
	else if (fieldName == tfn::Edited) edited = QDateTime::fromString(value, "dd.MM.yyyy HH:mm:ss");
	else CG_THROW("Invalid transaction field name!");
}

QString Transaction::toString() const
{
	QString output = "";
	for (const QString& field : getFieldNames())
	{
		output += field + ": " + getField(field) + "; ";
	}
	return output;
}

bool operator<(const Transaction& a, const Transaction& b)
{
	if (a.date == b.date)
	{
		if (a.added == b.added) return a.id < b.id;
		else return a.added < b.added;
	}
	else return a.date < b.date;
}

bool operator==(const Transaction& a, const Transaction& b)
{
	QStringList fields = Transaction::getFieldNames();
	for (const QString& field : fields)
	{
		if (a.getField(field) != b.getField(field)) return false;
	}
	return true;
}

TransactionGroup::TransactionGroup(const Transaction& transaction) : Transaction(transaction)
{}

QString TransactionGroup::toString() const
{
	return Transaction::toString() + "Transaction Group";
}

bool operator==(const TransactionGroup& a, const TransactionGroup& b)
{
	QStringList fields = Transaction::getFieldNames();
	for (const QString& field : fields)
	{
		if (a.getField(field) != b.getField(field)) return false;
	}
	if (a.transactions.size() != b.transactions.size()) return false;
	for (int32_t i = 0; i < a.transactions.size(); i++)
	{
		if (*a.transactions[i] != *b.transactions[i]) return false;
	}
	return true;
}
