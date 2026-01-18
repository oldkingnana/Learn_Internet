#include "mutex.hpp"
#include "myeasylog.hpp"
#include "TcpClient.hh"

#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <arpa/inet.h>
#include <unistd.h>

bool oldking::TcpClient::init()
{
	socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);	
	if(socket_fd_ < 0)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, FILENAME, "socket err");
		return false;
	}

	return true;
}


void oldking::TcpClient::start()
{
	while(true)
	{
		std::string msg = func_("");

		struct sockaddr_in info;
		bzero(&info, sizeof(info));

		info.sin_family = AF_INET;
		info.sin_port = htons(port_);
		
		in_addr in;
		bzero(&in, sizeof(in));

		inet_aton(ip_.c_str(), &in);
		info.sin_addr = in;

		socklen_t infolen = sizeof(info);

		int16_t send_result = sendto(
				socket_fd_, 
				msg.c_str(), 
				msg.length(), 
				0, 
				(struct sockaddr *)(&info), 
				infolen
				);
	
		if(send_result < 0)
		{
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, FILENAME, "send err");
			return ;
		}

		char buff[1024];

		//struct sockaddr_in server_info;
		//bzero(&server_info, sizeof(server_info));
		//socklen_t server_infolen = sizeof(server_info); 

		recvfrom(socket_fd_, buff, sizeof(buff) - 1, 0, nullptr, nullptr);
		std::string buff_str = buff;

		std::cout << buff_str << std::endl;
	}
}


