#ifndef B5A41727_3F14_48C8_A3CF_1D8F2D0E21CD
#define B5A41727_3F14_48C8_A3CF_1D8F2D0E21CD

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "err.h"
#include "lumlib.h"

// TODO: #define LOAD_FACTOR_THRESHOLD 0.7 /* Factor for resizing hash table. */

struct keyval {
    char *key;
    void *value;
    struct keyval *next;
};

struct hashtable {
    // TODO: size_t capacity; /* Capacity of the hash table */
    /* Current size of the hash table */
    size_t size;
    struct keyval **table;
};

struct hashtable *hashtable_init(size_t size);
err_t hashtable_contains(struct hashtable *self, const char *key);
err_t hashtable_insert(struct hashtable *self, const char *key, void *value);
err_t hashtable_get(struct hashtable *self, const char *key, void **mut_value);
err_t hashtable_free(struct hashtable *self);

static struct keyval *keyval_init(const char *key, void *value);
static err_t keyval_free(struct keyval *self);

static unsigned int hash_naive(const char *key, size_t size);
static unsigned int hash_prime(const char *key, size_t size);

// —————————————————————————————————————————————————————————————————————————————————————
// PUBLIC

struct hashtable *hashtable_init(size_t size)
{
    struct hashtable *self = malloc(sizeof(struct hashtable));
    if (self == NULL) {
        fprintf(stderr, "error: %s: in hash table", err_str(ErrNoMem));
        return NULL;
    }

    self->size = size;
    self->table = (struct keyval **)calloc(self->size, sizeof(struct keyval *));
    if (self->table == NULL) {
        free(self);
        fprintf(stderr, "error: %s: for table", err_str(ErrNoMem));
        return NULL;
    }

    return self;
};

/**
 * Insert a key-value pair into the `hashtable`
 */
err_t hashtable_insert(struct hashtable *self, const char *key, void *value)
{
    if (self == NULL || key == NULL)
        return ErrInvalid;

    unsigned int index = hash_prime(key, self->size);

    struct keyval *keyval = keyval_init(key, value);
    if (keyval == NULL) {
        fprintf(stderr, "error: Failed to create key value pair");
        return ErrNoMem;
    }

    /* Handle collisions by chaining (add new item to end of linked list) */
    if (self->table[index] == NULL) {
        self->table[index] = keyval;
    }
    else {
        struct keyval *cur = self->table[index];
        while (cur->next != NULL)
            cur = cur->next;
        cur->next = keyval;
    }

    return ErrOk;
}

/**
 * Find and move key's value into `mut_value` then return ErrOk if successful.
 * Else return ErrNotFound if not, or ErrInvalid for invalid inputs.
 *
 * Usage:
 * #char *retrieved_string;
 * #if (hashtable_get(ht, "my_key", (void **)&retrieved_string) == ErrOk)
 * #####printf("String Value: %d\n", *retrieved_string);
 */
err_t hashtable_get(struct hashtable *self, const char *key, void **mut_value)
{
    if (self == NULL || key == NULL)
        return ErrInvalid;

    unsigned int index = hash_prime(key, self->size);
    struct keyval *cur = self->table[index];

    while (cur != NULL) {
        if (strcmp(cur->key, key) == 0) {
            *mut_value = cur->value;
            return ErrOk;
        }
        cur = cur->next;
    }

    return ErrNotFound;
}

/**
 * Return ErrOk if key exists.
 * Else return ErrNotFound if not, or ErrInvalid for invalid inputs.
 */
err_t hashtable_contains(struct hashtable *self, const char *key)
{
    if (self == NULL || key == NULL)
        return ErrInvalid;

    int index = hash_prime(key, self->size);
    struct keyval *cur = self->table[index];

    /*
     * Check for chaining collisions (multiple keys hashed to same index)
     */
    while (cur != NULL) {
        if (strcmp(cur->key, key) == 0)
            return ErrOk;
        cur = cur->next;
    }

    return ErrNotFound;
}

err_t hashtable_free(struct hashtable *self)
{
    if (self == NULL)
        return ErrInvalid;

    for (size_t i = 0; i < self->size; i++) {
        struct keyval *cur = self->table[i];
        while (cur != NULL) {
            struct keyval *tmp = cur;
            cur = cur->next;
            err_t err = keyval_free(tmp);
            if (err != ErrOk)
                fprintf(stderr, "error: %s: in keyval_free\n", err_str(err));
        }
    }
    if (self->table != NULL)
        free(self->table);
    free(self);

    return ErrOk;
};

// —————————————————————————————————————————————————————————————————————————————————————
// PRIVATE

static struct keyval *keyval_init(const char *key, void *value)
{
    struct keyval *self = (struct keyval *)malloc(sizeof(struct keyval));
    if (self == NULL) {
        fprintf(stderr, "error: %s\n", err_str(ErrNoMem));
        return NULL;
    }

    self->key = strdup(key);
    self->value = value;
    self->next = NULL;

    return self;
}

static err_t keyval_free(struct keyval *self)
{
    if (self == NULL)
        return ErrInvalid;

    if (self->key != NULL)
        free(self->key);
    free(self);

    return ErrOk;
}

/**
 * Hash key into an index by summing of key's ASCII values
 */
static unsigned int hash_naive(const char *key, size_t size)
{
    unsigned int hash = 0;
    for (size_t i = 0; key[i] != '\0'; i++)
        hash += key[i];

    return (hash < 0 ? -hash : hash) % size;
}

/**
 * Hash key into an index with prime number multiplier distribution
 */
static unsigned int hash_prime(const char *key, size_t size)
{
    unsigned int hash = 0;
    for (size_t i = 0; key[i] != '\0'; i++)
        hash = (hash * 31) + key[i];

    return (hash < 0 ? -hash : hash) % size;
}

// —————————————————————————————————————————————————————————————————————————————————————
// TESTS

int test__hash__table()
{
    struct hashtable *ht = hashtable_init(10);
    if (ht == NULL) {
        fprintf(stderr, "Failed to initialize hashtable\n");
        return EXIT_FAILURE;
    }

    int int_value = 5;
    double double_value = 3.14;
    char *string_value = "Hello, World!";

    if (hashtable_insert(ht, "integer", &int_value) != ErrOk
        || hashtable_insert(ht, "double", &double_value) != ErrOk
        || hashtable_insert(ht, "string", string_value) != ErrOk) {
        fprintf(stderr, "Failed to insert key-value pairs\n");
        return EXIT_FAILURE;
    }

    int *retrieved_int;
    double *retrieved_double;
    char *retrieved_string;

    if (hashtable_get(ht, "integer", (void **)&retrieved_int) == ErrOk)
        printf("Integer Value: %d\n", *retrieved_int);

    if (hashtable_get(ht, "double", (void **)&retrieved_double) == ErrOk)
        printf("Double Value: %f\n", *retrieved_double);

    if (hashtable_get(ht, "string", (void **)&retrieved_string) == ErrOk)
        printf("String Value: %s\n", retrieved_string);

    // ...

    if (hashtable_free(ht) != ErrOk) {
        fprintf(stderr, "Failed to free hashtable\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

#endif /* B5A41727_3F14_48C8_A3CF_1D8F2D0E21CD */
