#pragma once

#include <QChartView>
#include <QValueAxis>
#include <QLineSeries>
#include <QScatterSeries>
#include <QMouseEvent>
#include <QToolTip>
#include <qdatetime.h>

class TooltipChartView : public QChartView
{
	Q_OBJECT;

public:
	explicit TooltipChartView(QWidget* parent = nullptr);
	void updateChart(QChart* newChart, QLineSeries* series, const QDate& startingDate);

protected:
	void mouseMoveEvent(QMouseEvent *event) override;

private:
	QLineSeries* series;
	QScatterSeries* marker;
	QDate startingDate;

	float findYValueFromX(float x) const;
};
