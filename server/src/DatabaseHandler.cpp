#include "../include/DatabaseHandler.hpp"

DataBaseHandler::DataBaseHandler()
{
	usersSer = new Serializer("../users.data");
}

DataBaseHandler::~DataBaseHandler()
{
	delete usersSer;
}

void DataBaseHandler::AddUser(UserData* user)
{
    std::lock_guard<std::recursive_mutex> lock(mutex);

	if (!IsUserRegistered(user))
	{
        if (usersSer->Append(user->ToString()))
        {
            std::cout << "Registering: " << user->ToString() << std::endl;
        }
	}
}

bool DataBaseHandler::IsUserRegistered(UserData* user)
{
    std::lock_guard<std::recursive_mutex> lock(mutex);
	UserData current;
    auto lines = usersSer->GetLines();
    if (lines.size() > 0)
    {
        for (auto const& i : lines)
        {
            current.FromString(i);
            if (current.GetUsername() == user->GetUsername())
            {
                return true;
            }
        }
    }
    return false;
}