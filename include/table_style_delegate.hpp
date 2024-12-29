#pragma once

#include <QStyledItemDelegate>
#include <QPainter>

class TableStyleDelegate : public QStyledItemDelegate {
public:
	explicit TableStyleDelegate(QObject *parent = nullptr);
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};
