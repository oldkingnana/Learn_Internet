#include <string>

#include "UdpServer.hh"
#include "myeasylog.hpp"

std::string func(std::string msg)
{
	oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, "test", "in func");

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
	oldking::UdpServer server(7777, func);
	if(server.init() == false)
	{
		return -1;
	}
	server.start();
	return 0;
}

