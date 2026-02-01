#pragma once

#include "common.hh"
#include <string>

namespace oldking
{
	template<typename structor>
	class ProtocolBase
	{
	public:
		ProtocolBase()
		{}
		
		~ProtocolBase()
		{}

		virtual std::string operator()(structor s) const { (void)s; return {}; }

		virtual structor operator()(std::string msg) const { (void)msg; return {}; }
	};
}

