#pragma once
#include "UserData.hpp"

class ClientSessionData
{
    public:
    ~ClientSessionData();
    ClientSessionData(int fd, char* ip);
    int GetFd() const;
    char* GetIp() const;
    UserData* GetOwner() const;
    bool IsLogged() const;
    void UserLogged(UserData* owner);

    private:
    bool logged;
    UserData* owner;
    int fd;
    char* ip;
};