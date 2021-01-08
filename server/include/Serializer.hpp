#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <list>
#include "sys/stat.h"
#include "../../common/errhandler.hpp"

class Serializer
{
    public:
    Serializer(std::string fileName);
    bool Append(std::string value);
    bool Clean();
    std::list<std::string> GetLines();

    private:
    int fd;
    std::ifstream inStream;
    std::ofstream outStream;
    std::string filePath;
    bool FileValid();


};