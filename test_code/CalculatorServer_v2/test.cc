#include "TcpServer.hpp"
#include "Protocol.hh"
#include "CalculatorServer.hh"
#include "Socket.hh"

#include <functional>
#include <memory>


void func2(std::shared_ptr<oldking::ProtocolServer> protocol)
{
	oldking::CalculatorServer cal(protocol);
}

void func1(oldking::Socket sock)
{
	oldking::ProtocolServer(sock, func2);
}

int main()
{
	oldking::TcpServer(7777, );


}

