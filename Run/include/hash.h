#ifndef HASH
#define HASH

#include <stdint.h>

typedef uint64_t hash_t;

hash_t hash_adler_32(const void *data, size_t size);

#endif
