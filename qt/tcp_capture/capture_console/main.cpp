#include <QCoreApplication>
#include <winsock2.h>
#include <QDebug>
#include <windows.h>
#include <QDir>
#include <pcap.h>
#include <QRegularExpression>
#include <objbase.h>
#include <netioapi.h>

bool checkNpcapExist()
{
    WCHAR path[512];
    uint len = GetSystemDirectory(path, 480);
    QString systemPath = QString::fromWCharArray(path, len);

    QString npcapFolder = QDir(systemPath).filePath("Npcap");
    return QDir(npcapFolder).exists();
}

QString getDeviceFriendName(const char *devName)
{
    QString friendName(devName);

    // extract guid string by regex
    QRegularExpression regex(R"(({[A-F0-9\-]+}))");
    auto match = regex.match(friendName);

    if (!match.hasMatch()) return friendName;

    // translate guid string to GUID
    GUID guid;

    HRESULT hr = CLSIDFromString(match.captured(1).toStdWString().c_str(), &guid);

    if (!SUCCEEDED(hr)) return friendName;

    // GUID => LUID => Alias(FriendName)
    NET_LUID luid;
    if (0 == ConvertInterfaceGuidToLuid(&guid, &luid))
    {
        WCHAR buffer[256] = {0};
        if (0 == ConvertInterfaceLuidToAlias(&luid, buffer, 256))
        {
            friendName = QString::fromWCharArray(buffer);
        }
    }

    return friendName;
}

void packet_handler(u_char* param, const struct pcap_pkthdr* header, const u_char* pkt_data)
{
    Q_UNUSED(param);
    Q_UNUSED(pkt_data);

    // struct tm* ltime;
    // char timestr[16];
    // time_t local_tv_sec;

    uint len = header->len;
    QString str = QString::asprintf("%ld:%ld (%ld)", header->ts.tv_sec, header->ts.tv_usec, len);
    qDebug() << str;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // 判断npcap是否存在
    if (!checkNpcapExist())
    {
        qDebug() << "Npcap not exists";
        return -1;
    }

    // 检索网卡列表
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_if_t *alldevs;
    // pcap_t *handle;

    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        qCritical() << "Error in pcap_findalldevs:" << errbuf;
        return -1;
    }

    QStringList devNames;
    pcap_if_t *dev = alldevs;
    char* lastName = nullptr;
    while (dev)
    {
        QString devName = getDeviceFriendName(dev->name);
        devNames.append(devName);
        lastName = dev->name;
        dev = dev->next;
    }

    // open the adapter
    pcap_t* adapter = pcap_open_live(lastName, 65536, 1, 1000, errbuf);
    if (adapter == nullptr)
    {
        pcap_freealldevs(alldevs);
        qDebug() << "fail to open the net adapter: " << lastName;
        return -1;
    }

    pcap_freealldevs(alldevs);

    // set filter
    struct bpf_program fcode;
    int res = 0;
    if ((res = pcap_compile(adapter, &fcode, "tcp port 3000", 1, PCAP_NETMASK_UNKNOWN)) < 0)
    {
        qDebug() << "fail to compile:" << pcap_statustostr(res);
        pcap_close(adapter);
        return -1;
    }
    if ((res = pcap_setfilter(adapter, &fcode) < 0))
    {
        qDebug() << "fail to set filter:" << pcap_statustostr(res);
        pcap_close(adapter);
        return -1;
    }

    // start capture loop
    pcap_loop(adapter, 0, packet_handler, nullptr);

    pcap_close(adapter);

    qDebug() << "hello";

    return a.exec();
}
