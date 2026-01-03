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
    Q_UNUSED(param)
    Q_UNUSED(pkt_data)

    uint len = header->len;
    if (len < 55) return; // tcp packet must be larger than 54

    int offset = 0;
    // 以太网
    if (pkt_data[12] == 8 && pkt_data[13] == 0)
    {
        // ipv4数据包
        offset = 14;
    }
    else {
        if (pkt_data[0] == 2 && pkt_data[1] == 0 && pkt_data[2] == 0 && pkt_data[3] == 0)
        {
            // loop back
            offset = 4;
        }
        else
            return;
    }

    // ip
    int ip_pack_total_len = pkt_data[offset+2] * 256 + pkt_data[offset+3];
    u_char ip_len = (pkt_data[offset] & 15) * 4;
    offset += ip_len;

    // tcp
    u_char tcp_len = (((pkt_data[offset + 12]) >> 4) & 15) * 4;
    offset += tcp_len;

    int payload_len = ip_pack_total_len - ip_len - tcp_len;

    qDebug() << QString("data len: %1").arg(payload_len);

    // struct tm* ltime;
    // char timestr[16];
    // time_t local_tv_sec;


    // QString str = QString::asprintf("%ld:%ld (%ld)", header->ts.tv_sec, header->ts.tv_usec, len);
    // qDebug() << str;
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
        if (devName == "以太网")
        {
            lastName = dev->name;
        }

        qDebug() << devName;
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
    if ((res = pcap_compile(adapter, &fcode, "tcp port 10000", 1, PCAP_NETMASK_UNKNOWN)) < 0)
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
