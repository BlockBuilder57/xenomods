// Copyright 2017 plutoo
#include "skylaunch/nx/kernel/mutex.h"

#include "../internal.h"
#include "skylaunch/nx/kernel/svc.h"
#include "skylaunch/nx/runtime/env.h"

#define HAS_LISTENERS 0x40000000

void mutexLock(Mutex* m) { nnosLockMutex(m); }

bool mutexTryLock(Mutex* m) { return nnosTryLockMutex(m); }

void mutexUnlock(Mutex* m) { nnosUnlockMutex(m); }

void rmutexLock(RMutex* m) { mutexLock(m); }

bool rmutexTryLock(RMutex* m) { return mutexTryLock(m); }

void rmutexUnlock(RMutex* m) { mutexUnlock(m); }