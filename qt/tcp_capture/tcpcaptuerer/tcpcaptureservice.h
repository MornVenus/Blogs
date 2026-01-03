#ifndef TCPCAPTURESERVICE_H
#define TCPCAPTURESERVICE_H

#include <QObject>
#include <QCoreApplication>
#include <winsock2.h>
#include <QDebug>
#include <windows.h>
#include <QDir>
#include <pcap.h>
#include <QRegularExpression>
#include <objbase.h>
#include <netioapi.h>
#include <QMap>
#include <QThread>
#include "tcppacket.h"

class TcpCaptureService : public QThread
{
    Q_OBJECT
public:
    explicit TcpCaptureService(QObject *parent = nullptr);
    ~TcpCaptureService();
    QStringList getDeviceNames();
    void startCapture(const QString& devName, int port);
    void stopCapture();
    void importCapFile(const char* filePath, int port);
    int port;

private:
    char m_errbuf[PCAP_ERRBUF_SIZE];

    pcap_if_t * m_alldevs = nullptr;
    pcap_t* m_adapter = nullptr;
    QMap<QString, char*> m_nameMap;

private:
    QString getDeviceFriendName(const char *devName);
protected:
    void run();
signals:
    void packetReceived(TcpPacket* packet);
    void packetReceived(Packet* packet);
};

#endif // TCPCAPTURESERVICE_H
