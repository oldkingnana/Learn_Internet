#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>

int main()
{
	uint32_t IP = 0b11000000101010000000000000000001; 
	// 192.168.0.1
	// 11000000 10101000 00000000 00000001 

	std::cout << "host_IP: " << IP << "  ->" << "  network_IP: " << htonl(IP) << std::endl; 
	// host_IP: 3232235521  ->  network_IP: 16820416
	// 11000000 10101000 00000000 00000001 -> 00000001 00000000 10101000 11000000 

	std::cout << "network_IP: " << IP << "  ->" << "  host_IP: " << ntohl(IP) << std::endl; 
	// network_IP: 3232235521  ->  host_IP: 16820416
	// 11000000 10101000 00000000 00000001 -> 00000001 00000000 10101000 11000000 

	return 0;
}

