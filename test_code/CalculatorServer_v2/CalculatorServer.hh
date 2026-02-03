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
	class CalculatorServer
	{
	public:
		CalculatorServer(std::shared_ptr<ProtocolServer> protocol)
		: protocol_(protocol)
		{}

		~CalculatorServer()
		{}

		void run()
		{
			while(true)
			{
				if(protocol_->obtain(info_))
				{
					switch (info_.oper_) 
					{
					case '+':
						result_.v_ = info_.x_ + info_.y_;
						break;
					case '-':
						result_.v_ = info_.x_ - info_.y_;
						break;
					case '*':
						result_.v_ = (info_.x_ * info_.y_);
						break;
					case '/':
						result_.v_ = (info_.x_ / info_.y_);
						break;
					case '%':
						result_.v_ = (info_.x_ % info_.y_);
						break;
					default:
						result_.v_ = static_cast<int16_t>(114514);
						break;
					}
				}
			}
		}

	private:
		std::shared_ptr<ProtocolServer> protocol_;
		oldking::Info info_;
		oldking::Result result_;
	};
}

