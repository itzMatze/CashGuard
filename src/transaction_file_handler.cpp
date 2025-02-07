#include "transaction_file_handler.hpp"
#include <set>
#include <fstream>
#include <QFile>
#include <QMessageBox>
#include "transaction_model.hpp"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "util/log.hpp"

bool loadFile(const std::string& path, rapidjson::Document& document)
{
  std::ifstream file(path, std::ios::binary);
  if (!file.is_open())
  {
    cglog::error("Failed to open file \"{}\"", path);
    return false;
  };
  std::stringstream file_stream;
  file_stream << file.rdbuf();
  std::string file_content = file_stream.str();

  document.Parse(file_content.c_str());
  if (document.HasParseError())
  {
    cglog::error("Failed to parse file \"{}\"", path);
    return false;
  }
  return true;
}

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
	transactionModel.clear();
	rapidjson::Document doc;
	if (!loadFile(filePath.toStdString(), doc)) return false;
	std::set<uint64_t> ids;
	for (const auto& rj_transaction : doc.GetArray())
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
	doc.SetArray();
  auto& allocator = doc.GetAllocator();
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
			doc.PushBack(group, allocator);
		}
		else
		{
			rapidjson::Value jsonObject;
			serializeTransaction(*transaction, jsonObject, allocator);
			doc.PushBack(jsonObject, allocator);
		}
	}

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
