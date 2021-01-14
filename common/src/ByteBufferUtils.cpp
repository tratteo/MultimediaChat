#include "../ByteBufferUtils.hpp"

void PutString(char* startAddress, std::string value) 
{
	int len = value.size();
	for (int i = 0; i < len; i++)
	{
		startAddress[i] = value.at(i);
	}
}

void PutUInt(char* startAddress, int value) 
{
	int size = sizeof(int);
	for (int i = 0; i < size; i++)
	{
		startAddress[i] = 0x0;
		startAddress[i] = (unsigned int)value >> (8 * i);
	}
}

int ReadUInt(char* currentAddress) 
{
	unsigned int res = 0x0;
	int size = sizeof(int);
	for (int i = 0; i < size; i++)
	{
		res |= (unsigned char)currentAddress[i] << (8 * i);
	}
	return res;
}

std::string ReadString(char* currentAddress, int length) 
{
	std::string res = "";
	for (int i = 0; i < length; i++)
	{
		res += currentAddress[i];
	}
	return res;
}