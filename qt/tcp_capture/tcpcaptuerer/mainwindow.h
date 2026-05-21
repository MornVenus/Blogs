#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "tcpcaptureservice.h"
#include <QAbstractItemModel>
#include <QStringListModel>
#include <QList>
#include <QItemSelectionModel>
#include <QQueue>
#include "packdataservice.h"
#include "packlistmodel.h"
#include "packdelegate.h"
#include <QMetaEnum>
#include "views/powerview.h"
#include <QDialog>
#include "myfiltermodel.h"
#include <QMenu>
#include <QClipboard>
#include <QFile>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum DataType
    {
        HeaderStream = 0,
        Pattern = 1,
        TimingSet = 2,
        AmbientTemperatureSetting = 3,
        GeneralSetting = 4,
        PowerSequence = 5,
        StartSignal = 6,
        StopSignal = 7,
        DftResult = 8,
        ExceptionInfo = 9,
        Query = 10,
        BIB = 11,
        SocketV = 12,
        SocketTemp = 13,
        TimingLevel = 14,
        VectorStepLoop = 15,
        Register = 16,
        Debug = 17,
        AutoDiagItem = 18,
        Initialize = 19,
        RunPowerSeq = 20,
        BibSlotCheck = 21,
        FpgaReportStatus = 0x16,
        ReadHardwareInfo = 0x17,
        ReceiveStatusAndResend = 0x18,
        OvenTemperature = 0x19,
        HeartAlive = 0x1A,
        SelfWarmTemp = 0x1B,
        SendDFTresult = 0x1C,
        ReplyHardwareInfo = 0x1D,
        CheckDriverBoard = 0x1E,
        DriverBoardAllDown = 0x1F,
        ReportTimingError = 0x20,
        ModifyFPGAIPPORT = 0x21,
        UpgradeFPGAOnline = 0x22,
        QueryDftState = 0x23,
        Unknown1 = 0x24,
        DutStateCheck = 0x25,
        SocketDuty = 0x26,
        SingleSitePower = 0x27,
        QueryRegualChannel = 0x28,
        PowerBoardCheck = 0x29,

        Unknown2 = 0x2A,
        Unknown3 = 0x2B,
        Unknown4 = 0x2C,
        Unknown5 = 0x2D,
        Unknown6 = 0x2E,
        Unknown7 = 0x2F,

        PowerQuery = 0x30,
        SetSocketFan = 0x31,
        SocketFanResp = 0x32,
        AmbTempResp = 0x33
    };
    Q_ENUM(DataType)

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_startStopBtn_clicked(bool checked);
    void onPacketReceived(Packet* packet);
    void do_currentRow_changed(const QModelIndex& current, const QModelIndex& previous);

    void on_clearBtn_clicked();

    void on_analysisDataBtn_clicked();

    void on_importAction_triggered();

    void on_tcp_listview_contextmenu(const QPoint& pos);


private:
    Ui::MainWindow *ui;
    TcpCaptureService* service;
    // QStringListModel* tcpListModel;
    PackListModel* tcpListModel;
    QList<TcpPacket*> m_packetList;
    QList<Packet*> m_packList;
    QItemSelectionModel* selModel;
    QQueue<char> m_srcQue;
    QQueue<char> m_dstQue;
    PackDataService* packDataService;
    PackDelegate* delegate;
    QStringList m_avatars;
    MyFilterModel* m_filterProxy = nullptr;

    QFile* m_senderFile = nullptr;
    QFile* m_recvFile = nullptr;

private:
    void init();
    void initAvatars();
    QString getHexString(const QByteArray& bytes, int from, int len, bool splitLine = true);
};
#endif // MAINWINDOW_H
