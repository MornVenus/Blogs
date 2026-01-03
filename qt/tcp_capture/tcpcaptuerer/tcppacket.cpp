#include "tcppacket.h"

TcpPacket::TcpPacket(QObject *parent)
    : QObject{parent}
{}

void TcpPacket::setData(QByteArray data)
{
    m_data = data;
}

QByteArray TcpPacket::getData()
{
    return m_data;
}

int TcpPacket::length()
{
    return m_data.length();
}
