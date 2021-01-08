#pragma once
class ClientSessionData
{
    public:
    ClientSessionData(int fd, char* ip);
    int GetFd();
    char* GetIp();

    bool logged;

    private:
    int fd;
    char* ip;
};