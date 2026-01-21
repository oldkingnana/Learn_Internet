#include "ChatServer.hh"

#include <queue>
#include <utility>

void oldking::ChatServer::start()
{
	// 这里其实并没有做到接收与发送解耦，后续会改一个解耦的版本
	// 顺带一提，消息队列其实是为了解耦而生的
	// 因为如果没有解耦，那其实只需要借用socket自带的队列就行了
	// 换句话说我的主线程既启动和死循环ChatServer服务，又负责recv消息
	while(true)
	{
		get_msg_();

		for(const auto& it : client_map_)
		{
			task_arg* arg = new task_arg
			{
				this,
				waiting_queue_.front().first, 
				it.first,
				it.second
			};

			TP_.SendTask(send_task_, (void*)arg);
		}
	}
}

void oldking::ChatServer::get_msg_()
{
	std::string buff;
	std::string sender_ip;
	int16_t sender_port;
	Udp_->recv(
			buff, 
			sender_ip, 
			sender_port
			);

	if(client_map_.find(sender_ip) == client_map_.end())
		client_map_[sender_ip] = sender_port;	
	else if(client_map_[sender_ip] != sender_port)
		client_map_[sender_ip] = sender_port;	

	waiting_queue_.emplace(std::move(buff), std::move(sender_ip));
}
		
void oldking::ChatServer::send_msg_(const std::string& msg, const std::string& ip, const int16_t port)
{
	Udp_->send(msg, ip, port);		
}

void* oldking::ChatServer::send_task_(void* arg)
{
	auto pinfo = (task_arg*)arg;

	(*pinfo).self->send_msg_(
			(*pinfo).msg,
			(*pinfo).ip,
			(*pinfo).port
			);

	delete pinfo;

	return nullptr;
}


