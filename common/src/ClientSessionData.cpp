#include "../ClientSessionData.hpp"

ClientSessionData::~ClientSessionData()
{
    close(fd);
    delete inUdpSocket;
    delete outUdpSocket;
}

UDPSocket* ClientSessionData::GetUDP(UDPSocket::Type type) const
{
    if (type == UDPSocket::IN)
    {
        return inUdpSocket;
    }
    else if (type == UDPSocket::OUT)
    {
        return outUdpSocket;
    }
    else
    {
        return nullptr;
    }
}

ClientSessionData::ClientSessionData(int fd, char* ip)
{
    this->fd = fd;
    this->ip = ip;
    this->owner = nullptr;
    this->inUdpSocket = new UDPSocket(ip, 5051, UDPSocket::IN);
    this->outUdpSocket = new UDPSocket(ip, 5051, UDPSocket::OUT);
}

void ClientSessionData::UserLogged(UserData* owner)
{
    if (this->owner != nullptr)
    {
        delete owner;

    }
    this->owner = owner;
    logged = true;
}   
