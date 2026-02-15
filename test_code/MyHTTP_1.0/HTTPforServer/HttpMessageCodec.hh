#pragma once

#include "common.hh"
#include "Socket.hh"
#include "myeasylog.hpp"
#include "HTTPObj.hpp"

#include <memory>
#include <string>
#include <functional>
#include <unordered_map>

namespace oldking
{
	class ProtocolBase
	{
	public:
		ProtocolBase() {}
		ProtocolBase(const ProtocolBase& other) = delete;
		ProtocolBase& operator=(const ProtocolBase& other) = delete;
	};

	class HttpMessageCodec : public ProtocolBase
	{
	public:
		HttpMessageCodec(std::shared_ptr<Socket> sock, std::function<void(HttpMessageCodec*)> func)
		: buff_({})
		, sock_(sock)
		, func_(func)
		{}

		~HttpMessageCodec()
		{}

		// ---------------public API---------------
		void run();

		bool obtain(struct HTTPReqObj& obj);
		
		bool deliver(HTTPResObj obj);

	private:
		// ------serialize & deserialize API-------
		std::string serialize(HTTPResObj obj);

		void deserialize_req_line(std::string req_line, HTTPReqObj& obj);
		void deserialize_header(std::string header, HTTPReqObj& obj);	
		void deserialize_data(std::string buff, uint64_t length, HTTPReqObj& obj);

		// --------------about detail--------------
		std::string get_method_(std::string& msg);

		std::string get_req_target_(std::string& msg);
		
		std::string get_version_(std::string& msg);
		
		std::unordered_map<std::string, std::string> get_header_(std::string& msg);

	private:
		std::string buff_;
		std::shared_ptr<oldking::Socket> sock_;				
		std::function<void(HttpMessageCodec*)> func_;
	};
}
