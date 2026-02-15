#include "Socket.hh"
#include "HTTPObj.hpp"
#include "HttpMessageCodec.hh"
#include "TcpServer.hpp"
#include <memory>

int main()
{
	oldking::TcpServer TS(7777, [](std::shared_ptr<oldking::Socket> sock)->void
	{
		oldking::HttpMessageCodec HTTPS(sock, [](oldking::HttpMessageCodec* pHTTPS)->void
		{
			oldking::HTTPReqObj req;
			pHTTPS->obtain(req);

			std::cout << "obj.request line->" << req.method_ + " " << req.req_target_ + " " << req.version_ << "\r\n";
			for(auto it = req.header_.begin(); it != req.header_.end(); it++)
			{
				std::cout << it->first << ": " << it->second << "\r\n" << std::flush;
			}
			std::cout << "\r\n" << std::flush;
			std::cout << req.data_ << std::flush;

			oldking::HTTPResObj res;
			res.version_ = "HTTP/1.0";
			res.res_code_ = "200";
			res.res_state_ = "OK";
			res.header_["Content-Length"] = "5";
			res.data_ = "abcde";

			pHTTPS->deliver(res);
		});
		HTTPS.run();
	});
	TS.run();

	return 0;
}

