#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __is_libk

#include <kernel/panic.h>

#endif

// TODO: Make this value randomized
#define STACK_CHK_GUARD 0x595e9fbd94fda766

uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

/**
 * Basic stack smashing protector implementation
 * Based on https://wiki.osdev.org/Stack_Smashing_Protector
 */
void __stack_chk_fail(void)
{
#ifdef __is_libk
  PANIC("Stack Smashing Detected");
#else
  printf("Stack Smashing Detected (TODO: abort() impl)\n");
#endif
}
