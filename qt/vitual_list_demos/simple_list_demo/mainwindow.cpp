#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QStringListModel>
#include <QList>
#include <QStringList>
#include "student.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QList<Student> studentList;
    studentList.append(Student("smx", 30));
    studentList.append(Student("venus", 31));
    studentList.append(Student("star", 32));

    QStringList strList;
    for (auto& stu : studentList)
    {
        strList << QString("name: %1, age: %2").arg(stu.getName()).arg(stu.getAge());
    }

    QStringListModel* listModel = new QStringListModel(strList);
    ui->listView->setModel(listModel);
}

MainWindow::~MainWindow()
{
    delete ui;
}
