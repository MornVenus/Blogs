#ifndef PACKDATASERVICE_H
#define PACKDATASERVICE_H

#include <QObject>
#include "tcppacket.h"
#include <QQueue>

class PackExtracter
{
    enum Step
    {
        START_S = 0,
        START_T,
        START_A,
        START_R,
        START_T2,
        END_E,
        END_N,
        END_D
    };
public:
    QList<Packet*> append(Packet* pack)
    {
        QList<Packet*> result;
        for(char& ch: pack->data)
        {
            switch(s)
            {
            case START_S:
                if (ch == 'S')
                {
                    if (tempPacket && tempPacket != pack)
                        delete tempPacket;
                    tempPacket = pack;
                    s = (Step)(s + 1);
                    queue.push_back(ch);
                }

                break;
            case START_T:
                if (ch == 'T')
                {
                    s = (Step)(s + 1);
                    queue.push_back(ch);
                }
                else {
                    queue.clear();
                }
                break;
            case START_A:
                if (ch == 'A')
                {
                    s = (Step)(s + 1);
                    queue.push_back(ch);
                }
                else {
                    queue.clear();
                }
                break;
            case START_R:
                if (ch == 'R')
                {
                    s = (Step)(s + 1);
                    queue.push_back(ch);
                }
                else {
                    queue.clear();
                }
                break;
            case START_T2:
                if (ch == 'T')
                {
                    s = (Step)(s + 1);
                    queue.push_back(ch);
                }
                else {
                    queue.clear();
                }
                break;
            case END_E:
                if (ch == 'E')
                {
                    s = (Step)(s + 1);
                }
                queue.push_back(ch);
                break;
            case END_N:
                if (ch == 'N')
                {
                    s = (Step)(s + 1);
                }
                queue.push_back(ch);
                break;
            case END_D:
                queue.push_back(ch);
                if (ch == 'D')
                {
                    // 将数据抛出去并清空 queue
                    QByteArray array;
                    array.resize(queue.size());

                    for (int i = 0; i < array.size(); ++i)
                        array[i] = queue.dequeue(); // 出队（会修改queue）
                    s = START_S;
                    Packet* packet = new Packet();
                    packet->tick = tempPacket->tick;
                    packet->srcIp = tempPacket->srcIp;
                    packet->dstIp = tempPacket->dstIp;
                    packet->srcPort = tempPacket->srcPort;
                    packet->dstPort = tempPacket->dstPort;
                    packet->data = array;
                    result.append(packet);
                }
                break;
            default:
                break;
            }
        }
        if (tempPacket != pack)
            delete pack;
        return result;
    }
private:
    Packet* tempPacket = nullptr;
    QQueue<char> queue;
    Step s = START_S;
};

class PackDataService : public QObject
{
    Q_OBJECT
public:
    explicit PackDataService(QObject *parent = nullptr);
    int port;
public slots:
    void append(Packet* pack);
signals:
    void packetReceived(Packet* pack);
private:
    PackExtracter m_src;
    PackExtracter m_dst;

};

#endif // PACKDATASERVICE_H
