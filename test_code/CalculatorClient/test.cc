#include "TcpClient.hpp"
#include "Protocol.hh"
#include "CalculatorClient.hh"
#include "Socket.hh"
#include "myeasylog.hpp"

#include <functional>
#include <memory>

int main()
{
	oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, "test", "exe begin");
	oldking::TcpClient client("127.0.0.1", 7777, [](oldking::Socket&& sock)->void
		{
			oldking::ProtocolClient PC(std::move(sock), [](oldking::ProtocolClient* pPC)->void
				{
					oldking::CalculatorClient cal(pPC);
					cal.run();
				});
			PC.run();
		});
	client.run();
	return 0;
}

