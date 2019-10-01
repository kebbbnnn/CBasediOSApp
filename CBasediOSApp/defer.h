//
//  defer.h
//  CBasediOSApp
//
//  Created by Kevin Ladan on 25/9/19.
//  Copyright © 2019 Ultimate Computer Services, Inc. All rights reserved.
//

/*
 * Go-style defer scoped implementation
 *
 * When compiled with clang, use: -fblocks -lBlocksRuntime
 *
 * Example of use:
 *
 * {
 *   int fd = open(fname, O_RDONLY);
 *   if (fd == -1) {
 *     error(....);
 *     return;
 *   }
 *   defer { close(fd); };
 *   ssize_t sz = read(fd, buf, sizeof(buf));
 *   ...
 *   ...
 * }
 *
 */

#define __STRMERGE(a, b) a##b
#define _STRMERGE(a, b) __STRMERGE(a, b)
#ifdef __clang__
#if __has_extension(blocks)
static void __attribute__((unused)) __clang_cleanup_func(void (^*dfunc)(void)) {
  (*dfunc)();
}

#define defer                                        \
void (^_STRMERGE(__defer_f_, __COUNTER__))(void) \
__attribute__((cleanup(__clang_cleanup_func))) __attribute__((unused)) = ^

#else /* __has_extension(blocks) */
#define defer UNIMPLEMENTED - NO - SUPPORT - FOR - BLOCKS - IN - YOUR - CLANG - ENABLED
#endif /*  __has_extension(blocks) */
#else  /* !__clang__, e.g.: gcc */

#define __block
#define _DEFER(a, count)                                                                      \
auto void _STRMERGE(__defer_f_, count)(void* _defer_arg __attribute__((unused)));         \
int _STRMERGE(__defer_var_, count) __attribute__((cleanup(_STRMERGE(__defer_f_, count)))) \
__attribute__((unused));                                                              \
void _STRMERGE(__defer_f_, count)(void* _defer_arg __attribute__((unused)))
#define defer _DEFER(a, __COUNTER__)
#endif /* ifdef __clang__ */

/* Block scoped mutexes */
#define MX_SCOPED_LOCK(m) \
MX_LOCK(m);           \
defer {               \
MX_UNLOCK(m);     \
}

#define MX_SCOPED_RWLOCK_READ(m) \
MX_RWLOCK_READ(m);           \
defer {                      \
MX_RWLOCK_UNLOCK(m);     \
}
#define MX_SCOPED_RWLOCK_WRITE(m) \
MX_RWLOCK_WRITE(m);           \
defer {                       \
MX_RWLOCK_UNLOCK(m);      \
}
