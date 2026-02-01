#pragma once

#include "common.hh"
#include "ProtocolBase.hh"

#include <string>

namespace oldking 
{

	struct Structor 
	{
		int16_t x_;
		int16_t y_;
		char oper_;
	};

	class Protocol : public ProtocolBase<Structor> 
	{
	public:
		Protocol() {}

		~Protocol()
		{}
		
		std::string operator()(Structor s) const override
		{
			return (std::to_string(s.x_) +
				   std::to_string(s.oper_) +
				   std::to_string(s.y_));
		}

		Structor operator()(std::string msg) const override
		{
			Structor s;
			s.oper_ = msg[msg.find_first_of(' ') + 1];
			s.x_ = std::stoi(msg.substr(0, msg.find_first_of(' ')));
			s.y_ = std::stoi(msg.substr(msg.find_last_of(' ') + 1));
			return s;
		}
	};
}
