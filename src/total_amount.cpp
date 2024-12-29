#include "total_amount.hpp"

Amount getCurrentTotalAmount(TransactionModel& transaction_model)
{
	int32_t total_amount = 0;
	for (uint32_t i = 0; i < transaction_model.rowCount(); i++) total_amount += transaction_model.getTransaction(i).amount.value;
	return Amount(total_amount);
}
