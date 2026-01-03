#include "packdataservice.h"

PackDataService::PackDataService(QObject *parent)
    : QObject{parent}
{}

void PackDataService::append(Packet *pack)
{
    if (!pack) return;
    if (pack->dstPort == port)
    {
        QList<Packet*> list = m_dst.append(pack);
        for (int i = 0; i < list.size(); i++)
        {
            emit packetReceived(list.at(i));
        }
    }
    else if (pack->srcPort == port)
    {
        QList<Packet*> list = m_src.append(pack);
        for (int i = 0; i < list.size(); i++)
        {
            emit packetReceived(list.at(i));
        }
    }
}
