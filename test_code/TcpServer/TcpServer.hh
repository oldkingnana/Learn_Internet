#pragma once 

#include "TcpBase.hh"
#include "Addr.hh"
#include "common.hh"
#include "myeasylog.hpp"

#include <string>
#include <cstdint>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

namespace oldking 
{
	#define FILENAME "Tcpserver"

	#define MAX_STR 1024

	class TcpServer : public TcpBase 
	{
	public:
		// state type
		#define STARTING 1
		#define RUNNING 2

		class Peer;

		// constructer & destructer
		TcpServer(const uint16_t& server_port)
		: addr_in_("0.0.0.0", server_port)
		, state_(STARTING)
		, listen_socket_fd_(-1)
		{
			init();
		}

		~TcpServer()
		{
			clear();
		}

		// API
		
	public:
		bool send(const Peer& peer, const std::string& msg);

		bool recv(const Peer& peer, std::string& buff);

		void listen_peer();

		bool accept_peer(Peer& peer);

	private:
		void init();

		void clear();

		bool clear(const Peer& peer);

	public:
		// Peer
		class Peer 
		{
		public:
			Peer()
			: sockfd_(-1)
			, addr_({})
			{}

			Peer(const int& sockfd, const Addr_in& addr_in)
			: sockfd_(sockfd)
			, addr_(addr_in)
			{}

			Peer(const Peer& other)
			: sockfd_(other.sockfd_)
			, addr_(other.addr_)
			{}

			Peer& operator=(const Peer& other)
			{
				sockfd_ = other.sockfd_;
				addr_ = other.addr_;
				return *this;
			}

			bool operator==(const Peer& other)
			{
				return sockfd_ == other.sockfd_ && 
					   addr_ == other.addr_;
			}

			std::string getIP(){ return addr_.getIP(); }

			std:: int16_t getPort(){ return addr_.getPort(); }

			~Peer()
			{}

		friend bool oldking::TcpServer::send(const Peer& peer, const std::string& msg);
		friend bool oldking::TcpServer::recv(const Peer& peer, std::string& buff);
		friend void oldking::TcpServer::clear();
		friend bool oldking::TcpServer::accept_peer(Peer& peer);

		private:
			int sockfd_;
			Addr_in addr_;
		};

	private:
		Addr_in addr_in_;
		int16_t state_;
		int16_t listen_socket_fd_;
		std::vector<Peer> peer_list_;
	};
}

