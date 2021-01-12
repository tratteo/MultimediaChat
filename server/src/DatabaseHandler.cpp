#include "../include/DatabaseHandler.hpp"

DataBaseHandler::DataBaseHandler()
{
    ParseDatabase();
}

DataBaseHandler::~DataBaseHandler()
{
    SerializeDatabase();
}

void DataBaseHandler::SerializeDatabase()
{
    for (auto usr : registeredUsers)
    {
        std::cout << "Serializing data for user: " << usr->GetUsername() << std::endl;
        std::string userDataFormat = "Credentials:" + usr->ToString();
        for (auto& chat : usr->GetChats())
        {
            userDataFormat.append(chat->ToString(usr->GetUsername()));
        }

        Overwrite(userDataFormat, DATABASE_PATH, usr->GetUsername() + ".data");
    }
}

void DataBaseHandler::ParseDatabase()
{
    std::vector<std::string> filenames = GetFilenames(DATABASE_PATH);
    std::list<std::string> lines;
    std::regex regex;
    std::smatch match;
    UserData *user;
    for (const auto& name : filenames)
    {
        ChatData* currentChat = nullptr;
        std::cout << "Parsing: " << name << std::endl;
        
        std::list<std::string> lines = GetLines(DATABASE_PATH, name);

        for (const std::string& line : lines)
        {
            // Find credentials
            if (std::regex_search(line, match, std::regex("Credentials:")))
            {
                if (line.find("-") != std::string::npos)
                {
                    user = new UserData();
                    user->FromString(line.substr(line.find(":") + 1, line.length()));
                    //std::cout << "Credentials: " << user->ToString();
                }
            }
            else if(std::regex_search(line, match, std::regex("Chat:")))
            {
                //std::cout << "Found chat" << std::endl;
                //Find chat 
                if (currentChat != nullptr)
                {
                    user->AddChat(currentChat);
                    //std::cout << "Adding last chat" << std::endl;
                }
                std::string dest = line.substr(line.find(":") + 1, line.length());
                currentChat = new ChatData(user->GetUsername(), dest);
                //std::cout << "Adding chat between: " << currentChat->firstUser << " and " << currentChat->secondUser << std::endl;
            }
            else 
            {
                int i1 = line.find(">");
                if (i1 != std::string::npos)
                {
                    int i2 = line.find(":");
                    if (i2 != std::string::npos)
                    {
                        std::string from = line.substr(0, i1);
                        std::string to = line.substr(i1 + 1, i2 - i1 - 1);
                        std::string msg = line.substr(i2 + 1, line.length() - i2 - 1);
                        MessagePayload message;
                        //std::cout << "Adding message: " << from << "->"<<to<<": "<<msg << std::endl;
                        message.Create(from, to, msg);
                        currentChat->AddMessage(message);

                    }
                }
            }
        }

        registeredUsers.push_back(user);
        if (currentChat != nullptr)
        {
            user->AddChat(currentChat);
            //std::cout << "Adding last chat" << std::endl;
        }
    }
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
    const std::list<UserData*>::iterator it = std::find_if(registeredUsers.begin(), registeredUsers.end(), [&] (UserData* data) { return username == data->GetUsername(); });
    if (it != registeredUsers.end())
    {
        return *it;
    }
    else
    {
        return nullptr;
    }
}

ClientSessionData* DataBaseHandler::GetUserSession(std::string username)
{
    std::list<ClientSessionData*>::iterator it = std::find_if(connectedUsers.begin(), connectedUsers.end(), [&] (ClientSessionData* data) {return username == data->GetOwner()->GetUsername(); });
    if (it != connectedUsers.end())
    {
        return *it;
    }
    else
    {
        return nullptr;
    }
}

void DataBaseHandler::AddMessage(MessagePayload message)
{
    ChatData* chat;
    ClientSessionData* fromSes = GetUserSession(message.from);
    if (fromSes != nullptr)
    {
        chat = fromSes->GetOwner()->GetChatWith(message.to);
        if (chat == nullptr)
        {   
            chat = new ChatData(message.from, message.to);
            fromSes->GetOwner()->AddChat(chat);
            //std::cout << "Adding chat" << std::endl;
        }
        //std::cout << "Adding msg: " << message.ToString() << "to  user: " << fromSes->GetOwner()->GetUsername() << std::endl;
        chat->AddMessage(message);
    }
    ClientSessionData* toSes = GetUserSession(message.to);
    if (toSes != nullptr)
    {
        chat = toSes->GetOwner()->GetChatWith(message.from);
        if (chat == nullptr)
        {
            chat = new ChatData(message.to, message.from);
            toSes->GetOwner()->AddChat(chat);
            //std::cout << "Adding chat" << std::endl;
        }
        //std::cout << "Adding msg: " << message.ToString() << "to  user: " << toSes->GetOwner()->GetUsername() << std::endl;
        chat->AddMessage(message);
    }
}

void DataBaseHandler::RegisterUser(UserData* user)
{   
    mutex.lock();
	if (!IsUserRegistered(user->GetUsername()))
	{
        registeredUsers.push_front(user);
	}
    mutex.unlock();
}

bool DataBaseHandler::IsUserRegistered(std::string username)
{
    if (registeredUsers.size() == 0)
    {
        return false;
    }
    std::list<UserData*>::iterator it = std::find_if(registeredUsers.begin(), registeredUsers.end(), [&](UserData* data) { return username == data->GetUsername(); });
    return it != registeredUsers.end();

}

std::vector<std::string> DataBaseHandler::GetFilenames(std::string path) const
{
    std::vector<std::string> filenames;
    struct dirent* entry;
    DIR* dir = opendir(path.c_str());

    if (dir == NULL) 
    {
        return filenames;
    }
    std::regex filesRegex ("[A-Za-z0-9]+.data");
    while ((entry = readdir(dir)) != NULL) 
    {
        if (std::regex_match(entry->d_name, filesRegex))
        {
            filenames.push_back(entry->d_name);

        }
    }
    closedir(dir);
    return filenames;
}
