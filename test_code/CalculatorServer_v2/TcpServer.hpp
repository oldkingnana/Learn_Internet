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
	class TcpServer : public TcpBase 
	{
	public:
		// constructer & destructer
		TcpServer(const uint16_t& server_port, std::function<void(oldking::Socket)> func)
		: addr_in_("0.0.0.0", server_port)
		, listen_socket_fd_(-1)
		, func_(func)
		{
			init();
		}

		~TcpServer() { clear(); }
	
		void run()
		{
			// start listening
			listen_socket();
			
			// start accept 
			while(true)
			{
				Socket sock = accept_socket();
				pid_t pid = fork();
				if(pid == 0)
				{
					// child 
					close(listen_socket_fd_);
					pid_t n = fork();
					if(n == 0)
						func_(sock);
					exit(OK);
				}
				else if(pid > 0)
				{
					// parent
					sock.close();		
				}
				else 
				{
					// err 
					oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, FILENAME_TCPSERVER, "fork err");
					exit(FORK_ERR);
				}
			}
		}

	private:
		// API
		void listen_socket()
		{
			int16_t listen_result = listen(listen_socket_fd_, 6);
			if(listen_result < 0)
			{
				oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, FILENAME_TCPSERVER, "listen err");
				exit(LISTEN_ERR);
			}
		}

		Socket accept_socket()
		{
			sockaddr_in addr;
			bzero(&addr, sizeof(addr));
			socklen_t len;
			int16_t sockfd = accept(listen_socket_fd_, (struct sockaddr *)(&addr), &len);	
			if(sockfd < 0)
			{
				oldking::MyEasyLog::GetInstance().WriteLog(LOG_WARNING, FILENAME_TCPSERVER, "accept err");
				return Socket(-1, addr_in_);
			}
		
			Addr_in addr_in(addr);
			return Socket(sockfd, addr_in);
		}

		void init()
		{
			// init
			listen_socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
			if(listen_socket_fd_ < 0)
			{
				oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, FILENAME_TCPSERVER, "socket err");
				exit(SOCK_ERR);	
			}
		
			struct sockaddr_in addr_in = addr_in_.getAddr_in();
		
			int16_t bind_result = bind(listen_socket_fd_, (struct sockaddr*)(&addr_in), sizeof(addr_in));
			if(bind_result < 0)
			{
				oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, FILENAME_TCPSERVER, "bind err");
				exit(BIND_ERR);	
			}
		}

		void clear()
		{
			close(listen_socket_fd_);
		}

	private:
		Addr_in addr_in_;
		int16_t listen_socket_fd_;
		std::function<void(oldking::Socket)> func_;
	};

}
