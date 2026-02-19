#include "Socket.hh"
#include "HTTPObj.hpp"
#include "HttpMessageCodec.hh"
#include "TcpServer.hpp"
#include "Util.hpp"
#include "common.hpp"

#include <memory>
#include <string>

int main()
{
	oldking::TcpServer TS(7777, [](std::shared_ptr<oldking::Socket> sock)->void
	{
		oldking::HttpMessageCodec HTTPS(sock, [](oldking::HttpMessageCodec* pHTTPS)->void
		{
			oldking::HTTPReqObj req;
			pHTTPS->obtain(req);

		//	std::cout << "obj.request line->" << req.method_ + " " << req.req_target_ + " " << req.version_ << "\r\n";
		//	for(auto it = req.header_.begin(); it != req.header_.end(); it++)
		//	{
		//		std::cout << it->first << ": " << it->second << "\r\n" << std::flush;
		//	}
		//	std::cout << "\r\n" << std::flush;
		//	std::cout << req.data_ << std::flush;

			std::cout << 
			"get a request message" << std::endl << 
			std::endl << 
			"request line:" << std::endl << 
			"method: " << req.method_ << std::endl << 
			"path: " << req.req_target_ << std::endl << 
			"version: " << req.version_ << std::endl << 
			std::endl << 
			"header:" << std::endl;
			for(auto it : req.header_)
			{
				std::cout << it.first << ": " << it.second << std::endl;
			}
			std::cout << std::endl;
			std::cout << "data: " << std::endl << 
			req.data_;
			
			// Message parsing

			oldking::HTTPResObj res;
			// check path 
			if(req.req_target_ == "/")
			{
				if(oldking::Util::ReadFile(WEBROOT + std::string("/index.html"), res.data_))
				{
					res.header_["Content-Type"] = "text/html; charset=UTF-8";
					res.res_code_ = "200";
					res.res_state_ = "OK";
				}
				else 
				{
					// err
					res.res_code_ = "404";
					res.res_state_ = "Not Found";
				}
			}
			else 
			{
				std::string exten;
				if(oldking::Util::GetFileExten(req.req_target_, exten))
				{
					if(!oldking::Util::ReadFile(WEBROOT + req.req_target_, res.data_))
					{
						res.header_["Content-Type"] = mime_map[exten];
						res.res_code_ = "200";
						res.res_state_ = "OK";
					}
					else 
					{
						// err
						res.res_code_ = "404";
						res.res_state_ = "Not Found";
					}
				}
				else 
				{
					// err 
					res.res_code_ = "404";
					res.res_state_ = "Not Found";
				}
			}
			res.version_ = "HTTP/1.1";
			res.header_["Connection"] = "close";
 
			res.header_["Content-Length"] = std::to_string(res.data_.length());

			std::cout << "send response message: " << std::endl << 
			"response line:" << std::endl << 	
			"version: " << res.version_ << std::endl << 
			"code: " << res.res_code_ << std::endl << 
			"state: " << res.res_state_ << std::endl << 
			std::endl;

			std::cout << "headers: " << std::endl;
			for(auto it : res.header_)
			{
				std::cout << it.first << ": " << it.second << std::endl;
			}
			std::cout << std::endl;

			std::cout << "data: " << std::endl << 
			res.data_;

			pHTTPS->deliver(res);
		});
		HTTPS.run();
	});
	TS.run();

	return 0;
}

