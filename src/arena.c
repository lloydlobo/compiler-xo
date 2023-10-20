#include "arena.h"

struct ArenaAllocator *create_ArenaAllocator(size_t bytes) {
        struct ArenaAllocator *self
            = (struct ArenaAllocator *)malloc(sizeof(struct ArenaAllocator));
        if (self == NULL) {
                return NULL; // Failed to allocate memory for the allocator
        }

        self->m_size = bytes;
        self->m_buffer = (unsigned char *)malloc(bytes);
        if (self->m_buffer == NULL) {
                free(self);
                return NULL; // Failed to allocate memory for the buffer
        }

        self->m_offset = self->m_buffer; // starts where buffer starts
        return self;
}

void destroy_ArenaAllocator(struct ArenaAllocator *self) {
        if (self != NULL) {
                free(self->m_buffer);
                free(self);
        }
}

void *alloc(struct ArenaAllocator *self, size_t size) {
        if (self == NULL) {
                return NULL;
        }

        void *offset = self->m_offset; // starts where buffer starts
        self->m_offset += size;
        return offset;
}
