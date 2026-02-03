#pragma once

#include "common.hh"
#include "Socket.hh"
#include "myeasylog.hpp"

#include <memory>
#include <string>
#include <functional>

#include <jsoncpp/json/value.h>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>

namespace oldking
{
	// <len>\r\n<functional part>\r\n

	struct Info 
	{
		int16_t x_;
		int16_t y_;
		char oper_;
	};

	struct Result 
	{
		int16_t v_;
	};

	class ProtocolServer
	{
	public:
		ProtocolServer(Socket sock, std::function<void(std::shared_ptr<ProtocolServer>)> func)
		: buff_({})
		, sock_(std::make_unique<oldking::Socket>(sock))
		, func_(func)
		{}

		~ProtocolServer()
		{
			sock_->close();
		}

		void run();

		bool obtain(struct Info& info);
		
		bool deliver(Result result);

	private:
		std::string serialize(Result result);

		Info deserialize(std::string msg);

	private:
		std::string buff_;
		std::unique_ptr<oldking::Socket> sock_;				
		std::function<void(std::shared_ptr<ProtocolServer>)> func_;
	};

	class ProtocolClient
	{
	public:
		ProtocolClient(Socket sock, std::function<void(std::shared_ptr<ProtocolClient>)> func)
		: buff_({})
		, sock_(std::make_unique<oldking::Socket>(sock))
		, func_(func)
		{}

		~ProtocolClient()
		{
			sock_->close();
		}
	
		void run();

		bool obtain(struct Result& result);

		bool deliver(struct Info info);

	private:
		std::string serialize(struct Info info);
		
		Result deserialize(std::string msg);
	
	private:
		std::string buff_;
		std::unique_ptr<oldking::Socket> sock_;
		std::function<void(std::shared_ptr<ProtocolClient>)> func_;
	};
}
