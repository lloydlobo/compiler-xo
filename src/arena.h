#include <stddef.h>
#include <stdlib.h>

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
