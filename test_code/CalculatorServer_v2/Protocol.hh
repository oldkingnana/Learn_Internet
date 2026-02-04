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

	class ProtocolBase
	{
	public:
		ProtocolBase() {}
		ProtocolBase(const ProtocolBase& other) = delete;
		ProtocolBase& operator=(const ProtocolBase& other) = delete;
	};

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

	class ProtocolServer : public ProtocolBase
	{
	public:
		ProtocolServer(Socket sock, std::function<void(ProtocolServer*)> func)
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

		void close() { sock_->close(); }

	private:
		std::string serialize(Result result);

		Info deserialize(std::string msg);

	private:
		std::string buff_;
		std::unique_ptr<oldking::Socket> sock_;				
		std::function<void(ProtocolServer*)> func_;
	};

	class ProtocolClient : public ProtocolBase
	{
	public:
		ProtocolClient(Socket sock, std::function<void(ProtocolClient*)> func)
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
		
		void close() { sock_->close(); }

	private:
		std::string serialize(struct Info info);
		
		Result deserialize(std::string msg);
	
	private:
		std::string buff_;
		std::unique_ptr<oldking::Socket> sock_;
		std::function<void(ProtocolClient*)> func_;
	};
}
