#include "TcpClient.hh"
#include <string>
#include <iostream>

std::string func(std::string str)
{
	(void)str;
	std::cout << "say something: " << std::endl;
	std::string msg;

	std::cin >> msg;

	return msg;
}

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		std::cerr << "argv err" << std::endl;
		return -1;
	}

	oldking::TcpClient TcpC(argv[1], std::stoi(argv[2]), func);

	TcpC.init();
	TcpC.start();

	return 0;
}





