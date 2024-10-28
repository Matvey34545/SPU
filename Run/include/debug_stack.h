#ifndef DEBUG_STACK
#define DEBUG_STACK

#include "config.h"
#include <assert.h>

#ifdef NO_CANARY
    #define FOR_CANARY(...)

#else
    #define FOR_CANARY(...) __VA_ARGS__

#endif

#ifdef NO_HASH
    #define FOR_HASH(...)

#else
    #define FOR_HASH(...) __VA_ARGS__

#endif

#define CHECK_HASH(st)                                                      \
    do {                                                                    \
    hash_t temp_hash = st->hash_struct;                                     \
    st->hash_struct = 0;                                                    \
    if (hash_adler_32(st, sizeof(stack_t)) != temp_hash)                    \
        assert(0);                                                          \
                                                                            \
    if (st->hash_stack != hash_adler_32(st->data, st->capacity))            \
        assert(0);                                                          \
                                                                            \
    } while(0)


#ifdef NO_DEBUG
    #define    FOR_DEBUG(...)
    #define CHECK_STACK(st, init)                                          \
    do {                                                                   \
        if (stack_error(st) != OK)                                         \
            assert(0);                                                     \
                                                                           \
    } while(0)

#else
    #define    FOR_DEBUG(...) __VA_ARGS__
    #define __INIT__ Init {__LINE__, __FILE__, __func__}

    #define CHECK_STACK(st, init_call)                                         \
    do{                                                                        \
        if (stack_error(st) != OK)                                             \
        {                                                                      \
            dump(stack_err_error(stack_error(st)), st, init_call);             \
            assert(1);                                                         \
        }                                                                      \
    } while(0)

#endif


#endif
