#include "../include/UserData.hpp"

UserData::UserData()
{

}

UserData::UserData(std::string username, std::string password)
{
    this->username = username;
    this->password = password;
}

void UserData::AddChat(Chat chat)
{
    chats.push_back(chat);
}

std::string UserData::GetUsername() const
{
    return this->username;
}

std::string UserData::GetPassword() const
{
    return this->password;
}

std::string UserData::ToString() const
{
    return username + "-" + password + "\n";
}


void UserData::FromString(std::string format)
{
    int index = format.find("-");
    if (index == -1) return;
    username = format.substr(0, index);
    password = format.substr(index + 1, format.length());
}

void UserData::FromCredentials(std::string username, std::string password)
{
    this->username = username;
    this->password = password;
}

