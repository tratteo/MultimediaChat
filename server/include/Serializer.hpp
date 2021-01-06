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
    std::fstream stream;
    std::string filePath;
    bool FileValid();


};