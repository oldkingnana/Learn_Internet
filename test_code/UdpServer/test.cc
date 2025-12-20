#include <string>

#include "UdpServer.hh"

std::string func(std::string msg)
{
	if(msg == "How are you?")
	{
		return "I am fine!";
	}
	else 
	{
		return "I dont understand!";
	}
}

int main()
{
	oldking::UdpServer server(1080, func);
	if(server.init() == false)
	{
		return -1;
	}
	server.start();
	return 0;
}

