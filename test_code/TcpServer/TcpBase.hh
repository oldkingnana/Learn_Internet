#pragma once 


namespace oldking
{
	class TcpBase 
	{
	public:
		TcpBase()
		{}

		TcpBase(TcpBase& other) = delete;
		TcpBase(TcpBase&& other) = delete;

		TcpBase& operator=(TcpBase& other) = delete;

		~TcpBase()
		{}
	};
}









