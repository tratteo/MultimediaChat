#include "../include/ClientSessionData.hpp"

ClientSessionData::~ClientSessionData()
{
}

ClientSessionData::ClientSessionData(int fd, char* ip)
{
    this->fd = fd;
    this->ip = ip;
    this->logged = false;
    this->owner = nullptr;
}

void ClientSessionData::UserLogged(UserData* owner)
{
    if (this->owner != nullptr)
    {
        delete owner;

    }
    this->owner = owner;
    logged = true;
}   
