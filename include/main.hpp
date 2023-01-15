#pragma once

#include <map>
#include <string>

#include "ModuleObject.hpp"
#include "nn/crypto.h"
#include "nn/fs.h"
#include "nn/nn.h"
#include "nn/oe.h"
#include "nn/os.hpp"
#include "nn/prepo.h"
#include "nn/ro.h"
#include "operator.h"
#include "skylaunch/inlinehook/And64InlineHook.hpp"
#include "skylaunch/logger/TcpLogger.hpp"
#include "skylaunch/utils/cpputils.hpp"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "skylaunch/nx/kernel/virtmem.h"
#include "skylaunch/nx/runtime/env.h"

#ifdef __cplusplus
};
#endif


extern "C" void skylaunch_init();