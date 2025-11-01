#include "transaction_table_style_delegate.hpp"

TransactionTableStyleDelegate::TransactionTableStyleDelegate(QObject* parent) : QStyledItemDelegate(parent)
{}

void TransactionTableStyleDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QStyleOptionViewItem opt = option;
	// disable hover highlighting
	opt.state &= ~QStyle::State_MouseOver;
	// disable focus highlight
	opt.state &= ~QStyle::State_HasFocus;
	initStyleOption(&opt, index);
	QStyledItemDelegate::paint(painter, opt, index);
}
