#include "UdpServer.hh"
#include "myeasylog.hpp"

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>

bool oldking::UdpServer::init()
{
	oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, "UdpServer", "init begin");

	socket_fd_ = socket(AF_INET, SOCK_DGRAM, 0);		
	
	// err
	if(socket_fd_ < 0)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_ERROR, "UdpServer", "socket error");
		return false;	
	}

	struct sockaddr_in info;
	bzero(&info, sizeof(info));

	info.sin_port = htons(port_);
	info.sin_family = AF_INET;
	info.sin_addr.s_addr = INADDR_ANY;

	int64_t bind_result = bind(socket_fd_, (struct sockaddr*)&info, sizeof(info));
	
	// err
	if(bind_result < 0)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_ERROR, "UdpServer", "bind error");
		return false;
	}

	oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, "UdpServer", "init finish");

	return true;
}
void oldking::UdpServer::start()
{
	oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, "UdpServer", "start begin");

	is_running_ = true;

	char buff[1024];

	while(true)
	{
		// recv
		struct sockaddr_in peer;
		socklen_t peer_len = sizeof(peer);

		int16_t recv_len = recvfrom(socket_fd_, buff, sizeof(buff) - 1, 0, (struct sockaddr *)&peer, (socklen_t *)&peer_len);

		if(recv_len > 0)
		{
			int16_t peer_port = ntohs(peer.sin_port);
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, "UdpServer", "peer ip: " + std::string(inet_ntoa(peer.sin_addr)) + " peer port: " + std::to_string(peer_port));
			
			// func
			buff[recv_len] = 0;
			std::string msg = buff;
			std::string result = func_(msg);
			// send back
			sendto(socket_fd_, result.c_str(), sizeof(result.c_str()) - 1, 0, (struct sockaddr *)&peer, peer_len);
		}
		else 
		{
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_WARNING, "UdpServer", "recv_len <= 0");
		}
	}
	return ;
}

