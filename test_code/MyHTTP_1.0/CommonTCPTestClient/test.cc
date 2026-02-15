#include "TcpClient.hpp"
#include "Socket.hh"
#include "myeasylog.hpp"

#include <functional>
#include <memory>
#include <unistd.h>

int main()
{
	oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, "test", "exe begin");
	oldking::TcpClient client("127.0.0.1", 7777, [](std::shared_ptr<oldking::Socket> sock)->void
		{
			sock->send("POST /api/test HTTP/1.1\r\nHost: localhost:8080\r\nUser-Agent: TestClient/1.0\r\nContent-Type: application/json\r\nContent-Length: 27\r\nConnection: keep-alive\r\n\r\n{\"name\":\"alice\",\"age\":18}");
			std::string buff;
		//	for(int n = 0; n < 30; n++)
		//	{
		//		sock->recv(buff);
		//		std::cout << buff;
		//		usleep(100);
		//	}
			sock->recv(buff);
			std::cout << buff << std::flush;
		});
	client.run();
	return 0;
}

