#pragma once
#include "UserData.hpp"

class ClientSessionData
{
    public:
    ~ClientSessionData();
    ClientSessionData(int fd, char* ip);
    int GetFd() const;
    void RegisterOwner(UserData* owner);
    char* GetIp() const;
    UserData* GetOwner() const;
    bool logged;

    private:
    UserData* owner;
    int fd;
    char* ip;
};