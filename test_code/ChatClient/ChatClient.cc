#include "ChatClient.hh"
#include "myeasylog.hpp"


void oldking::ChatClient::start()
{
	TP_.SendTask(recv_task_, static_cast<void*>(this));
	
	std::string buff;

	while(true)
	{
		getline(std::cin, buff);
		//std::cin >> buff;
		oldking::MyEasyLog::GetInstance().WriteLog(
				LOG_DEBUG,
				"ChatClient", 
				"buff: " + buff
				);
		send_msg_(buff);	
		buff.clear();	
	}
}
		
void oldking::ChatClient::send_msg_(const std::string& msg)
{
	Udp_->Send(msg);
}

void oldking::ChatClient::recv_msg_()
{
	std::string msg_buf;
	while(true)
	{
		if(Udp_->Recv(msg_buf) == -1)
			return ;
		else 
			std::cout << msg_buf << std::endl;
		msg_buf.clear();
	}
}

void* oldking::ChatClient::recv_task_(void* arg)
{
	auto pself = static_cast<ChatClient*>(arg);

	pself->recv_msg_();

	return nullptr;
}


