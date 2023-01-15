#include "main.hpp"

#include "../../bf2mods/plugin_main.hpp"
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


void stub() {
}


/**
 struct RomMountedHook : Trampoline<RomMountedHook> {
 	static Result Hook(const char* path, void* buf, size_t size) {
 		auto res = Orig(path, buffer, size);

 		// bring up the rest
 		bf2mods::main();

 		return res;
 	}
 }
 */

Result (*nnFsMountRomImpl)(char const*, void*, unsigned long);

Result handleNnFsMountRom(char const* path, void* buffer, unsigned long size) {
	Result rc = nnFsMountRomImpl(path, buffer, size);
	skylaunch::logger::s_Instance->LogFormat("[handleNnFsMountRom] Mounted ROM (0x%x)", rc);
#if 0
	skylaunch::logger::s_Instance->LogFormat("text: 0x%" PRIx64 " | rodata: 0x%" PRIx64
										   " | data: 0x%" PRIx64 " | bss: 0x%" PRIx64 " | heap: 0x%" PRIx64,
										   skylaunch::utils::g_MainTextAddr, skylaunch::utils::g_MainRodataAddr,
										   skylaunch::utils::g_MainDataAddr, skylaunch::utils::g_MainBssAddr,
										   skylaunch::utils::g_MainHeapAddr);
#endif
	// Setup bf2mods code.
	bf2mods::bf2mods_main();
	return rc;
}

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
	A64HookFunction(reinterpret_cast<void*>(nn::ro::Initialize), reinterpret_cast<void*>(stub), nullptr);

	// initialize logger
	skylaunch::logger::s_Instance = new skylaunch::logger::TcpLogger();
	skylaunch::logger::s_Instance->Log("[skylaunch] Beginning initialization.\n");
	skylaunch::logger::s_Instance->StartThread();

	// override exception handler to dump info
	nn::os::SetUserExceptionHandler(exception_handler, exception_handler_stack, sizeof(exception_handler_stack),
									&exception_info);

	// Hook nn::fs::MountRom to wait before initializing the rest of our code
	A64HookFunction(reinterpret_cast<void*>(nn::fs::MountRom), reinterpret_cast<void*>(handleNnFsMountRom),
					(void**)&nnFsMountRomImpl);
}

extern "C" void skylaunch_start() {
	skylaunch::utils::init();
	virtmemSetup(); // needed for libnx JIT

	skylaunch_main();
}
