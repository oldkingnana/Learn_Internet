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

void oldking::UdpServer::send(const std::string& msg, const std::string& dst_ip, const int16_t& dst_port)
{
	struct sockaddr_in peer;
	bzero(&peer, sizeof(peer));

	peer.sin_family = AF_INET;
	peer.sin_port = htons(dst_port);
	peer.sin_addr.s_addr = inet_addr(dst_ip.c_str()); 

	socklen_t len = sizeof(peer);

	int16_t send_result = sendto(
			socket_fd_, 
			msg.c_str(), 
			msg.length(), 
			0, 
			(struct sockaddr*)&peer, 
			len
			);

	if(send_result < 0)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_WARNING, "UdpServer", "send err");
		return ;
	}

}

void oldking::UdpServer::recv(std::string& buff, std::string& sender_ip, int16_t& sender_port)
{
	struct sockaddr_in peer;
	bzero(&peer, sizeof(peer));
	socklen_t len = sizeof(peer);

	char c_buff[MSG_MAX];	
	c_buff[MSG_MAX - 1] = 0;

	int16_t recv_result = recvfrom(
			socket_fd_, 
			c_buff, 
			sizeof(c_buff) - 1, 
			0, 
			(struct sockaddr *)(&peer), 
			&len
			);

	if(recv_result < 0)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, "UdpServer", "recv err");
		return ;
	}

	buff = c_buff;
	sender_ip = inet_ntoa(peer.sin_addr);
	sender_port = ntohs(peer.sin_port);
	
	return ;
}


