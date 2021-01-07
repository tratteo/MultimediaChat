#include "../include/UserData.hpp"

UserData::UserData(std::string username, std::string password)
{
    this->username = username;
    this->password = password;
}


std::string UserData::GetUsername()
{
    return this->username;
}

std::string UserData::GetPassword()
{
    return this->password;
}

std::string UserData::ToString()
{
    return "User: " + username + " - Password: " + password + "\n";
}

