#include "HttpRouter.hh"
#include "Socket.hh"
#include "HTTPObj.hpp"
#include "TcpServer.hpp"
#include "Util.hpp"
#include "common.hpp"
#include "HttpRouter.hh"

#include <memory>
#include <string>
#include <iostream>

int main()
{
	try 
	{
		std::unique_ptr<oldking::TcpServer> TS(std::make_unique<oldking::TcpServer>(7777, [](std::shared_ptr<oldking::Socket> sock)->void
		{
			oldking::HttpRouter HTTPS(sock);
			HTTPS.run();
		}));
		TS->run();
	} 
	catch (const std::invalid_argument& ex) 
	{
		std::cout << std::string(ex.what()) << std::endl;
		exit(-1);
	} 
	catch (const std::out_of_range& ex)
	{
		std::cout << std::string(ex.what()) << std::endl;
		exit(-1);
	}
	catch (...)
	{
		std::cout << "unkown err" << std::endl;
		exit(-1);
	}

	return 0;
}

