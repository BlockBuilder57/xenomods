#include "skylaunch/logger/TcpLogger.hpp"

#include <sys/fcntl.h>

#include <bf2mods/Logger.hpp>
#include <optional>
#include <skylaunch/hookng/Hooks.hpp>
#include <vector>

namespace skylaunch::logger {

	struct NnSocketInitalizeHook : hook::Trampoline<NnSocketInitalizeHook> {
		static Result Hook(void* v, ulong ul, ulong ul2, int i1) {
			return 0;
		}
	};

	// event loop stuff
	u32 gListenerSocket;
	std::optional<std::vector<u32>> gClientSockets;
	fd_set closeFds; // set of fds to close

	static void EventLoopThread(void* threadArg) {
		auto* logger = reinterpret_cast<TcpLogger*>(threadArg);
		fd_set readFdSet;

		while (true) {

			FD_ZERO(&readFdSet);
			FD_SET(gListenerSocket, &readFdSet);

			// get the max fd
			auto maxfd = [&]() {
				s32 max = 0;
				for(const auto fd : (*gClientSockets)) {
					FD_SET(fd, &readFdSet);

					if(fd > max)
						max = fd;
				}
				return max;
			};

			// maybe write fds?
			nn::socket::Select(maxfd() + 1, &readFdSet, nullptr, nullptr, nullptr);

			if(FD_ISSET(gListenerSocket, &readFdSet)) {
				// Listener socket is ready to read (new connection)
				// Let's accept it
				u32 addrLen;
				sockaddr_in clientAddr{};
				auto socket = nn::socket::Accept(gListenerSocket, (sockaddr*)&clientAddr, &addrLen);
				if(!(socket & 0x80000000)) {
					(*gClientSockets).push_back(socket);
				}
			}

			// Check if any of our client sockets should be closed
			for(const auto fd : (*gClientSockets)) {
				if(FD_ISSET(fd, &closeFds)) {
					nn::socket::Close(fd);

					// Remove the socket from the list
					std::erase_if(*gClientSockets, [&](auto pred) {
						return pred == fd;
					});

					FD_CLR(fd, &closeFds);
					FD_CLR(fd, &readFdSet); // reading wouldn't be a valid action at this point anyways /shrug
				}
			}
		}
	}


	constexpr size_t poolSize = 0x100000;
	static void* socketPool = memalign(0x4000, poolSize);

	void TcpLogger::Initialize() {
		// Hook nn::socket::Initialize and nn::socket::Finalize to avoid crashes
		// and avoid a game doing funny business with us.
		NnSocketInitalizeHook::HookAt(nn::socket::Initialize);
		hook::StubHook<Result()>::HookAt(nn::socket::Finalize);

		// Initialize nn sockets now.
		NnSocketInitalizeHook::Orig(socketPool, poolSize, 0x20000, 14);

		// Create, bind, and listen for connections
		gListenerSocket = nn::socket::Socket(AF_INET, SOCK_STREAM, 0);
		if(gListenerSocket & 0x80000000)
			return;

		// Set socket options and make the socket non-blocking for our usage of select().
		{
			int flags = 1;
			nn::socket::SetSockOpt(gListenerSocket, SOL_SOCKET, SO_REUSEADDR, &flags, sizeof(flags));
			nn::socket::Fcntl(gListenerSocket, F_SETFL, O_NONBLOCK);
		}

		sockaddr_in serverAddr{};

		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.s_addr = INADDR_ANY;
		serverAddr.sin_port = nn::socket::InetHtons(port);

		int rval = nn::socket::Bind(gListenerSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
		if(rval < 0)
			return;

		rval = nn::socket::Listen(gListenerSocket, 4);
		if(rval < 0)
			return;

		// initialize the client socket vector
		gClientSockets = std::vector<u32>{};

		// start the event loop thread
		const size_t socketThreadStackSize = 0x3000;
		static void* socketThreadStack = memalign(0x1000, socketThreadStackSize);
		auto thread = new nn::os::ThreadType;

		nn::os::CreateThread(thread, EventLoopThread, this, socketThreadStack, socketThreadStackSize, 16, 0);
		nn::os::SetThreadName(thread, "TcpEventLoop");
		nn::os::StartThread(thread);
	}

	void TcpLogger::SendRaw(void* data, size_t size) {
		for(const auto fd : *gClientSockets) {
			if(nn::socket::Send(fd, data, size, 0) == -1) {
				// Request a close by the event loop. To be fair writing should be part of the event loop but
				// I'm too lazy to do it the right way and nn's bsd doesn't abort for sends on differing threads
				// like it does closes. so this will have to do for now
				FD_SET(fd, &closeFds);
			}
		}
	}
}; // namespace skylaunch::logger
