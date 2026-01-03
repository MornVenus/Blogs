#ifndef MYFILTERMODEL_H
#define MYFILTERMODEL_H
#include <QSortFilterProxyModel>
#include "packlistmodel.h"
#include <QDebug>

class MyFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit MyFilterModel(QObject* parent = nullptr): QSortFilterProxyModel(parent)
    {

    }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override
    {
        QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

        auto model = index.data().value<PackModel>();
        bool visible = model.datType != "Initialize(0x13)";
        return visible;
    }
};

#endif // MYFILTERMODEL_H
