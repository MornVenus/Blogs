#include "powerview.h"
#include "ui_powerview.h"
#include <QSpan>
#include <QDataStream>
#include <QTableWidgetItem>
#include <QAbstractItemView>

PowerView::PowerView(const QByteArray& data, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PowerView), m_data(data)
{
    ui->setupUi(this);

    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // QSpan<const quint8> dataSpan( reinterpret_cast<const quint8*>(data.constData()),  data.size());
    if (data.length() > 24)
    {
        int index = data.at(21);
        ui->indexLabel->setText(QString::number(index));
        // 15个
        for (int i = 22; i < data.length(); i += 15)
        {
            if (i + 15 >= data.length()) break;

            int row = ui->tableWidget->rowCount();
            ui->tableWidget->insertRow(row);


            int channel = data.at(i);

            int setV = getHexValue(data, i + 1, 2);
            double setVD = setV / 1000.0;
            // qDebug() << QString("v0: %1 v1: %2").arg((int)data.at(i + 1)).arg((int)data.at(i + 2));

            int ocp = getHexValue(data, i + 3, 2);
            double ocpD = ocp / 10.0;

            int waitTime = (uchar)data.at(i + 5);

            int ovp = getHexValue(data, i + 6, 2);
            double ovpD = ovp / 10.0;

            int uvp = getHexValue(data, i + 8, 2);
            double uvpD = uvp / 10.0;

            int delay = (uchar)data.at(i + 10) * 10;

            int stepVolt = getHexValue(data, i + 11, 2);
            double stepVoltD = stepVolt / 1000.0;

            int stepDelay = getHexValue(data, i + 13, 2);

            ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(channel)));
            ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(setVD)));
            ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(ocpD)));
            ui->tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(waitTime)));
            ui->tableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(ovpD)));
            ui->tableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(uvpD)));
            ui->tableWidget->setItem(row, 6, new QTableWidgetItem(QString::number(delay)));
            ui->tableWidget->setItem(row, 7, new QTableWidgetItem(QString::number(stepVoltD)));
            ui->tableWidget->setItem(row, 8, new QTableWidgetItem(QString::number(stepDelay)));

            // QSpan<const quint8> setVSpan = dataSpan.subspan(i + 1, 2);

            // QDataStream stream(setVSpan);

            // double setv =
        }
    }
}

PowerView::~PowerView()
{
    delete ui;
}

int PowerView::getHexValue(const QByteArray &data, int index, int length)
{
    int value = 0;
    for (int i = index + length - 1; i >=index; i--)
    {
        uchar val = (uchar)data.at(i);
        value = val  + value * 256;
    }
    return value;
}
