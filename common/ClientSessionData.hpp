#pragma once
#include "UserData.hpp"
#include "UDPSocket.hpp"


class ClientSessionData
{
    public:
    ~ClientSessionData();
    ClientSessionData(int fd, char* ip);
    void UserLogged(UserData* owner);

    UDPSocket* GetUDP(UDPSocket::Type type) const;
    inline int GetFd() const { return fd; }
    inline char* GetIp() const { return ip; }
    inline UserData* GetOwner() const { return this->owner; };
    inline bool IsLogged() const { return logged; };

    private:
    int fd;
    char* ip;
    UDPSocket* inUdpSocket;
    UDPSocket* outUdpSocket;
    bool logged;
    UserData* owner;
};