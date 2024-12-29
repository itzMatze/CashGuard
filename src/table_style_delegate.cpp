#include "table_style_delegate.hpp"

TableStyleDelegate::TableStyleDelegate(QObject *parent) : QStyledItemDelegate(parent)
{}

void TableStyleDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItem opt = option;
	// disable hover highlighting
	opt.state &= ~QStyle::State_MouseOver;
	QStyledItemDelegate::paint(painter, opt, index);
}
