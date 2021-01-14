#pragma once
#include <string>
#include <tuple>
#include <list>
#include "Payloads.hpp"

class ChatData
{
    public:
    ChatData();
    ChatData(std::string firstUser, std::string secondUser);
    void AddMessage(MessagePayload message);
    std::string ToString(std::string owner) const;
    inline std::tuple<std::string, std::string> GetUsers() const { return std::make_tuple(firstUser, secondUser); };
    private:
    std::string secondUser;
    std::string firstUser;
    std::list<MessagePayload> messages;

};