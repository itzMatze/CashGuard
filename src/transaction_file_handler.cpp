#include "transaction_file_handler.hpp"
#include <set>
#include <QFile>
#include <QMessageBox>
#include "transaction_model.hpp"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "util/log.hpp"

const char* version_string = "0.1.0";

Transaction parseTransaction(const auto& rj_transaction)
{
	QStringList fields = Transaction::getFieldNames();
	Transaction transaction;
	for (const QString& field : fields)
	{
		if (!rj_transaction.HasMember(field.toStdString().c_str())) continue;
		transaction.setField(field, rj_transaction[field.toStdString().c_str()].GetString());
	}
	return transaction;
}

bool loadFromFile(const QString& filePath, TransactionModel& transactionModel)
{
	cglog::debug("Loading file");
	transactionModel.clear();
	QFile file(filePath);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    cglog::error("Failed to open file \"{}\"", filePath.toStdString());
    return false;
  };
	if (file.size() == 0) return true;
  QTextStream in(&file);
	QString fileContent = in.readAll();

	rapidjson::Document doc;
  doc.Parse(fileContent.toStdString().c_str());
  if (doc.HasParseError())
  {
    cglog::error("Failed to parse file \"{}\"", filePath.toStdString());
    return false;
  }

	if (std::string(doc["Version"].GetString()) != std::string(version_string))
	{
    cglog::error("Unsupported version in file \"{}\"", filePath.toStdString());
		return false;
	}

	transactionModel.addCategory("None", QColor());
	for (const auto& rj_category : doc["Categories"].GetArray()) transactionModel.addCategory(rj_category["Name"].GetString(), QColor(rj_category["Color"].GetString()));
	std::set<uint64_t> ids;
	for (const auto& rj_transaction : doc["Transactions"].GetArray())
	{
		Transaction transaction = parseTransaction(rj_transaction);
		if (!ids.emplace(transaction.id).second)
		{
			CG_THROW("Duplicate ID: {}", transaction.getField(TransactionFieldNames::ID).toStdString());
		}
		if (rj_transaction.HasMember("Transactions"))
		{
			std::shared_ptr<TransactionGroup> transaction_group = std::make_shared<TransactionGroup>(transaction);
			for (const auto& rj_sub_transaction : rj_transaction["Transactions"].GetArray())
			{
				transaction_group->transactions.push_back(std::make_shared<Transaction>(parseTransaction(rj_sub_transaction)));
			}
			transactionModel.add(transaction_group);
		}
		else
		{
			transactionModel.add(std::make_shared<Transaction>(transaction));
		}
	}
	return true;
}

void serializeTransaction(const Transaction& transaction, rapidjson::Value& jsonObject, rapidjson::Document::AllocatorType& allocator)
{
	jsonObject.SetObject();
	QStringList fields = Transaction::getFieldNames();
	for (const QString& field : fields)
	{
		rapidjson::Value name(field.toStdString().c_str(), allocator);
		rapidjson::Value value(transaction.getField(field).toStdString().c_str(), allocator);
		jsonObject.AddMember(name, value, allocator);
	}
}

bool saveToFile(const QString& filePath, const TransactionModel& transactionModel)
{
	rapidjson::Document doc;
	doc.SetObject();
  auto& allocator = doc.GetAllocator();
	rapidjson::Value value(version_string, allocator);
	doc.AddMember("Version", value, allocator);

	rapidjson::Value json_categories;
	json_categories.SetArray();
	for (const QString& category : transactionModel.getCategoryNames())
	{
		if (category == "None") continue;
		rapidjson::Value jsonObject;
		jsonObject.SetObject();
		{
			rapidjson::Value value(category.toStdString().c_str(), allocator);
			jsonObject.AddMember("Name", value, allocator);
		}
		{
			rapidjson::Value value(transactionModel.getCategoryColors().at(category).name(QColor::NameFormat::HexArgb).toStdString().c_str(), allocator);
			jsonObject.AddMember("Color", value, allocator);
		}
		json_categories.PushBack(jsonObject, allocator);
	}
	doc.AddMember("Categories", json_categories, allocator);

	rapidjson::Value json_transactions;
	json_transactions.SetArray();
	for (std::shared_ptr<Transaction> transaction : transactionModel.getUnfilteredTransactions())
	{
		if (std::shared_ptr<const TransactionGroup> transaction_group = std::dynamic_pointer_cast<const TransactionGroup>(transaction))
		{
			rapidjson::Value group;
			serializeTransaction(*transaction, group, allocator);
			rapidjson::Value json_sub_transactions;
			json_sub_transactions.SetArray();
			for (const std::shared_ptr<const Transaction> sub_transaction : transaction_group->transactions)
			{
				rapidjson::Value jsonObject;
				serializeTransaction(*sub_transaction, jsonObject, allocator);
				json_sub_transactions.PushBack(jsonObject, allocator);
			}
			group.AddMember("Transactions", json_sub_transactions, allocator);
			json_transactions.PushBack(group, allocator);
		}
		else
		{
			rapidjson::Value jsonObject;
			serializeTransaction(*transaction, jsonObject, allocator);
			json_transactions.PushBack(jsonObject, allocator);
		}
	}
	doc.AddMember("Transactions", json_transactions, allocator);

	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	doc.Accept(writer);

	QFile file(filePath);
	if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::Text)) return false;
	QTextStream out(&file);
	out << buffer.GetString();
	file.close();
	return true;
}
