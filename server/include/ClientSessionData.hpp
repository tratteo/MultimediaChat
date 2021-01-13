#pragma once
#include "UserData.hpp"

class ClientSessionData
{
    public:
    ~ClientSessionData();
    ClientSessionData(int fd, char* ip);
    void UserLogged(UserData* owner);
    inline int GetFd() const { return this->fd; };
    inline char* GetIp() const { return this->ip; };
    inline UserData* GetOwner() const { return this->owner; };
    inline bool IsLogged() const { return logged; };

    private:
    bool logged;
    UserData* owner;
    int fd;
    char* ip;
};