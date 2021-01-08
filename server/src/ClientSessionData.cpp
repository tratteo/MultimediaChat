#include "../include/ClientSessionData.hpp"

ClientSessionData::ClientSessionData(int fd, char* ip)
{
    this->fd = fd;
    this->ip = ip;
    this->logged = false;
}

int ClientSessionData::GetFd()
{
    return this->fd;
}

char* ClientSessionData::GetIp()
{
    return this->ip;
}