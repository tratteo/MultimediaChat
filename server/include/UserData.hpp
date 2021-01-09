#pragma once
#include <string>

class UserData
{
    private: 
    std::string username;
    std::string password;

    public:
    UserData();
    UserData(std::string username, std::string password);
    void FromCredentials(std::string username, std::string password);
    void FromString(std::string);
    std::string ToString() const;
    std::string GetUsername() const;
    std::string GetPassword() const;
};