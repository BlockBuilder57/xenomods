#include "skylaunch/logger/TcpLogger.hpp"

#include "skylaunch/utils/cpputils.hpp"

#include <skylaunch/hookng/Hooks.hpp>

#define PORT 6969

namespace skylaunch::logger {
	int g_tcpSocket;
	bool g_loggerInit = false;

	struct NnSocketInitalizeHook : hook::Trampoline<NnSocketInitalizeHook> {
		static Result Hook(void* v, ulong ul, ulong ul2, int i1) {
			return 0;
		}
	};

	void TcpLogger::Initialize() {
		const size_t poolSize = 0x100000;
		void* socketPool = memalign(0x4000, poolSize);

		// Hook nn::socket::Initialize and nn::socket::Finalize to avoid crashes
		// and avoid a game doing funny business with us.
		NnSocketInitalizeHook::HookAt(nn::socket::Initialize);
		hook::StubHook<Result()>::HookAt(nn::socket::Finalize);

		// Initalize nn sockets now.
		NnSocketInitalizeHook::Orig(socketPool, poolSize, 0x20000, 14);

		sockaddr_in serverAddr{};
		g_tcpSocket = nn::socket::Socket(AF_INET, SOCK_STREAM, 0);
		if(g_tcpSocket & 0x80000000)
			return;

		int flags = 1;
		nn::socket::SetSockOpt(g_tcpSocket, SOL_SOCKET, SO_KEEPALIVE, &flags, sizeof(flags));

		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.s_addr = INADDR_ANY;
		serverAddr.sin_port = nn::socket::InetHtons(PORT);

		int rval = nn::socket::Bind(g_tcpSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
		if(rval < 0)
			return;

		rval = nn::socket::Listen(g_tcpSocket, 1);
		if(rval < 0)
			return;

		u32 addrLen;
		g_tcpSocket = nn::socket::Accept(g_tcpSocket, (struct sockaddr*)&serverAddr, &addrLen);

		g_loggerInit = true;
	}

	void TcpLogger::SendRaw(void* data, size_t size) {
		nn::socket::Send(g_tcpSocket, data, size, 0);
	}
}; // namespace skylaunch::logger
