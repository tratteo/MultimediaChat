#pragma once
#include <string>

class UserData
{
    private: 
    std::string username;
    std::string password;

    public:
    UserData(std::string username, std::string password);
    std::string ToString();
    std::string GetUsername();
    std::string GetPassword();
};