#ifndef PACKDELEGATE_H
#define PACKDELEGATE_H
#include <QWidget>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QDebug>
#include "packlistmodel.h"

class PackDelegate: public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit PackDelegate(QObject* parent = nullptr):QStyledItemDelegate{parent}
    {

    }

public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QRect rect(option.rect.left() + 2, option.rect.top() + 2, option.rect.width() - 4, option.rect.height() - 4);

        auto model = index.data().value<PackModel>();
        // if (model.datType == "Initialize(0x13)")
        // {
        //     return;
        // }
        painter->save();

        if (model.isSrc)
        {
            if (option.state & QStyle::State_MouseOver || option.state & QStyle::State_Selected)
            {
                painter->fillRect(rect, QColor(61, 139, 253));
            }
            else
            {
                painter->fillRect(rect, QColor(13, 110, 253));
            }
        }
        else
        {
            if (option.state & QStyle::State_MouseOver || option.state & QStyle::State_Selected)
            {
                painter->fillRect(rect, QColor(137, 145, 151));
            }
            else
            {
                painter->fillRect(rect, QColor(108, 117, 125));
            }
        }

        if (model.isValid)
            painter->setPen(Qt::white);
        else
            painter->setPen(Qt::red);

        QRect avatarRect(10, rect.top() + 4, 24, 24);
        QPixmap pixmap(QString(":/images/images/%1.png").arg(model.avatar));
        painter->drawPixmap(avatarRect, pixmap);

        QRect timeRect(avatarRect.right() + 10, rect.top(),
                       100, rect.height());
        painter->drawText(timeRect, Qt::AlignLeft | Qt::AlignVCenter, model.time);

        QRect dataTypeRect(timeRect.right() + 10, rect.top(),
                           120, rect.height());
        painter->drawText(dataTypeRect, Qt::AlignLeft | Qt::AlignVCenter, model.datType);

        QRect lenRect(dataTypeRect.right() + 10, rect.top(),
                      60, rect.height());
        painter->drawText(lenRect, Qt::AlignLeft | Qt::AlignVCenter, QString::number(model.length));

        painter->restore();
    }

    QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex & index) const override {
        auto model = index.data().value<PackModel>();
        return QSize(240, 36);
    }
};

#endif // PACKDELEGATE_H
