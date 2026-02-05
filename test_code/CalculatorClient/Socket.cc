#include "Socket.hh"
#include "common.hh"

bool oldking::Socket::operator==(const Socket& other) const 
{
	return sockfd_ == other.sockfd_ && 
		   addr_ == other.addr_;
}

bool oldking::Socket::send(const std::string &msg)
{
	oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, FILENAME_SOCK, "send: " + msg);

	// 不使用write,因为write没办法规避用户关闭fd后写入导致的信号杀死进程的问题
	if(::send(sockfd_, msg.c_str(), msg.length(), MSG_NOSIGNAL) < 0)	
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_WARNING, FILENAME_SOCK, "write err");
		return false;
	}

	return true;
}

bool oldking::Socket::recv(std::string& buff)
{
	char cbuff[MAX_STR];
	int16_t read_len = read(sockfd_, cbuff, MAX_STR - 1);
	
	if(read_len < 0)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_ERROR, FILENAME_SOCK, "read err");
		exit(READ_ERR);	
	}
	else if(read_len == 0)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_WARNING, FILENAME_SOCK, "fd is close!");
		return false;
	}

	cbuff[read_len] = 0;
	buff += cbuff;

	oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, FILENAME_SOCK, "recv: " + buff);
	
	return true;
}

