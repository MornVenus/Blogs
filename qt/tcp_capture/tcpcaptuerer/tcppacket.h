#ifndef TCPPACKET_H
#define TCPPACKET_H

#include <QObject>

class TcpPacket : public QObject
{
    Q_OBJECT
public:
    explicit TcpPacket(QObject *parent = nullptr);

    void setData(QByteArray data);
    QByteArray getData();
    int length();

private:
    QByteArray m_data;

signals:
};

class Packet
{
public:
    qint64 tick;
    QString srcIp;
    int srcPort;
    QString dstIp;
    int dstPort;
    QByteArray data;

    uint ack = 0;
    uint seq = 0;
    uint len = 0;
    bool equals(const Packet* other)
    {
        if (other == nullptr) return false;
        return len == other->len
               && srcIp == other->srcIp && srcPort == other->srcPort
               && dstIp == other->dstIp && dstPort == other->dstPort
               && ack == other->ack && seq == other->seq;
    }
};



// Ethernet (以太网帧头)
struct ethernet_header {
    uchar dest[6];
    uchar src[6];
    ushort type;
};

// IPv4 头
struct ip_header {
    uchar  ver_ihl;        // 版本 + 头长度
    uchar  tos;            // 服务类型
    ushort tlen;           // 总长度
    ushort identification; // 标识
    ushort flags_fo;       // 标志 + 片偏移
    uchar  ttl;            // 生存时间
    uchar  proto;          // 协议类型 (6=TCP, 17=UDP, etc.)
    ushort crc;            // 头校验和
    uchar  saddr[4];       // 源地址
    uchar  daddr[4];       // 目的地址
};

// TCP 头
struct tcp_header {
    ushort sport;          // 源端口
    ushort dport;          // 目的端口
    uint   seqnum;         // 序号
    uint   acknum;         // 确认号
    uchar  hdrlen_resv;    // 头部长度 (高4位)
    uchar  flags;
    ushort window;
    ushort checksum;
    ushort urgentptr;
};

#endif // TCPPACKET_H
