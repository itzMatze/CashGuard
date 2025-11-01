#pragma once

#include <QStyledItemDelegate>
#include <QPainter>

class TransactionTableStyleDelegate : public QStyledItemDelegate
{
public:
	explicit TransactionTableStyleDelegate(QObject* parent);
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};
