#include "../ClientSessionData.hpp"

ClientSessionData::~ClientSessionData()
{
    close(fd);
    if(udpSocket != nullptr)
    {
        delete udpSocket;
        udpSocket = nullptr;
    }
}


ClientSessionData::ClientSessionData(int fd, char* ip)
{
    this->fd = fd;
    this->ip = ip;
    this->owner = nullptr;
/*     UDPSocket *sock = new UDPSocket(0);
    int port = sock->GetPort();
    delete sock;  */
    udpSocket = new UDPSocket(5001);
    	
/* 	int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK); */
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
