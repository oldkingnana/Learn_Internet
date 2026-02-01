#pragma once 

#include "Addr.hh"
#include "common.hh"
#include "myeasylog.hpp"

#include <string>

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
		{
			check_fd();
		}

		Socket(const int& sockfd, const Addr_in& addr_in)
		: sockfd_(sockfd)
		, addr_(addr_in)
		{
			check_fd();
		}

		Socket(const Socket& other)
		: sockfd_(other.sockfd_)
		, addr_(other.addr_)
		{
			check_fd();
		}
		
		~Socket() { close(); }

		Socket& operator=(const Socket& other); 

		bool operator==(const Socket& other) const; 

		std::string getIP(){ return addr_.getIP(); }

		std:: int16_t getPort(){ return addr_.getPort(); }
		
		bool send(const std::string& msg);

		bool recv(std::string& buff);

		bool is_closed() { return is_closed_; }
		
		void close() { ::close(sockfd_); is_closed_ = true; }

	private:
		void check_fd();
		
	
	private:
		int sockfd_;
		Addr_in addr_;
		bool is_closed_;
	};
}
