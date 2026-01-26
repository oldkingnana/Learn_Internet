#include "TcpServer.hh"
#include "myeasylog.hpp"

#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <arpa/inet.h>
#include <unistd.h>

void oldking::TcpServer::listen_peer()
{
	int16_t listen_result = listen(listen_socket_fd_, 6);
	if(listen_result < 0)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, FILENAME, "listen err");
		exit(LISTEN_ERR);
	}
}

bool oldking::TcpServer::accept_peer(oldking::TcpServer::Peer& peer)
{
	sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	socklen_t len;
	int16_t sockfd = accept(listen_socket_fd_, (struct sockaddr *)(&addr), &len);	
	if(sockfd < 0)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_WARNING, FILENAME, "accept err");
		return false;
	}

	Addr_in addr_in(addr);
	Peer peer_tmp(sockfd, addr_in);

	peer = peer_tmp;

	for(auto& it : peer_list_)
	{
		if(peer_tmp.sockfd_ == it.sockfd_)
		{
			it = peer_tmp;
			return true;	
		}
	}	

	peer_list_.push_back(peer_tmp);

	return true;
}

bool oldking::TcpServer::send(const Peer& peer, const std::string& msg)
{
	if(write(peer.sockfd_, msg.c_str(), msg.length()) < 0)	
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_WARNING, FILENAME, "write err");
		return false;
	}

	return true;
}

bool oldking::TcpServer::recv(const Peer& peer, std::string& buff)
{
	char cbuff[MAX_STR];
	int16_t read_len = read(peer.sockfd_, cbuff, MAX_STR - 1);
	
	if(read_len < 0)
	{
		
	}
	else if(read_len == 0)
	{

	}
	else 
	{
		
	}

	return true;
}

void oldking::TcpServer::init()
{
	// init
	listen_socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_socket_fd_ < 0)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, FILENAME, "socket err");
		exit(SOCK_ERR);	
	}

	struct sockaddr_in addr_in = addr_in_.getAddr_in();

	int16_t bind_result = bind(listen_socket_fd_, (struct sockaddr*)(&addr_in), sizeof(addr_in));
	if(bind_result < 0)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, FILENAME, "bind err");
		exit(BIND_ERR);	
	}
}

void oldking::TcpServer::clear()
{
	for(auto it = peer_list_.begin(); it != peer_list_.end(); )
	{
		if(close(it->sockfd_) == -1)
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_WARNING, FILENAME, "close err, peer fd is " + std::to_string(it->sockfd_));
		it = peer_list_.erase(it);
	}

	if(close(listen_socket_fd_))
				oldking::MyEasyLog::GetInstance().WriteLog(LOG_WARNING, FILENAME, "close err, listen_socket_fd_ is " + std::to_string(listen_socket_fd_));
}

bool oldking::TcpServer::clear(const Peer& peer)
{
	
}

