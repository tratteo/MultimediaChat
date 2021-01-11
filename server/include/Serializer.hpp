#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <list>
#include <filesystem>
#include <sys/stat.h> 
#include <errno.h> 
#include "../../common/errhandler.hpp"


bool Append(std::string value, std::string path, std::string filename);
bool Overwrite(std::string value, std::string path, std::string filename);
bool Clean(std::string path, std::string filename);
std::list<std::string> GetLines(std::string path, std::string filename);
bool CheckPath(std::string path);