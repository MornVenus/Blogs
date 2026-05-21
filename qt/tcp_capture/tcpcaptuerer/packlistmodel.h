#ifndef PACKLISTMODEL_H
#define PACKLISTMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QDebug>
#include <QPixmap>
#include <QtSql>
#include <QDataWidgetMapper>

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
        m_db = QSqlDatabase::addDatabase("QSQLITE"); // 添加SQLITE数据库驱动
        m_db.setDatabaseName("captures.db");
        if (m_db.open())
        {
            qDebug() << "success to open the database";
        }
        else
            qDebug() << "fail to open the database";
    }

    ~PackListModel()
    {
        if (m_db.isValid() && m_db.isOpen())
        {
            m_db.close();
        }
    }
private:
    QList<PackModel> m_list;
    QSqlDatabase  m_db;   //数据库
    int _cnt{0};


public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        Q_UNUSED(parent);
        return _cnt;
    }

    QVariant data(const QModelIndex &index, int role) const override
    {
        Q_UNUSED(role)

        if (role != Qt::DisplayRole) return {};

        int row = index.row();
        if (row >= _cnt)
        {
            return {};
        }

        // 从数据库中读取
        QSqlQuery query;
        query.prepare("select * from packets where id = :id");
        query.bindValue(":id", row + 1);
        query.exec();
        if (query.isValid()) {
            qDebug() << "从数据库中获取到数据了";
        } else {
            qDebug() << "获取数据失败";
        }

        PackModel model = m_list.at(row);
        return QVariant::fromValue(model);
    }

    // bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override
    // {
    //     Q_UNUSED(role)
    //     if (index.row() <0 || index.row() >= m_list.size()) return false;
    //     PackModel model = value.value<PackModel>();
    //     m_list[index.row()] = model;
    //     return true;
    // }

    void clear()
    {
        _cnt = 0;
        beginResetModel();
        m_list.clear();
        endResetModel();
    }

    void append(PackModel model)
    {
        const int row = m_list.size();

        beginInsertRows(QModelIndex(), row, row);
        // 写入到数据库？ 只要维护一个 _cnt 就可以了？

        _cnt++;
        m_list.append(model);

        QSqlQuery query;
        query.prepare("insert into packets(role, time, type, len, file, start, finish)"
                      "values(:role, :time, :type, :len, :file, :start, :finish)");

        query.bindValue(":role", model.isSrc ? 0 : 1);
        query.bindValue(":time", model.time);
        query.bindValue(":type", static_cast<int>(1));
        query.bindValue(":len", model.length);
        query.bindValue(":file", 1);
        query.bindValue(":start", 0);
        query.bindValue(":finish", model.length - 1);

        if (!query.exec()) {
            qDebug() << "插入失败:" << query.lastError().text();
        }
        else {
            // _cnt++;
        }
        endInsertRows();
    }
};

#endif // PACKLISTMODEL_H
