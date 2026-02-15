#pragma once 

#include "TcpBase.hh"
#include "Addr.hh"
#include "common.hh"
#include "myeasylog.hpp"
#include "Socket.hh"

#include <memory>
#include <string>
#include <cstdint>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <functional>

namespace oldking 
{
	class TcpClient : public TcpBase 
	{
	public:
		// constructer & destructer
		TcpClient(const std::string& server_ip, const uint16_t& server_port, std::function<void(std::shared_ptr<oldking::Socket>)> func)
		: addr_in_(server_ip, server_port)
		, func_(func)
		{
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, FILENAME_TCPCLIENT, "constructor");
		}

		~TcpClient() { }
	
		void run() 
		{ 
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, FILENAME_TCPCLIENT, "run begin!");
			func_(get_socket()); 
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, FILENAME_TCPCLIENT, "run finish!");
		}

	private:
		std::shared_ptr<Socket> get_socket()
		{
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, FILENAME_TCPCLIENT, "get socket begin!");
			int16_t socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);	
			if(socket_fd_ < 0)
			{
				oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, FILENAME_TCPCLIENT, "socket err");
				exit(SOCK_ERR);
			}
		
			struct sockaddr_in addr_in = addr_in_.getAddr_in();
			socklen_t len = sizeof(addr_in);
		
			if(connect(socket_fd_, (struct sockaddr*)(&addr_in), len) < 0)
			{
				oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, FILENAME_TCPCLIENT, "connect err");
				exit(CONNECT_ERR);
			}
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, FILENAME_TCPCLIENT, "get socket finish!");
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, FILENAME_TCPCLIENT, "get socket: " + std::to_string(socket_fd_));
			
			return std::make_shared<oldking::Socket>(socket_fd_, addr_in_);
		}

	private:
		Addr_in addr_in_;
		std::function<void(std::shared_ptr<oldking::Socket>)> func_;
	};

}
