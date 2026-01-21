#include <cstdint>
#include <string>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "myeasylog.hpp"
#include "UdpClient.hh"

bool oldking::UdpClient::init(std::string ip, uint16_t port)
{
	oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, "UdpClient", "init begin");

	is_init_ = true;	

	sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd_ == -1)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_ERROR, "UdpClient", "init error! socket failed!");
		is_init_ = false;
		return false;
	}

	bzero(&server_, sizeof(server_));
	bzero(&iaddr_, sizeof(iaddr_));

	// ip 
	if(inet_aton(ip.c_str(), &iaddr_) == 0)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_ERROR, "UdpClient", "init error! ip failed!");
		is_init_ = false;
		return false;	
	}
	ip_ = ip;
	server_.sin_addr = iaddr_;	

	// port 
	iport_ = htons(port);	
	port_ = port;
	server_.sin_port = iport_;
	
	// family 
	server_.sin_family = AF_INET;

	oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, "UdpClient", "init finish");

	return true;
}

const oldking::UdpClient& oldking::UdpClient::operator<<(const std::string& msg)
{
	oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, "UdpClient", "input aka '<<' begin");
	
	if(!is_init_)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_WARNING, "UdpClient", "UdpClient is not init!");
		return *this;
	}

	sendto(sockfd_, msg.c_str(), msg.size(), 0, (struct sockaddr *)&server_, sizeof(server_));

	oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, "UdpClient", "input aka '<<' finish");

	return *this;
}

const oldking::UdpClient& oldking::UdpClient::operator>>(std::string& msg)
{
	oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, "UdpClient", "output aka '>>' begin");

	if(!is_init_)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_WARNING, "UdpClient", "UdpClient is not init!");
		return *this;
	}
	
	char buffer[1024];

	sockaddr_in server;
	bzero(&server, sizeof(server));
	socklen_t server_len;
	bzero(&server_len, sizeof(server_len));

	int16_t recv_len = recvfrom(sockfd_, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&server, &server_len);	
	if(recv_len > 0)
	{
		buffer[recv_len] = 0;
		msg = buffer;
	}

	oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, "UdpClient", "output aka '>>' finish");
	
	return *this;
}

