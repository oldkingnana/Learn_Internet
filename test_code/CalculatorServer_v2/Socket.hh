#pragma once 

#include "Addr.hh"
#include "common.hh"
#include "myeasylog.hpp"

#include <string>
#include <unistd.h>

namespace oldking
{
	#define DEFAULTFD -1

	// Peer
	class Socket
	{
	public:
		Socket()
		: sockfd_(DEFAULTFD)
		, addr_({})
		{}

		Socket(const int& sockfd, const Addr_in& addr_in)
		: sockfd_(sockfd)
		, addr_(addr_in)
		{}

		Socket(const Socket& other) = delete;
		
		Socket(Socket&& other) noexcept
		: sockfd_(other.sockfd_)
		, addr_(other.addr_)
		{
    		other.sockfd_ = -1;
		}
		
		~Socket() 
		{
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, FILENAME_SOCK, "close begin!");
			if(sockfd_ != -1) 
			{
				::close(sockfd_); 
				sockfd_ = -1;
			}
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, FILENAME_SOCK, "close finish!");
		}

		Socket& operator=(const Socket& other) = delete; 

		bool operator==(const Socket& other) const; 

		std::string getIP(){ return addr_.getIP(); }

		std:: int16_t getPort(){ return addr_.getPort(); }
		
		bool send(const std::string& msg);

		bool recv(std::string& buff);

		void close()
		{
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, FILENAME_SOCK, "close begin!");
			if(sockfd_ != -1) 
			{
				::close(sockfd_); 
				sockfd_ = -1;
			}
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, FILENAME_SOCK, "close finish!");
		}

	private:
		int sockfd_;
		Addr_in addr_;
	};
}
