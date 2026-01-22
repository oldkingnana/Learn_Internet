#include "ChatClient.hh"

#include <iostream>

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		std::cout << "usage: ./ChatClient <ip> <port>" << std::endl;
		return -1;
	}

	std::string ip = argv[1];

	int16_t port = std::stoi(std::string(argv[2]));

	oldking::ChatClient CC(ip, port);

	CC.start();

	return 0;
}

