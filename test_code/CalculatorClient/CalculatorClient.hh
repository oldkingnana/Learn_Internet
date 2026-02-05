#pragma once

#include "Protocol.hh"
#include "Socket.hh"
#include "common.hh"
#include "myeasylog.hpp"

#include <list>
#include <memory>
#include <string>
#include <sys/wait.h>

namespace oldking 
{
	class CalculatorClient
	{
	public:
		CalculatorClient(ProtocolClient* protocol)
		: protocol_(protocol)
		{}

		~CalculatorClient()
		{
		}

		void run()
		{
			while(true)
			{
				std::string x_str;
				std::string y_str;
				std::string oper_str;
				
				std::cout << "input: " ;

				std::string buff;
				std::getline(std::cin, buff);

				x_str = buff.substr(0, buff.find_first_of(' '));
				buff.erase(0, buff.find_first_of(' ') + 1);
				oper_str = buff.substr(0, buff.find_first_of(' '));
				buff.erase(0, buff.find_first_of(' ') + 1);
				y_str = buff.substr(0, buff.find_first_of(' '));
				buff.erase(0, buff.find_first_of(' ') + 1);

				//std::cout << "x_: " << x_str << "  y_: " << y_str << "  oper_: " << oper_str << std::endl;

				info_.x_ = std::stoi(x_str);
				info_.y_ = std::stoi(y_str);
				info_.oper_ = oper_str[0];
				
				//std::cout << "info_.x_: " << info_.x_ << "  info_.y_: " << info_.y_ << "  info_.oper_: " << info_.oper_ << std::endl;

				protocol_->deliver(info_);

				while(!protocol_->obtain(result_))
				{}
				
				std::cout << "result: " << result_.v_ << std::endl;

				bzero(&info_, sizeof(info_));
				bzero(&result_, sizeof(result_));
				buff_.clear();
			}
		}

	private:
		ProtocolClient* protocol_;
		oldking::Info info_;
		oldking::Result result_;
		std::string buff_;
	};
}

