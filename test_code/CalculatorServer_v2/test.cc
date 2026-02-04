#include "TcpServer.hpp"
#include "Protocol.hh"
#include "CalculatorServer.hh"
#include "Socket.hh"

#include <functional>
#include <memory>

int main()
{
	oldking::TcpServer server(7777, [](oldking::Socket sock)->void
		{
			oldking::ProtocolServer PS(sock, [](oldking::ProtocolServer* pPS)->void
				{
					oldking::CalculatorServer cal(pPS);
					cal.run();
				});
			PS.run();
		});
	server.run();
	return 0;
}

