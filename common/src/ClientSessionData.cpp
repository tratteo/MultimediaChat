#include "../ClientSessionData.hpp"

ClientSessionData::~ClientSessionData()
{
    close(fd);
    delete udpSocket;
}


ClientSessionData::ClientSessionData(int fd, char* ip)
{
    this->fd = fd;
    this->ip = ip;
    this->owner = nullptr;
    UDPSocket *sock = new UDPSocket(ip, 0);
    int port = sock->GetPort();
    delete sock;
    udpSocket = new UDPSocket(ip, port);
}

void ClientSessionData::UserLogged(UserData* owner)
{
    if (this->owner != nullptr)
    {
        delete this->owner;

    }
    this->owner = owner;
    logged = true;
}   
