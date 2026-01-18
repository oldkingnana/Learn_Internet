#include "TcpServer.hh"
#include "mutex.hpp"
#include "myeasylog.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <arpa/inet.h>
#include <unistd.h>

bool oldking::TcpServer::init()
{
	socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd_ < 0)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, FILENAME, "socket err");
		return false;
	}

	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(server_port_);
	socklen_t addrlen = sizeof(addr);

	int16_t bind_result = bind(socket_fd_, (struct sockaddr*)(&addr), addrlen);
	if(bind_result < 0)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, FILENAME, "bind err");
		return false;
	}

	return true;
}

void oldking::TcpServer::start()
{
	state_ = RUNNING;

	while(true)
	{
		char buff[1024];

		struct sockaddr_in client_info;
		socklen_t client_info_len = sizeof(client_info);

		// receive
		int16_t recv_result = recvfrom(
				socket_fd_, 
				buff, 
				sizeof(buff) - 1, 
				0, 
				(struct sockaddr *)(&client_info), 
				&client_info_len
				);
		if(recv_result < 0)
		{
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, FILENAME, "recv err");
			exit(-1);
		}
		
		// do
		buff[1023] = 0;
		std::string buff_str = buff;
		std::string result_str = func_(buff);
		
		// send back
		int16_t send_result = sendto(
				socket_fd_, 
				result_str.c_str(), 
				result_str.length(), 
				0, 
				(struct sockaddr *)(&client_info), 
				client_info_len
				);

		if(send_result < 0)
		{
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, FILENAME, "send err");
			exit(-1);
		}

	}

	return ;
}

