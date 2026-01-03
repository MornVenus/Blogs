#ifndef PACKLISTMODEL_H
#define PACKLISTMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QDebug>
#include <QPixmap>

struct PackModel
{
    QString avatar;
    QString time;
    int length;
    QString datType;
    bool isSrc;
    bool isValid = true;
};

Q_DECLARE_METATYPE(PackModel)

class PackListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit PackListModel(QObject *parent = nullptr):QAbstractListModel{parent}
    {

    }
private:
    QList<PackModel> m_list;


public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        Q_UNUSED(parent);
        return m_list.size();
    }

    void clear()
    {
        m_list.clear();
    }

    QVariant data(const QModelIndex &index, int role) const override
    {
        Q_UNUSED(role)
        int row = index.row();
        if (row >= m_list.size())
        {
            return QVariant();
        }
        // return m_list.at(row);
        PackModel model = m_list.at(row);
        return QVariant::fromValue(model);
    }

    // QAbstractItemModel interface
public:
    // QModelIndex index(int row, int column, const QModelIndex &parent) const override
    // {
    //     QModelIndex index(row, column, parent);
    //     return index;
    // }
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override
    {
        Q_UNUSED(role)
        if (index.row() <0 || index.row() >= m_list.size()) return false;
        PackModel model = value.value<PackModel>();
        m_list[index.row()] = model;
        return true;
    }
    bool insertRows(int row, int count, const QModelIndex &parent) override
    {
        if (row < 0 || row > m_list.size()) return false;
        beginInsertRows(parent, row, row + count - 1);
        m_list.insert(row, PackModel());
        endInsertRows();
        return true;
    }
};

#endif // PACKLISTMODEL_H
