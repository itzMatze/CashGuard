#include "total_amount.hpp"
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis>
#include <limits>

Amount getCurrentTotalAmount(const TransactionModel& transaction_model)
{
	int32_t total_amount = 0;
	for (uint32_t i = 0; i < transaction_model.rowCount(); i++) total_amount += transaction_model.getTransaction(i)->amount.value;
	return Amount(total_amount);
}

QDate roundUpMonth(const QDate& date)
{
	QDate newDate = date.addMonths(1);
	return QDate(newDate.year(), newDate.month(), 1);
}

QDate roundDownMonth(const QDate& date)
{
	return QDate(date.year(), date.month(), 1);
}

int64_t findMagnitude(int32_t value)
{
	int64_t magnitude = 10;
	while (magnitude < value) magnitude *= 10;
	return magnitude / 10;
}

std::pair<QChart*, QLineSeries*> getSmallTotalAmountChart(const TransactionModel& transactionModel)
{
	// show the last year rounded to always show full months
	QLineSeries* series = new QLineSeries();
	int32_t previousAmount = 0;
	int32_t amount = 0;
	for (int32_t i = transactionModel.rowCount() - 1; i >= 0; i--)
	{
		std::shared_ptr<Transaction> transaction = transactionModel.getTransaction(i);
		amount += transaction->amount.value;
		int32_t daysOld = transaction->date.daysTo(roundUpMonth(QDate::currentDate()));
		if (daysOld <= 365)
		{
			// draw line to old value to prevent diagonal lines
			series->append(365 - daysOld, double(previousAmount) / 100.0);
			series->append(365 - daysOld, double(amount) / 100.0);
		}
		previousAmount = amount;
	}

	QChart* chart = new QChart();
	chart->legend()->hide();
	chart->addSeries(series);
	chart->setMargins(QMargins(0, 0, 0, 0));
	chart->setBackgroundBrush(Qt::NoBrush);
	chart->setBackgroundPen(Qt::NoPen);
	chart->setPlotAreaBackgroundVisible(false);

	QCategoryAxis* xAxis = new QCategoryAxis();
	int32_t totalDays = 0;
	for (int32_t i = 0; i <= 12; ++i)
	{
		QDate oldDate = QDate::currentDate().addMonths(i-11);
		totalDays += oldDate.daysInMonth();
		xAxis->append(oldDate.toString("MMMM"), totalDays);
	}
	xAxis->setRange(0.0, 365.0);
	QPen xGridPen(Qt::darkGray);
	xGridPen.setStyle(Qt::DashLine);
	xGridPen.setWidth(1);
	xAxis->setGridLinePen(xGridPen);
	chart->addAxis(xAxis, Qt::AlignBottom);
	series->attachAxis(xAxis);

	QValueAxis* yAxis = new QValueAxis();
	QPen yGridPen(Qt::darkGray);
	yGridPen.setStyle(Qt::DashLine);
	yGridPen.setWidth(1);
	yAxis->setGridLinePen(yGridPen);
	yAxis->setLabelsVisible(false);
	chart->addAxis(yAxis, Qt::AlignLeft);
	series->attachAxis(yAxis);

	return {chart, series};
}

std::pair<QChart*, QLineSeries*> getTotalAmountChart(const TransactionModel& transactionModel)
{
	const QDate& dateMin = transactionModel.getFilter().dateMin;
	const QDate& dateMax = transactionModel.getFilter().dateMax;
	int32_t totalDays = dateMin.daysTo(dateMax);
	QLineSeries* series = new QLineSeries();
	int32_t previousAmount = 0;
	int32_t amount = 0;
	int32_t minAmount = std::numeric_limits<int32_t>::max();
	int32_t maxAmount = std::numeric_limits<int32_t>::min();
	for (int32_t i = transactionModel.rowCount() - 1; i >= 0; i--)
	{
		std::shared_ptr<Transaction> transaction = transactionModel.getTransaction(i);
		amount += transaction->amount.value;
		if (transaction->date <= dateMax && transaction->date >= dateMin)
		{
			int32_t daysOld = transaction->date.daysTo(dateMax);
			// draw line to old value to prevent diagonal lines
			series->append(totalDays - daysOld, double(previousAmount) / 100.0);
			series->append(totalDays - daysOld, double(amount) / 100.0);
			minAmount = std::min(minAmount, amount);
			maxAmount = std::max(maxAmount, amount);
		}
		previousAmount = amount;
	}

	QChart* chart = new QChart();
	chart->legend()->hide();
	chart->addSeries(series);
	chart->setMargins(QMargins(0, 0, 0, 0));
	chart->setBackgroundBrush(Qt::NoBrush);
	chart->setBackgroundPen(Qt::NoPen);
	chart->setPlotAreaBackgroundVisible(false);

	// x axis
	{
		QCategoryAxis* axis = new QCategoryAxis();
		axis->setLabelsColor(Qt::white);
		QFont font = axis->labelsFont();
		font.setPointSize(12);
		axis->setLabelsFont(font);
		QDate labelDate = dateMin;
		int32_t labelDays = dateMin.daysInMonth() - dateMin.day();
		while (labelDate < dateMax)
		{
			axis->append(labelDate.toString("MM.yy"), labelDays);
			labelDate = labelDate.addMonths(1);
			labelDays += labelDate.daysInMonth();
		}
		axis->setRange(0.0, totalDays);
		QPen gridPen(Qt::darkGray);
		gridPen.setStyle(Qt::DashLine);
		gridPen.setWidth(1);
		axis->setGridLinePen(gridPen);
		chart->addAxis(axis, Qt::AlignBottom);
		series->attachAxis(axis);
	}

	// y axis
	{
		QValueAxis* axis = new QValueAxis();
		axis->setLabelsColor(Qt::white);
		QFont font = axis->labelsFont();
		font.setPointSize(12);
		axis->setLabelsFont(font);

		int32_t magnitude = findMagnitude(std::max(std::abs(minAmount), std::abs(maxAmount)));
		minAmount = (minAmount / magnitude) * magnitude - magnitude;
		maxAmount = (maxAmount / magnitude) * magnitude + magnitude;
		axis->setRange(double(minAmount) / 100.0, double(maxAmount) / 100.0);

		QPen gridPen(Qt::darkGray);
		gridPen.setStyle(Qt::DashLine);
		gridPen.setWidth(1);
		axis->setGridLinePen(gridPen);
		axis->setLabelsVisible(true);
		chart->addAxis(axis, Qt::AlignLeft);
		series->attachAxis(axis);
	}

	return {chart, series};
}
