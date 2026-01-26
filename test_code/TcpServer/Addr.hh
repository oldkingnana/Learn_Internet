#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <cstdint>
#include <arpa/inet.h>

#include "myeasylog.hpp"
#include "common.hh"

namespace oldking
{
	class Addr_in
	{
	public:
		Addr_in(const std::string& ip, const uint16_t& port)
		: IP_(ip)
		, port_(port)
		, Addr_in_({})
		{
			Addr_in_.sin_family = AF_INET;
			Addr_in_.sin_addr.s_addr = inet_addr(ip.c_str()); 
			Addr_in_.sin_port = htons(port);
		}

		Addr_in(const struct sockaddr_in& addr_in)
		: IP_({})
		, port_(0)
		, Addr_in_(addr_in) 
		{
			char buff[64];	
			const char* str = inet_ntop(AF_INET, (void*)(&addr_in.sin_addr), buff, sizeof(buff));
			if(str != nullptr)
				IP_ = str; 
			else
			{
				oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, "Addr", "inet_ntop err");
				exit(ADDR_ERR);
			}

			port_ = ntohs(addr_in.sin_port);
		}

		~Addr_in()
		{}

		bool operator==(const Addr_in& other)
		{
			return IP_ == other.IP_ && 
				   port_ == other.port_;
		}

		std::string getIP() {return IP_;}

		uint16_t getPort() {return port_;}

		struct sockaddr_in getAddr_in() {return Addr_in_;}

	private:
		std::string IP_;
		uint16_t port_;
		struct sockaddr_in Addr_in_;
	};
}



