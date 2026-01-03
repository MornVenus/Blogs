#include "tcpcaptureservice.h"
#include <QList>
#include <QMessageBox>

QList<const u_char*> list;

static Packet* parseTcpPacket(const u_char* pkt_data, const struct pcap_pkthdr* header)
{
    Packet* pkt = new Packet();

    qint64 t = (qint64)header->ts.tv_sec * 1000000LL +
        (qint64)header->ts.tv_usec;

    pkt->tick = t;

    int offset = 0;

    const ethernet_header* eth = reinterpret_cast<const ethernet_header*>(pkt_data);

    // 以太网帧头长度 = 14 字节
    if (ntohs(eth->type) == 0x0800)
    {
        // ipv4
        offset = 14;
    }
    else
    {
        if (pkt_data[0] == 2)
        {
            offset = 4;
        }
        else
        {
            // 非 IPv4
            return pkt;
        }
    }

    const ip_header* ih = reinterpret_cast<const ip_header*>(pkt_data + offset);
    int ipHeaderLen = (ih->ver_ihl & 0x0F) * 4;

    // 只处理 TCP (proto == 6)
    if (ih->proto != 6)
        return pkt;

    const tcp_header* th = reinterpret_cast<const tcp_header*>(pkt_data + offset + ipHeaderLen);
    int tcpHeaderLen = ((th->hdrlen_resv & 0xF0) >> 4) * 4;

    // 数据起始位置
    int totalLen = ntohs(ih->tlen);
    int dataOffset = offset + ipHeaderLen + tcpHeaderLen;
    int dataLen = totalLen - ipHeaderLen - tcpHeaderLen;

    if (dataLen < 0)
        dataLen = 0;

    // ---- 填充 Packet ----
    char srcIp[INET_ADDRSTRLEN];
    char dstIp[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, ih->saddr, srcIp, sizeof(srcIp));
    inet_ntop(AF_INET, ih->daddr, dstIp, sizeof(dstIp));

    pkt->srcIp = QString::fromUtf8(srcIp);
    pkt->dstIp = QString::fromUtf8(dstIp);
    pkt->srcPort = ntohs(th->sport);
    pkt->dstPort = ntohs(th->dport);
    pkt->ack = th->acknum;
    pkt->seq = th->seqnum;
    pkt->len = dataLen;

    // 提取 TCP 载荷（data）
    if (dataLen > 0) {
        pkt->data = QByteArray(reinterpret_cast<const char*>(pkt_data + dataOffset), dataLen);
    }

    return pkt;
}

static void packet_handler(u_char* param, const struct pcap_pkthdr* header, const u_char* pkt_data)
{
    Packet* pkt = parseTcpPacket(pkt_data, header);

    TcpCaptureService* service = reinterpret_cast<TcpCaptureService*>(param);
    if (pkt->data.isEmpty())
    {
        delete pkt;
        return;
    }
    emit service->packetReceived(pkt);
    // TcpPacket* packet = new TcpPacket();

    // QByteArray data(reinterpret_cast<const char*>(pkt_data), header->len);
    // packet->setData(data);

    // emit service->packetReceived(packet);

    // struct tm* ltime;
    // char timestr[16];
    // time_t local_tv_sec;

    // uint len = header->len;
    // QString str = QString::asprintf("%ld:%ld (%d)", header->ts.tv_sec, header->ts.tv_usec, len);
    // qDebug() << str;
    // list.append(pkt_data);
    // for (auto& item : list)
    // {
    //     qDebug() << item;
    // }
}

TcpCaptureService::TcpCaptureService(QObject *parent)
    : QThread{parent}
{}

TcpCaptureService::~TcpCaptureService()
{
    if (m_alldevs)
        pcap_freealldevs(m_alldevs);
    if (m_adapter)
        pcap_close(m_adapter);
     qDebug() << "~TcpCaptureService";
}

QStringList TcpCaptureService::getDeviceNames()
{
    QStringList devNames;

    if (m_alldevs)
        pcap_freealldevs(m_alldevs);
    if (pcap_findalldevs(&m_alldevs, m_errbuf) == -1) {
        qCritical() << "Error in pcap_findalldevs:" << m_errbuf;
        return devNames;
    }

    pcap_if_t *dev = m_alldevs;
    while (dev)
    {
        QString devName = getDeviceFriendName(dev->name);
        m_nameMap.insert(devName, dev->name);
        devNames.append(devName);
        dev = dev->next;
    }

    return devNames;
}

void TcpCaptureService::startCapture(const QString& devName, int port)
{
    if (!m_nameMap.contains(devName)) return;
    qDebug() << m_nameMap[devName];
    // open the adapter
    m_adapter = pcap_open_live(m_nameMap[devName], 65536, 1, 1000, m_errbuf);
    if (m_adapter == nullptr)
    {
        return;
    }

    // set filter
    struct bpf_program fcode;
    int res = 0;
    QString filterStr = QString("tcp port %1").arg(port);
    QByteArray array = filterStr.toUtf8();
    const char* filter = array.constData();

    if ((res = pcap_compile(m_adapter, &fcode, filter, 1, PCAP_NETMASK_UNKNOWN)) < 0)
    {
        qCritical() << "fail to compile:" << pcap_statustostr(res);
        pcap_close(m_adapter);
        return;
    }
    if ((res = pcap_setfilter(m_adapter, &fcode) < 0))
    {
        qCritical() << "fail to set filter:" << pcap_statustostr(res);
        pcap_close(m_adapter);
        m_adapter = nullptr;
        return;
    }
    this->port = port;
    start();
}

void TcpCaptureService::stopCapture()
{
    if (m_adapter)
    {
        pcap_breakloop(m_adapter);
    }
}

/**
 * 导入wireshark的抓包文件
 * @brief TcpCaptureService::importCapFile
 * @param filePath
 * @param port
 */
void TcpCaptureService::importCapFile(const char* filePath, int port)
{
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* handle = pcap_open_offline(filePath, errbuf);

    if (!handle) {
        qDebug() << "pcap_open_offline failed:" << errbuf;
        return;
    }

    struct bpf_program filter;

    QString filterStr = QString("tcp port %1").arg(port);
    QByteArray array = filterStr.toUtf8();
    const char* filter_exp = array.constData();

    if (pcap_compile(handle, &filter, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == -1) {
        qDebug() << "pcap_compile failed";
        return;
    }

    if (pcap_setfilter(handle, &filter) == -1) {
        qDebug() << "pcap_setfilter failed";
        return;
    }

    struct pcap_pkthdr* header;
    const u_char* data;

    Packet* lastSrcPack = nullptr, *lastDstPack = nullptr;
    while (pcap_next_ex(handle, &header, &data) > 0) {
        // data 指向完整的以太网帧
        Packet* pkt = parseTcpPacket(data, header);
        if (pkt->data.isEmpty())
        {
            delete pkt;
            continue;
        }
        if (pkt->srcPort == port)
        {
            if (pkt->equals(lastSrcPack))
            {
                delete pkt;
                continue;
            }

            lastSrcPack = pkt;
        }
        else
        {
            if (pkt->equals(lastDstPack))
            {
                delete pkt;
                continue;
            }
            lastDstPack = pkt;
        }

        emit packetReceived(pkt);
    }
    QMessageBox::information(nullptr, "info", "Done");
    qDebug() << "done";
    pcap_close(handle);
}

void TcpCaptureService::run()
{
    // start capture loop
    pcap_loop(m_adapter, 0, packet_handler, reinterpret_cast<u_char*>(this));
    pcap_close(m_adapter);
    m_adapter = nullptr;
}

QString TcpCaptureService::getDeviceFriendName(const char *devName)
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


