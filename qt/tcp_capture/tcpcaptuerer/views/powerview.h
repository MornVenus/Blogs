#ifndef POWERVIEW_H
#define POWERVIEW_H

#include <QWidget>

namespace Ui {
class PowerView;
}

class PowerView : public QWidget
{
    Q_OBJECT

public:
    explicit PowerView(const QByteArray& data, QWidget *parent = nullptr);
    ~PowerView();

private:
    Ui::PowerView *ui;
    const QByteArray& m_data;
private:
    int getHexValue(const QByteArray& data, int index, int length);
};

#endif // POWERVIEW_H
