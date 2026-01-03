#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QFile>

bool checkNpcapExist()
{
    WCHAR path[512];
    uint len = GetSystemDirectory(path, 480);
    QString systemPath = QString::fromWCharArray(path, len);

    QString npcapFolder = QDir(systemPath).filePath("Npcap");
    return QDir(npcapFolder).exists();
}

int main(int argc, char *argv[])
{
    QApplication::setStyle("fusion");
    QApplication a(argc, argv);

    QFile file(":/styles/style.qss");
    file.open(QIODevice::ReadOnly);

    a.setStyleSheet(QString::fromLatin1(file.readAll()));

    // 判断npcap是否存在
    if (!checkNpcapExist())
    {
        QMessageBox::critical(nullptr, "错误", "尚未安装npcap，请先安装！");
        return -1;
    }
    MainWindow w;
    w.show();
    return a.exec();
}
