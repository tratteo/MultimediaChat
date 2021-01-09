#include "../include/DatabaseHandler.hpp"

DataBaseHandler::DataBaseHandler()
{
	usersSer = new Serializer("../users.data");
    UserData current;
    auto lines = usersSer->GetLines();
    if (lines.size() > 0)
    {
        for (auto const& i : lines)
        {
            current.FromString(i);
            registeredUsers.push_front(current);
        }
    }
    std::cout << "Registered users: " << std::endl;
    for (auto & i : registeredUsers) 
    {
        std::cout << i.ToString();
    }
}

DataBaseHandler::~DataBaseHandler()
{
	delete usersSer;
}

void DataBaseHandler::UserConnected(ClientSessionData* data)
{
    mutex.lock();
    connectedUsers.push_front(data);
    mutex.unlock();
}

void DataBaseHandler::UserDisconnected(ClientSessionData* data)
{
    mutex.lock();
    connectedUsers.remove(data);
    mutex.unlock();
}

UserData* DataBaseHandler::GetRegisteredUser(std::string username)
{
    std::list<UserData>::iterator it = std::find_if(registeredUsers.begin(), registeredUsers.end(), [&](UserData& data) { return username == data.GetUsername(); });
    if (it != registeredUsers.end())
    {
        return &(*it);
    }
    else
    {
        return nullptr;
    }
}

ClientSessionData* DataBaseHandler::GetUserSession(std::string username)
{
    std::list<ClientSessionData*>::iterator it = std::find_if(connectedUsers.begin(), connectedUsers.end(), [&](ClientSessionData* data) {return username == data->GetOwner()->GetUsername(); });
    if (it != connectedUsers.end())
    {
        return *it;
    }
    else
    {
        return nullptr;
    }
}

void DataBaseHandler::RegisterUser(UserData* user)
{   
    mutex.lock();
	if (!IsUserRegistered(user))
	{
        registeredUsers.push_front(*user);
        if (usersSer->Append(user->ToString()))
        {
        }
	}
    mutex.unlock();
}

bool DataBaseHandler::IsUserRegistered(UserData* user)
{
    mutex.lock();
    std::list<UserData>::iterator it = std::find_if(registeredUsers.begin(), registeredUsers.end(), [&](UserData& data) { return user->GetUsername() == data.GetUsername(); });
    return it != registeredUsers.end();
    mutex.unlock();
}