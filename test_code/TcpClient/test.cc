#include "TcpClient.hh"

#include <string>
#include <iostream>
#include <sys/socket.h>

void usage()
{
	std::cout << "Usage: " << std::endl; 
	std::cout << "./Client -ip=<ip> -port=<port>" << std::endl;
}


int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		usage();	
		return -1;
	}

	std::string ip = std::string(argv[1]).substr(4);
	int16_t port = std::stoi(std::string(argv[2]).substr(6));

	oldking::TcpClient tcp(ip, port);

	tcp.init();

	std::string msg;
	std::string buff;
	
	while(true)
	{
		std::getline(std::cin, msg);
		tcp.send(msg);
		tcp.recv(buff);
		std::cout << buff << std::endl;
		msg.clear();
		buff.clear();
	}

	return 0;
}



