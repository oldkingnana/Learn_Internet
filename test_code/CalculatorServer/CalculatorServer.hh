#pragma once

#include "TcpServer.hpp"
#include "Protocol.hh"
#include "Socket.hh"
#include "common.hh"
#include "myeasylog.hpp"

#include <list>
#include <memory>
#include <string>
#include <sys/wait.h>

namespace oldking 
{
	class CalculatorServer
	{
	public:
		CalculatorServer(int16_t port)
		: pTP_(std::make_unique<TcpServer<Structor>>(port, protocol_))
		{}

		~CalculatorServer()
		{}

		void run()
		{
			pTP_->listen_socket();
			while(true)
			{
				Socket sock = pTP_->accept_socket();
				if(sock.is_closed())
					return ;
				
				int pid = fork();		
				if(pid > 0)
				{
					// parent
					sock.close();
					int status = 0;
					waitpid(pid, &status, 0);
					(void)status;
				}
				else if(pid == 0)
				{
					// child
					pTP_.reset();	
	
					int m = fork();
					if(m == 0)
					{
						std::cout << "IP: " + sock.getIP() + " connect" << std::endl;
					
						//task
						std::string buff;
						std::string result;
						while(true)
						{
							if(sock.recv(buff))
							{
								Structor s = protocol_(buff);
								switch (s.oper_) 
								{
								case '+':
									result = std::to_string(s.x_ + s.y_);
									break;
								case '-':
									result = std::to_string(s.x_ - s.y_);
									break;
								case '*':
									result = std::to_string(s.x_ * s.y_);
									break;
								case '/':
									result = std::to_string(s.x_ / s.y_);
									break;
								case '%':
									result = std::to_string(s.x_ % s.y_);
									break;
								default:
									result = "operator err!";
									break;
								}

								if(sock.send(result + "\0") == false)
								{
									std::cout << "IP: " + sock.getIP() << " quit!" << std::endl;
									exit(1);
								}
							}
							else
							{
								std::cout << "IP: " + sock.getIP() << " quit!" << std::endl;
								exit(1);
							}
						}
					}
					exit(1);
				}
				else 
				{
					oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, "CalculatorServer", "fork err");
					exit(FORK_ERR);	
				}
			}
		}

	private:
		Protocol protocol_;	
		std::unique_ptr<TcpServer<Structor>> pTP_;
	};
}

