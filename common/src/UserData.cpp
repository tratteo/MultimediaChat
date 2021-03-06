#include "../UserData.hpp"

UserData::UserData()
{

}

UserData::~UserData()
{
    for(auto &chat : chats)
    {
        if(chat != nullptr)
        {
            delete chat;
            chat = nullptr;
        }
    }
}

UserData::UserData(std::string username, std::string password)
{
    this->username = username;
    this->password = password;
}

void UserData::AddChat(ChatData* chat)
{
    chats.push_back(chat);
}

ChatData* UserData::GetChatWith(std::string username)
{
    std::list<ChatData*>::iterator it = std::find_if(chats.begin(), chats.end(), [&](ChatData* data) 
    { 
        std::tuple<std::string, std::string> users = data->GetUsers();
        return std::get<0>(users) == username || std::get<1>(users) == username;
    });

    if (it != chats.end())
    {
        return *it;
    }
    else
    {
        return nullptr;
    }
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

