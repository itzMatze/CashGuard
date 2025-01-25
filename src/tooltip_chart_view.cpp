#include "tooltip_chart_view.h"
#include <QChartView>
#include <QValueAxis>
#include <QLineSeries>
#include <QMouseEvent>
#include <QToolTip>

TooltipChartView::TooltipChartView(QWidget *parent) : QChartView(parent), series(nullptr), marker(new QScatterSeries(parent))
{}

void TooltipChartView::updateChart(QChart* newChart, QLineSeries* series, const QDate& startingDate)
{
	this->startingDate = startingDate;
	newChart->addSeries(marker);
	marker->setMarkerSize(8);
	marker->setColor(Qt::red);
	marker->setBorderColor(Qt::red);
	marker->attachAxis(newChart->axes().at(0));
	marker->attachAxis(newChart->axes().at(1));
	setChart(newChart);
	this->series = series;
}

void TooltipChartView::mouseMoveEvent(QMouseEvent *event)
{
	QPointF chartPos = chart()->mapToValue(event->pos());
	float x = chartPos.x();
	float y = findYValueFromX(x);
	QPoint tooltipPos(event->globalPosition().x(), event->globalPosition().y());
	QToolTip::showText(tooltipPos, QString("%1: %2").arg(QDate(startingDate.addDays(x)).toString("dd.MM.yyyy")).arg(y));
	marker->clear();
	marker->append(x, y);

	QChartView::mouseMoveEvent(event);
}

float TooltipChartView::findYValueFromX(float x) const
{
	if (series->count() == 0) return 0;
	if (series->count() == 1) return series->at(0).y();
	QPointF prevPoint = series->at(0);
	for (int i = 1; i < series->count(); i++)
	{
		QPointF currentPoint = series->at(i);
		if (x >= prevPoint.x() && x <= currentPoint.x())
		{
			float t = (x - prevPoint.x()) / (currentPoint.x() - prevPoint.x());
			return prevPoint.y() * (1.0 - t) + currentPoint.y() * t;
		}
		prevPoint = currentPoint;
	}

	// x is out of range, return the nearest y-value
	return (x < series->at(0).x()) ? series->at(0).y() : series->at(series->count() - 1).y();
}
