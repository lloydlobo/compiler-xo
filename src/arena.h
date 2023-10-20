#include <stddef.h>
#include <stdlib.h>

// Taken from Odin related tb tilde backend, an alternative to LLVM
// See src/tilde/tb_arena.h
enum {
        LUM_ARENA_SMALL_CHUNK_SIZE = 4 * 1024,
        LUM_ARENA_MEDIUM_CHUNK_SIZE = 512 * 1024,
        LUM_ARENA_LARGE_CHUNK_SIZE = 16 * 1024 * 1024,

        LUM_ARENA_ALIGNMENT = 16,
};

struct ArenaAllocator {
        size_t m_size;
        unsigned char *m_buffer;
        unsigned char *m_offset;
};

struct ArenaAllocator *create_ArenaAllocator(size_t bytes);

void destroy_ArenaAllocator(struct ArenaAllocator *allocator);

// Automatically determine the size of allocation
//
// NOTE: consider using sizeof_field macro defined in main to calculate size, as
// we do not have templates
void *alloc(struct ArenaAllocator *allocator, size_t size);
