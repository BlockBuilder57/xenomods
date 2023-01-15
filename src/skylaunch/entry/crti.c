#include "types.h"

// forward declare main
void skylaunch_start();

typedef void (*func_ptr)(void);

extern func_ptr __preinit_array_start__[0], __preinit_array_end__[0];
extern func_ptr __init_array_start__[0], __init_array_end__[0];
extern func_ptr __fini_array_start__[0], __fini_array_end__[0];

void __custom_init(void) {
	for(func_ptr* func = __preinit_array_start__; func != __preinit_array_end__; func++)
		(*func)();

	for(func_ptr* func = __init_array_start__; func != __init_array_end__; func++)
		(*func)();

	// figure out a better way to do this
	skylaunch_start();
}

void __custom_fini(void) {
	for(func_ptr* func = __fini_array_start__; func != __fini_array_end__; func++)
		(*func)();
}