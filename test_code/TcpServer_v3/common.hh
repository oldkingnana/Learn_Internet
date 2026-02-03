#pragma once

#include <string>

enum ERRCODE 
{
	OK = 0,
	ADDR_ERR,
	BIND_ERR,
	SOCK_ERR,
	LISTEN_ERR,
	READ_ERR,
	FORK_ERR
};

#define MAX_STR 1024

#define FILENAME_SOCK "Socket"
#define FILENAME_TCPSERVER "TcpServer"
#define FILENAME_PROTOCOL "PROTOCOL"


