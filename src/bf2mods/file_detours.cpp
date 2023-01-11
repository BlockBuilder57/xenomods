//
// Created by block on 1/9/2023.
//

#include "file_detours.hpp"

#include <bf2mods/engine/ml/devfile.hpp>
#include <bf2mods/engine/mm/fixstr.hpp>

#include "bf2logger.hpp"
#include "bf2mods/debug_wrappers.hpp"
#include "bf2mods/stuff/utils/debug_util.hpp"
#include "bf2mods/stuff/utils/util.hpp"
#include "bf2mods/utils.hpp"
#include "plugin_main.hpp"

#include <filesystem>

namespace sfs = std::filesystem;

namespace ml {

	GENERATE_SYM_HOOK(DrMdlMan_createMdl, "_ZN2ml8DrMdlMan9createMdlEPKvPKci", void*, void* this_pointer, char* model_buffer, char* model_path, int unknown) {
		bf2mods::g_Logger->LogInfo("DrMdlMan::createMdl \"{}\"", model_path);
		return DrMdlMan_createMdlBak(this_pointer, model_buffer, model_path, unknown);
	}

	GENERATE_SYM_HOOK(DrMdlMan_createInstantMdl, "_ZN2ml8DrMdlMan16createInstantMdlEPKvPKci", void*, void* this_pointer, char* model_buffer, char* model_path, int unknown) {
		bf2mods::g_Logger->LogInfo("DrMdlMan::createInstantMdl \"{}\"", model_path);
		return DrMdlMan_createInstantMdlBak(this_pointer, model_buffer, model_path, unknown);
	}

	GENERATE_SYM_HOOK(DrCalcTexStreamMan_createTexStmListObj, "_ZN2ml18DrCalcTexStreamMan19createTexStmListObjEPKcPKh", void*, void* this_pointer, char* path, char* buffer) {
		path[0] = 0; // trying this for the funny
		// this just makes all normal models invisible, boo
		return DrCalcTexStreamMan_createTexStmListObjBak(this_pointer, path, buffer);
	}

	GENERATE_SYM_HOOK(DevFileTh_readFile, "_ZN2ml9DevFileTh8readFileERKNS_14DevFileParamThE", void*, DevFileParamTh* fileParam) {
		if(fileParam->readBuffer != nullptr && fileParam->RequestedFilename) {
			auto filename = bf2mods::GetStringView(*fileParam->RequestedFilename);


			if(filename.ends_with("pc000101.wismt"))
			{
				if(fileParam->readBuffer != nullptr) {
					bf2mods::g_Logger->LogDebug("readFile: \"{}\" (start offset 0x{:08x} size 0x{:08x})", filename, fileParam->readStartOffset, fileParam->readSize);
				}
				//dbgutil::logStackTrace();

				//auto ptr = std::bit_cast<std::uint8_t*>(fileParam);
				//auto str = std::ostringstream {};
				//for(auto i = 0u; i < 0x70; ++i) {
				//	str << fmt::format("{:02x} ", ptr[i]);
				//}

				//skyline::logger::s_Instance->LogFormat("hexdump: %s", str.str().c_str());



				//bf2mods::g_Logger->LogDebug("hey apple its {}{}, (buffer {})", reinterpret_cast<char*>(fileParam->readBuffer)[0],  reinterpret_cast<char*>(fileParam->readBuffer)[1], fileParam->readBuffer);

#if 1
				// dump data read requests to sd card

				auto mutable_filename = std::string(fileParam->RequestedFilename->buffer);

				auto replace_all = [](std::string& inout, std::string_view what, std::string_view with) {
					std::size_t count{};
					for (std::string::size_type pos{};
						inout.npos != (pos = inout.find(what.data(), pos, what.length()));
						pos += with.length(), ++count) {
						inout.replace(pos, what.length(), with.data(), with.length());
					}
					return count;
				};

				replace_all(mutable_filename, "/", "_");
				replace_all(mutable_filename, ":", "_");

				auto path = fmt::format("sd:/config/bf2mods/dump/{}_{:08x}_{:08x}.bin", mutable_filename, fileParam->readStartOffset, fileParam->readSize);

				// i hope this works!

				Result res{};
				nn::fs::FileHandle fh{};

				res = nn::fs::CreateFile(path.c_str(), fileParam->readSize);

				if(R_FAILED(res) && res != 0x00000402 /* path already exists */) {
					bf2mods::g_Logger->LogError("couldnt create dump file \"{}\": 0x{:08x}; give up here, i must", path, res);
					//return squeef;
				}

				res = nn::fs::OpenFile(&fh, path.c_str(), nn::fs::OpenMode_ReadWrite);

				if(R_FAILED(res)) {
					bf2mods::g_Logger->LogError("couldnt open dump file \"{}\": 0x{:08x}", path, res);
				} else {
					res = nn::fs::WriteFile(fh, 0, fileParam->readBuffer, fileParam->readSize, {});

					if(R_FAILED(res)) {
						bf2mods::g_Logger->LogError("couldnt write dump of file read request to \"{}\": 0x{:08x}", path, res);
					} else {
						// force a flush since the write worked, i think
						nn::fs::FlushFile(fh);
						//bf2mods::g_Logger->LogDebug("wrote dump of file read request to \"{}\"", path);
					}


					nn::fs::CloseFile(fh);
				}
#endif

				void* squeef = DevFileTh_readFileBak(fileParam);

				return squeef;
			}
			else return DevFileTh_readFileBak(fileParam);

		}
		else
			return DevFileTh_readFileBak(fileParam);
	}

} // namespace ml

namespace bf2mods {

	void FileDetours::Initialize() {
		g_Logger->LogDebug("Setting up file detours...");

		//ml::DrMdlMan_createMdlHook();
		//ml::DrMdlMan_createInstantMdlHook();
		//ml::DrCalcTexStreamMan_createTexStmListObjHook();

		ml::DevFileTh_readFileHook();
	}

	BF2MODS_REGISTER_MODULE(FileDetours);

} // namespace bf2mods
