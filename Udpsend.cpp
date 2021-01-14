#include <thread>
#include <stdio.h> 
#include <string.h>
#include <unistd.h>
#include <string>
#include <sys/socket.h> 
#include <iostream>
#include <unistd.h> 
#include <signal.h>
#include <bit>
#include <netinet/in.h>
#include <iostream>
#include <arpa/inet.h>
#include <cmath>
#include <fstream>
void PrintBinChar(char val)
{
	for(int i = 0;i < 8; i++)
	{
		int temp = val >> i;
		if(temp % 2 == 0) std::cout<<"0";
		else std::cout<<"1";
	}
	std::cout<<" ";
}

void PrintBinInt(int val)
{
	for(int i = 0;i < 32; i++)
	{
		int temp = val >> i;
		if(temp % 2 == 0) std::cout<<"0";
		else std::cout<<"1";
		if((i+1)%8==0) std::cout<< " ";
	}
	std::cout<<" ";
}
void PutInt(char* startAddress, int value)
{
	int size = sizeof(int);
	for (int i = 0; i < size; i++)
	{
		startAddress[i] = 0x0;
		startAddress[i] = (unsigned int)value >> (8 * i);
	}
	
}
int ReadInt(char* currentAddress)
{
	unsigned int res = 0x0;
	int size = sizeof(int);
	for (int i = 0; i < size; i++)
	{
		res |= (unsigned char)currentAddress[i] << (8 * i);
	}
	return res;
}



int main()
{
	/*PrintBinChar(191);
	std::cout<<std::endl;
	PrintBinInt(191);
	std::cout<<std::endl;
	PrintBinInt(0 | 191);*/
    int sock;
    struct sockaddr_in servaddr, cliaddr;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		std::cout<<strerror(errno);
	}

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5051);

	// Convert IPv4 and IPv6 addresses from text to binary form, if the conversion fails, the ip format is not correct
	if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0)
	{
		std::cout<<strerror(errno);
	}
	
	std::ifstream file("test.data", std::ifstream::in | std::ios::ate);
	if (file.fail())
	{
		std::cerr << "Unable to open recording file" << std::endl;
		exit(1);
	}
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);
	
	char* buffer = new char[4100];   

	int segs = ceil((float)size / 4096);
	int packets = 0;
	int tot = 0;
	char handshake[sizeof(int)*2];
	
	std::cout<<"Handshake: "<<segs<<", "<<size<<std::endl;
	
	PutInt(handshake, segs);
	PutInt(handshake + sizeof(int), size);

	std::cout<<std::endl;
	std::cout<<ReadInt(handshake)<<std::endl;
	std::cout<<ReadInt(handshake+sizeof(int))<<std::endl;
	int sent = sendto(sock, handshake, 2*sizeof(int), MSG_CONFIRM, (const struct sockaddr*)&servaddr, sizeof(servaddr));

	int i = 0;
	while(!file.eof()) 
	{
		PutInt(buffer, i);
		file.read(buffer+4, 4096);
		std::streamsize s=file.gcount();

		std::cout<<"Sending seg: "<<i<<std::endl;

		int sent = sendto(sock, buffer, s, 0, (const struct sockaddr*)&servaddr, sizeof(servaddr));
		if(sent == -1)
		{
			std::cout<<strerror(errno);
			exit(1);
		}
		i++;
		tot += sent;
		packets++;
		//std::cout<<"Sent: "<<sent<<" over udp"<<std::endl;
		usleep(25);
	}
	std::cout<< "TOT: "<< tot<<"Packts: "<<packets<<std::endl;
}



