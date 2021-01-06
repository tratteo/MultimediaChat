#pragma once
class ClientSessionData
{
    public:
    ClientSessionData(int fd, char* ip);
    int GetFd();
    char* GetIp();
    bool IsLogged();

    private:
    int fd;
    bool logged;
    char* ip;
};