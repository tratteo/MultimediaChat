#pragma once
#include <string>

void PutString(char* startAddress, std::string value);
void PutUInt(char* startAddress, int value);
int ReadUInt(char* currentAddress);
std::string ReadString(char* currentAddress, int length);