#include "main.hpp"

#include <skylaunch/hookng/Hooks.hpp>

#include "../../bf2mods/main.hpp"
#include "skylaunch/utils/ipc.hpp"

// For handling exceptions
char ALIGNA(0x1000) exception_handler_stack[0x4000];
nn::os::UserExceptionInfo exception_info;

void exception_handler(nn::os::UserExceptionInfo* info) {
	skylaunch::logger::s_Instance->LogFormat("Exception occurred!\n");

	skylaunch::logger::s_Instance->LogFormat("Error description: %x\n", info->ErrorDescription);
	for(int i = 0; i < 29; i++)
		skylaunch::logger::s_Instance->LogFormat("X[%02i]: %" PRIx64 "\n", i, info->CpuRegisters[i].x);
	skylaunch::logger::s_Instance->LogFormat("FP: %" PRIx64 "\n", info->FP.x);
	skylaunch::logger::s_Instance->LogFormat("LR: %" PRIx64 "\n", info->LR.x);
	skylaunch::logger::s_Instance->LogFormat("SP: %" PRIx64 "\n", info->SP.x);
	skylaunch::logger::s_Instance->LogFormat("PC: %" PRIx64 "\n", info->PC.x);
}


struct RomMountedHook : skylaunch::hook::Trampoline<RomMountedHook> {
	static Result Hook(const char* path, void* buffer, ulong size) {
		auto res = Orig(path, buffer, size);

		// bring up the rest
		bf2mods::main();

		return res;
	}
};

void skylaunch_main() {
	// populate our own process handle
	Handle h;
	skylaunch::utils::Ipc::getOwnProcessHandle(&h);
	envSetOwnProcessHandle(h);

	// init hooking setup
	A64HookInit();

	// Initialize RO before the game has a chance to, then
	// hook it so the game can't even try
	nn::ro::Initialize();
	skylaunch::hook::StubHook<Result()>::HookAt(nn::ro::Initialize);

	// initialize logger
	skylaunch::logger::s_Instance = new skylaunch::logger::TcpLogger();
	skylaunch::logger::s_Instance->Log("[skylaunch] Beginning initialization.\n");
	skylaunch::logger::s_Instance->StartThread();

	// override exception handler to dump info
	nn::os::SetUserExceptionHandler(exception_handler, exception_handler_stack, sizeof(exception_handler_stack),
									&exception_info);

	// Hook nn::fs::MountRom to start executing the rest as soon as the ROM is mounted.
	RomMountedHook::HookAt(nn::fs::MountRom);
}

extern "C" void skylaunch_start() {
	skylaunch::utils::init();
	virtmemSetup(); // needed for libnx JIT

	skylaunch_main();
}
