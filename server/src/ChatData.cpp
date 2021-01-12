#include "../include/ChatData.hpp"

ChatData::ChatData(std::string firstUser, std::string secondUser)
{
	this->firstUser = firstUser;
	this->secondUser = secondUser;
}
ChatData::ChatData()
{
}

void ChatData::AddMessage(MessagePayload message)
{
	messages.push_back(message);
}

std::string ChatData::ToString(std::string owner)
{
    std::string subject = owner == firstUser ? secondUser : firstUser;
    std::string res = "Chat:" + subject + "\n";
    for (auto& msg : messages)
    {
        res.append(msg.ToString());
    }
    return res;
}