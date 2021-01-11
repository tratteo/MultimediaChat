#include "../include/Serializer.hpp"


bool Overwrite(std::string value, std::string path, std::string filename)
{
    CheckPath(path);
    std::ofstream outStream;
    outStream.open(path + "/" + filename, std::ofstream::trunc | std::ofstream::out);
    if (outStream.fail())
    {
        return false;
    }
    outStream << value;
    outStream.close();
    return true;
}

bool Append(std::string value, std::string path, std::string filename)
{
    CheckPath(path);
    std::ofstream outStream;
    outStream.open(path + "/" + filename, std::ofstream::out | std::ofstream::app);
    if (outStream.fail())
    {
        return false;
    }
    outStream << value;
    outStream.close();
    return true;
}

bool Clean(std::string path, std::string filename)
{
    std::ofstream outStream;
    outStream.open(path + "/" + filename, std::ofstream::trunc | std::ofstream::out);
    if(outStream.fail())
    {
        return false;
    }
    outStream.close();
    return true;
}

std::list<std::string> GetLines(std::string path, std::string filename)
{
    std::ifstream inStream;
    std::string line;
    std::list<std::string> lines;
    inStream.open(path + "/" + filename, std::ifstream::in);
    if(inStream.fail())
    {
        return lines;
    }
    while (std::getline(inStream, line))
    {
        lines.push_back(line);
    }
    inStream.close();
    return lines;
}

bool CheckPath(std::string path)
{
    struct stat info;
    struct stat buffer;
    if ((stat(path.c_str(), &buffer) != 0))
    {
        mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        return false;
    }
    else
    {
        return true;
    }
}
