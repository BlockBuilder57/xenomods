/**
 * @file cxa.h
 * @brief C++ ABI header. Shouldn't be here. Use llvm libc++abi or something from llvm4.0.1.
 */

#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

//extern s32 __cxa_guard_acquire(u32* guard);
//extern void __cxa_guard_release(u32* guard);
extern void __cxa_pure_virtual();
void __cxa_atexit();

#ifdef __cplusplus
}
#endif