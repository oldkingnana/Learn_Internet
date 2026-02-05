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

	class ProtocolClient : public ProtocolBase
	{
	public:
		ProtocolClient(Socket&& sock, std::function<void(ProtocolClient*)> func)
		: buff_({})
		, sock_(std::move(sock))
		, func_(func)
		{}

		~ProtocolClient()
		{
		}
	
		void run();

		bool obtain(struct Result& result);

		bool deliver(struct Info info);

	private:
		std::string serialize(struct Info info);
		
		Result deserialize(std::string msg);
	
	private:
		std::string buff_;
		oldking::Socket sock_;
		std::function<void(ProtocolClient*)> func_;
	};
}
