#include "../include/Serializer.hpp"


Serializer::Serializer(std::string filePath)
{
    this->filePath = filePath;
}

bool Serializer::Append(std::string value)
{
    value.append("\n");
    stream.open(filePath, std::fstream::out | std::fstream::app);
    if(!stream.good())
    {
        return false;
    }
    stream << value;
    stream.close();
    return true;
}

bool Serializer::Clean()
{
    stream.open(filePath, std::fstream::trunc | std::fstream::out);
    if(!stream.good())
    {
        return false;
    }
    stream.close();
    return true;
}

std::list<std::string> Serializer::GetLines()
{
    std::string line;
    std::list<std::string> lines;
    stream.open(filePath, std::fstream::in);
    if(!stream.good())
    {
        return lines;
    }
    while (std::getline(stream, line)) 
    {
        lines.push_front(line);
    }
    return lines;
}

bool Serializer::FileValid()
{
    struct stat buffer;
    return stat(filePath.c_str(), &buffer) == 0;
}