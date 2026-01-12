#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    if (m_senderFile)
    {
        m_senderFile->close();
        m_senderFile = nullptr;
    }
    if (ui->startStopBtn->isChecked())
    {
        service->stopCapture();
    }
    service->wait(); // 没有这个会出现奔溃 因为相当于主线程退出了，子线程还没结束。
    delete ui;
}

void MainWindow::init()
{
    service = new TcpCaptureService(this);
    QStringList devNames = service->getDeviceNames();
    ui->comboBoxNetAdapter->addItems(devNames);
    ui->comboBoxNetAdapter->setCurrentIndex(devNames.length() - 1);
    packDataService = new PackDataService(this);

    ui->tcpListView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(service, qOverload<Packet*>(&TcpCaptureService::packetReceived), packDataService, &PackDataService::append);
    connect(packDataService, &PackDataService::packetReceived, this, &MainWindow::onPacketReceived);
    connect(ui->tcpListView, &QListView::customContextMenuRequested, this, &MainWindow::on_tcp_listview_contextmenu);

    tcpListModel = new PackListModel(this);
    ui->tcpListView->setModel(tcpListModel);

    delegate = new PackDelegate(this);
    ui->tcpListView->setItemDelegate(delegate);

    // m_filterProxy = new MyFilterModel(this);
    // m_filterProxy->setSourceModel(tcpListModel);

    // ui->tcpListView->setModel(m_filterProxy);

    selModel = new QItemSelectionModel(tcpListModel);
    ui->tcpListView->setSelectionModel(selModel);

    connect(selModel, &QItemSelectionModel::currentRowChanged, this, &MainWindow::do_currentRow_changed);

    initAvatars();

    m_senderFile = new QFile("sender.dat", this);
    m_senderFile->open(QIODevice::ReadWrite);
}


void MainWindow::initAvatars()
{
    m_avatars =
        {
            "head", "pattern", "timing", "temperature", "setting", "poweroff", "start", "stop", "d", "exception", "query", "flash", "vol", "temperature", "level", "loop",
            "registered", "debug", "autotask", "init", "power", "check", "report", "hardware", "ok", "temp", "heart", "temper", "d", "hardware", "check", "down",
            "error", "modify", "upgrade", "query2", "unknown", "check", "socket", "power", "query2", "power", "unknown", "unknown", "unknown", "unknown", "unknown", "unknown",
            "query2", "fan", "fan", "temper"
        };
}

QString MainWindow::getHexString(const QByteArray &bytes, int from, int len, bool splitLine)
{
    QString hexString = "";
    for (int i = from; i < from + len; i++)
    {
        hexString += QString("%1 ").arg(static_cast<uchar>(bytes.at(i)), 2, 16, '0');
        if (splitLine && (i - from) % 16 == 15)
        {
            hexString += "\n";
        }
    }
    return hexString;
}

void MainWindow::on_startStopBtn_clicked(bool checked)
{
    if (checked)
    {
        int port = ui->spinBoxPort->value();
        packDataService->port = port;
        QString devName = ui->comboBoxNetAdapter->currentText();
        service->startCapture(devName, port);
        ui->startStopBtn->setText("Stop");
    }
    else
    {
        service->stopCapture();
        ui->startStopBtn->setText("Start");
    }
}

void MainWindow::onPacketReceived(Packet *packet)
{
    m_packList.append(packet);
    // 添加一个 item
    tcpListModel->insertRow(tcpListModel->rowCount());
    QModelIndex index = tcpListModel->index(tcpListModel->rowCount() - 1, 0);

    qint64 sec = packet->tick / 1000000;
    qint64 usec = packet->tick % 1000000;
    QDateTime time = QDateTime::fromSecsSinceEpoch(sec);
    QString timeStr = time.toString("HH:mm:ss") + "." + QString::number(usec);

    PackModel model;
    model.length = packet->data.length();
    auto dataType = packet->data.at(6);
    QMetaEnum metaEnum = QMetaEnum::fromType<MainWindow::DataType>();
    auto dataTypeEnum = static_cast<MainWindow::DataType>(dataType);
    QString dataTypeStr = metaEnum.valueToKey(dataTypeEnum);

    model.time = timeStr;
    model.datType = QString("%1(0x%2)").arg(dataTypeStr).arg(static_cast<int>(dataType), 0, 16);
    model.isSrc = packet->dstPort == service->port;

    if (static_cast<unsigned char>(dataType) < m_avatars.size())
        model.avatar = m_avatars.at(dataType);
    else
        model.avatar = "unknown";

    qDebug() << "before: " << m_senderFile->pos();
    m_senderFile->write(packet->data);
    qDebug() << "after: " << m_senderFile->pos();
    // 校验数据包是否正常
    if (model.length < 24)
    {
        model.isValid = false;
    }
    else
    {
        quint64 len = 0;
        for (int i = 20 ; i >= 13; i--)
        {
            len = len * 256 + static_cast<unsigned char>(packet->data.at(i));
        }
        QString str = "";
        if (len != packet->data.length() - 24)
        {
            for (int i = 13; i <= 20; i++)
            {
                str += packet->data.at(i);
            }
            model.isValid = false;
        }
    }

    tcpListModel->setData(index, QVariant::fromValue(model));

    // 刷新代理模型
    // m_filterProxy->invalidate();  // 触发代理模型的重新计算
    // QString str = QString("%1  %2->%3 %4").arg(timeStr).arg(packet->srcPort).arg(packet->dstPort).arg(packet->data.length());
    // tcpListModel->setData(index, str);
}

void MainWindow::do_currentRow_changed(const QModelIndex& current, const QModelIndex& previous)
{
    Q_UNUSED(previous);
    if (!current.isValid()) return;
    if (current.row() < m_packList.size())
    {
        auto packet = m_packList[current.row()];
        auto data = packet->data;
        if (data.length() < 25) {
            ui->labelData->clear();
            return;
        }
        int titleWidth = 20;
        DataType dataType = static_cast<DataType>(data.at(6));
        if (dataType == PowerSequence)
        {
            ui->analysisDataBtn->setEnabled(true);
        }
        else
        {
            ui->analysisDataBtn->setEnabled(false);
        }
        QMetaEnum metaEnum = QMetaEnum::fromType<DataType>();
        QString dataTypeEnumStr = metaEnum.valueToKey(dataType);
        QString dataTypeStr = QString("%1(0x%2) %3").arg(dataType).arg(dataType, 2, 16, u'0').arg(dataTypeEnumStr);
        QString param = getHexString(data, 7, 5);
        QString length = getHexString(data, 13, 8);
        QString dataStr = getHexString(data, 21, data.length() - 24);
        QString allStr = getHexString(data, 0, data.length());


        QFile file("output.txt");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "open file failed";
            return;
        }

        QTextStream out(&file);
        out << allStr;

        file.close();


        QString str = QString("%1 0x53 0x54 0x41 0x52 0x54(START)\n\n" // Start
                              "%2 0\n\n" // Version
                              "%3 %4\n\n" // DataType
                              "%5 %6\n\n" // Params
                              "%7 21(0x15)\n\n" // StartAddress
                              "%8 %9\n\n" // Length
                              "%10\n%11\n\n" // Data
                              "%12 0x45 0x4E 0x44(END)" // End
                              )
                          .arg(QString("Start:").leftJustified(titleWidth, ' ')) // %1
                          .arg(QString("Version:").leftJustified(titleWidth, ' ')) // %2
                          .arg(QString("DataType:").leftJustified(titleWidth, ' ')) // %3
                          .arg(dataTypeStr) // %4
                          .arg(QString("Params:").leftJustified(titleWidth, ' ')) // %5
                          .arg(param) // %6
                          .arg(QString("Start Address:").leftJustified(titleWidth, ' ')) // %7
                          .arg(QString("Length:").leftJustified(titleWidth, ' ')) // %8
                          .arg(length) // %9
                          .arg(QString("Data:").leftJustified(titleWidth, ' ')) // %10
                          .arg(dataStr) // %11
                          .arg(QString("End:").leftJustified(titleWidth, ' ')); // %12
        ui->labelData->setText(str);

        // QString str = "";
        // for (int i = 0; i < data.length(); i++)
        // {
        //     uchar ch = data[i];
        //     str += QString("%1 ").arg(ch, 2, 16, QLatin1Char('0')).toUpper();

        //     if ((i + 1) % 16 == 0)
        //         str += "\n";
        // }
        // ui->labelData->setText(str);
    }
}


void MainWindow::on_clearBtn_clicked()
{
    qDeleteAll(m_packList);
    m_packList.clear();
    tcpListModel->clear();
}


void MainWindow::on_analysisDataBtn_clicked()
{
    int index = ui->tcpListView->currentIndex().row();
    if (index < 0 || index >= m_packList.size()) return;

    auto packet = m_packList[index];
    auto data = packet->data;
    if (data.at(6) == PowerSequence)
    {
        QDialog dialog(this);
        PowerView* view = new PowerView(data, &dialog);
        dialog.exec();
        qDebug() << "打开弹窗显示上电信息";
    }
}


void MainWindow::on_importAction_triggered()
{
    int port = ui->spinBoxPort->value();
    service->port = port;
    packDataService->port = port;

    QString fileName = QFileDialog::getOpenFileName(this, "打开pcapng文件", "", "*.pcapng(*.pcapng)");
    if (!fileName.isEmpty())
    {
        QByteArray byte = fileName.toUtf8();
        const char* file = byte.constData();
        // 导入 wireshark的cap文件
        service->importCapFile(file, port);
    }
}

void MainWindow::on_tcp_listview_contextmenu(const QPoint &pos)
{
    QModelIndex index = ui->tcpListView->indexAt(pos);

    if (!index.isValid()) return;

    QMenu menu(this);

    QAction *copyDataAction = menu.addAction("copy");
    // QAction *copyValueAction = menu.addAction("copy value");
    QAction* selected = menu.exec(ui->tcpListView->viewport()->mapToGlobal(pos));

    if (selected == copyDataAction)
    {

        if (index.row() < m_packList.size())
        {
            auto packet = m_packList[index.row()];
            auto data = packet->data;
            QString dataStr = getHexString(data, 0, data.length());
            QClipboard *clipboard = QGuiApplication::clipboard();
            clipboard->setText(dataStr);
        }

    }
    // else if (selected == copyValueAction)
    // {
    //     qDebug() << "copy value";
    // }
}

