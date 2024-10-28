#include "hash.h"

hash_t hash_adler_32(const void *data, size_t size)
{
    const uint64_t two_in_32_degree = 0xFFFFFFFF;
    uint32_t hash_a = 1;
    uint32_t hash_b = 0;
    for (int i = 0; i < size; i++)
    {
        hash_a += *(uint8_t*)((char*)data + i);
        hash_b += hash_a;
    }
    hash_t hash = hash_b * two_in_32_degree + hash_a;
    return hash;
}


