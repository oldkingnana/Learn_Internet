#pragma once

namespace oldking
{
	class UdpBase
	{
	public: 
		UdpBase()
		{}

		~UdpBase()
		{}

		UdpBase(const UdpBase& other) = delete;

		UdpBase operator=(const UdpBase& other) = delete;
		
		UdpBase(UdpBase&& other) = delete;
		
		UdpBase operator=(UdpBase&& other) = delete;
	};
}








