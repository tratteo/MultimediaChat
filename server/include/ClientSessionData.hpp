#pragma once
class ClientSessionData
{
    public:
    ClientSessionData(int fd, char* ip);
    int GetFd();
    char* GetIp();

    private:
    int fd;
    char* ip;
};