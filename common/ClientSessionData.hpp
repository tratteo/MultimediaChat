#pragma once
#include "UserData.hpp"
#include "UDPSocket.hpp"


class ClientSessionData
{
    public:
    ~ClientSessionData();
    ClientSessionData(int fd, char* ip);
    void UserLogged(UserData* owner);

    inline int GetFd() const { return fd; }
    inline char* GetIp() const { return ip; }
    inline UserData* GetOwner() const { return this->owner; };
    inline bool IsLogged() const { return logged; };
    inline UDPSocket* GetUdp() const {return udpSocket;}
    int udpPort = 0;
    private:
    int fd;
    UDPSocket *udpSocket = nullptr;
    char* ip;
    bool logged;
    UserData* owner = nullptr;
};