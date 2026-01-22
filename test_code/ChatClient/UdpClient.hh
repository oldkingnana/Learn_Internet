#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "myeasylog.hpp"
#include "UdpBase.hh"

namespace oldking
{
	#define DEFAULT_IP "0.0.0.0"
	#define DEFAULT_PORT -1

	class UdpClient : UdpBase 
	{
	public:
		UdpClient()
		: ip_(DEFAULT_IP)
		, port_(DEFAULT_PORT) 
		, is_init_(false)
		, iaddr_({})
		, iport_(-1)
		, server_({})
		, sockfd_(-1)
		{
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, "UdpClient", "constructor");
		}
		
		void stop() {close(sockfd_);}
		
		~UdpClient()
		{
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, "UdpClient", "distructor");
		}

		bool init(std::string ip, uint16_t port);
	
		int16_t Send(const std::string& msg);

		int16_t Recv(std::string& msg);

	private:

		const UdpClient& operator<<(const std::string& msg);
		
		const UdpClient& operator>>(std::string& msg);

	private:
		std::string ip_;
		uint16_t port_;
		bool is_init_;
	
		in_addr iaddr_;
		in_port_t iport_;
	
		sockaddr_in server_;
		
		int64_t sockfd_;
	};
}

