#include "TcpServer.hh"
#include <iostream>
#include <ostream>
#include <string>

std::string func(std::string msg)
{
	if(msg == "ok")
	{
		return "yes, ok";
	}
	else 
	{
		return "hello world";
	}
}

int main(int argc, char *argv[])
{
	if(argc != 3)
	{
		std::cerr << "argv err" << std::endl;
		std::cerr << "type:" << std::endl;
		std::cerr << "server -ip=<ip> -port=<port>" << std::endl;

		return -1;
	}

	std::string ip_str = argv[1];
	std::string port_str = argv[2];

	ip_str = ip_str.substr(4);
	port_str = port_str.substr(6);

	std::cout << "ip -> " << ip_str << std::endl;
	std::cout << "port -> " << port_str << std::endl;


	oldking::TcpServer tcp(ip_str, std::stoi(port_str), func);
	if(!tcp.init())
	{
		std::cerr << "tcp init err" << std::endl;
		return -1;
	}

	tcp.start();

	return 0;
}





