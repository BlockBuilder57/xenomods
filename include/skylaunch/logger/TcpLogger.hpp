
#pragma once

#include <arpa/inet.h>

#include "alloc.h"
#include "mem.h"
#include "nn/socket.h"
#include "nn/time.h"
#include "operator.h"
#include "skylaunch/logger/Logger.hpp"

namespace skylaunch::logger {
	class TcpLogger : public Logger {
		u16 port;
	   public:

		explicit TcpLogger(u16 port)
			: port(port) {

		}

		void Initialize() override;
		void SendRaw(void*, size_t) override;
		std::string FriendlyName() override {
			return "TcpLogger";
		}
	};
}; // namespace skylaunch::logger