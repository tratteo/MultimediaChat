#include <thread>
#include <stdio.h> 
#include <string.h>
#include <unistd.h>
#include <string>
#include <sys/socket.h> 
#include <unistd.h> 
#include <signal.h>
#include <netinet/in.h>
#include <iostream>
#include <arpa/inet.h>
#include <fstream>

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
    struct sockaddr_in servaddr, cliaddr;
    int sock;
	// Creating socket file descriptor 
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    { 
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    } 

    // Filling server information 
    servaddr.sin_family = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(5051);

    // Bind the socket with the server address 
    if (bind(sock, (const struct sockaddr*)&servaddr,
        sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
	unsigned int len = sizeof(cliaddr);
	char buffer[4100] = {0};
    int tot = 0;
    int packets = 0;
    int n = recvfrom(sock, buffer, 4100, 0, (struct sockaddr*)&cliaddr, &len);
    
    int segs = ReadInt(buffer);
    int size = ReadInt(buffer +sizeof(int));
    std::cout<<"Received handshake "<<n<<", waiting "<<segs<<" packets, tot l: "<<size<<std::endl;
    memset(&buffer, 0, 4100);
    char matrix[segs][4096] = {0}; 
    int lengths[segs] = {0};
    int i = 1;
    while(i <= segs)
    {
		//std::cout<<"Receiving udp"<<std::endl;
		n = recvfrom(sock, buffer, 4100, 0, (struct sockaddr*)&cliaddr, &len);
		int number = ReadInt(buffer);
		std::cout<<"Received seg: "<<number<<std::endl;
		if(n > 0)
		{		
			tot += n;
			packets++;
			i++;
		}
		lengths[number] = n-4;
		memcpy(matrix[number], buffer + 4, 4096);
		memset(&buffer, 0, 4100);
		//std::cout << "From udp thread, received: " << n << std::endl;

    }
    		std::cout<< "TOT: "<< tot<<"Packts: "<<packets<<std::endl;
    std::ofstream out("received.data", std::ios::trunc | std::ios::out);
    for(int i = 0; i < segs; i++)
    {
    	out.write(matrix[i], lengths[i]);
    }
    out.close();
    
    

}
