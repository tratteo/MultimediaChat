#include "../include/Serializer.hpp"


Serializer::Serializer(std::string filePath)
{
    this->filePath = filePath;
}

bool Serializer::Append(std::string value)
{
    outStream.open(filePath, std::ofstream::out | std::ofstream::app);
    if (outStream.fail())
    {
        return false;
    }
    outStream << value;
    outStream.close();
    return true;
}

bool Serializer::Clean()
{
    outStream.open(filePath, std::ofstream::trunc | std::ofstream::out);
    if(outStream.fail())
    {
        return false;
    }
    outStream.close();
    return true;
}

std::list<std::string> Serializer::GetLines()
{
    std::string line;
    std::list<std::string> lines;
    inStream.open(filePath, std::ifstream::in);
    if(inStream.fail())
    {
        return lines;
    }
    while (std::getline(inStream, line))
    {
        lines.push_front(line);
    }
    inStream.close();
    return lines;
}

bool Serializer::FileValid()
{
    struct stat buffer;
    return stat(filePath.c_str(), &buffer) == 0;
}