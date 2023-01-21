#pragma once

/*

--- CVEC2 ---
a single-header vector library
written in pure C

*/

#include <stddef.h>
#include <stdint.h>

struct cvec2_allocator_set {
    void *(*alloc)(size_t size);
    void (*free)(void *ptr);
    void *(*realloc)(void *ptr, size_t new_size);
};

struct cvec2 {
    uint8_t *ptr;
    size_t len;
    size_t cap;
    size_t elem_size;

    struct cvec2_allocator_set allocator_set;
};

#undef CVEC2_EXPORT
#ifdef CVEC2_INLINE
#define CVEC2_EXPORT static inline
#else
#define CVEC2_EXPORT extern
#endif

#ifndef CVEC2_DEFAULT_CAP
#define CVEC2_DEFAULT_CAP (sizeof(size_t) * 8)
#endif

#ifndef CVEC2_DEFAULT_GROWTH
#define CVEC2_DEFAULT_GROWTH 2
#endif

/* -- CONSTRUCTORS -- */

// initialize a vector, using the standard library allocator
CVEC2_EXPORT struct cvec2 cvec2_init_default(size_t member_size);

// initialize a vector, using a custom set of allocator functions
CVEC2_EXPORT struct cvec2 cvec2_init_alloc(struct cvec2_allocator_set set,
                                           size_t member_size);

// destroy an allocator, and free the memory held by it
// does nothing when called on an already destroyed vector
CVEC2_EXPORT void cvec2_destroy(struct cvec2 *vector);

/* -- MEMORY MANAGEMENT -- */

// repeatedly grows the vectors capacity by multiplying its capacity by
// CVEC2_DEFAULT_GROWTH, until the vectors capacity >= cap
CVEC2_EXPORT void cvec2_grow(struct cvec2 *vector, size_t cap);

// resize the capacity of the vector, destructively
CVEC2_EXPORT void cvec2_resize(struct cvec2 *vector, size_t size);

// resize the capacity of the vector to be at least size
// does not grow the vector past size unless it is already greater than size
CVEC2_EXPORT void cvec2_reserve(struct cvec2 *vector, size_t size);

/* -- MUTATING FUNCTIONS -- */

// push a value onto the end of a vector
CVEC2_EXPORT void cvec2_push(struct cvec2 *vector, void *data);

// pop a value into a location of memory
CVEC2_EXPORT void cvec2_pop(struct cvec2 *vector, void *into);

CVEC2_EXPORT void cvec2_append(struct cvec2 *into, struct cvec2 *from);
CVEC2_EXPORT void cvec2_append_list(struct cvec2 *into, void *from, size_t len);

// insert a value into a vector, preserving order,
// at a specific index specified by whence
CVEC2_EXPORT void cvec2_insert(struct cvec2 *vector, void *from, size_t whence);

// insert a value into a vector, not preserving order,
// at a specifid index specified by whence
// faster than regular insert
CVEC2_EXPORT void cvec2_insert_fast(struct cvec2 *vector, void *from,
                                    size_t whence);

// TODO: unimplemented
// CVEC2_EXPORT void cvec2_insert_vec(struct cvec2 *into, struct cvec2 *from,
//                                    size_t whence);

// remove a value at a specified location, preserving order
CVEC2_EXPORT void cvec2_remove(struct cvec2 *vector, size_t whence);

// remove a value at a specified location, not preserving order
// faster than regular remove
CVEC2_EXPORT void cvec2_remove_fast(struct cvec2 *vector, size_t whence);

/* -- HELPER MACROS -- */

#define CVEC2_GET(vector__, ty__, idx__) ((ty__ *)(vector__)->ptr)[idx__]
#define CVEC2_GET_BYTE_INDEX_OF(vector__, idx__) ((vector__)->elem_size * idx__)

#ifdef CVEC2_IMPLEMENTATION

#include <stdlib.h>

const static struct cvec2_allocator_set cvec2_SG_default_allocator_set = {
    .alloc = malloc,
    .realloc = realloc,
    .free = free,
};

// TODO: optimize this
static inline void cvec2_memcpy(uint8_t *to, uint8_t *from, size_t size) {
    for (size_t i = 0; i < size; i++)
        to[i] = from[i];
}

/* -- CONSTRUCTORS -- */

CVEC2_EXPORT struct cvec2 cvec2_init_default(size_t size) {
    return (struct cvec2){
        .ptr = malloc(CVEC2_DEFAULT_CAP * size),
        .cap = CVEC2_DEFAULT_CAP,
        .len = 0,
        .elem_size = size,
        .allocator_set = cvec2_SG_default_allocator_set,
    };
}

CVEC2_EXPORT struct cvec2 cvec2_init_alloc(struct cvec2_allocator_set set,
                                           size_t member_size) {
    return (struct cvec2){
        .ptr = set.alloc(CVEC2_DEFAULT_CAP * member_size),
        .cap = CVEC2_DEFAULT_CAP,
        .len = 0,
        .elem_size = member_size,
        .allocator_set = set,
    };
}

CVEC2_EXPORT void cvec2_destroy(struct cvec2 *vec) {
    if (vec->ptr) {
        vec->allocator_set.free(vec->ptr);
        vec->ptr = 0;
        vec->len = 0;
        vec->cap = 0;
        vec->elem_size = 0;
    }
}

/* -- MEMORY MANAGEMENT -- */

CVEC2_EXPORT void cvec2_grow(struct cvec2 *vector, size_t cap) {
    while (vector->cap < cap)
        vector->cap *= CVEC2_DEFAULT_GROWTH;
    vector->ptr = vector->allocator_set.realloc(
        vector->ptr, vector->cap * vector->elem_size);
}

CVEC2_EXPORT void cvec2_resize(struct cvec2 *vector, size_t size) {
    vector->cap = size;
    vector->ptr = vector->allocator_set.realloc(vector->ptr, size);
}

CVEC2_EXPORT void cvec2_reserve(struct cvec2 *vector, size_t size) {
    if (vector->cap >= size)
        return;

    vector->cap = size;
    vector->ptr =
        vector->allocator_set.realloc(vector->ptr, size * vector->elem_size);
}

/* -- MUTATING FUNCTIONS -- */

CVEC2_EXPORT void cvec2_push(struct cvec2 *vector, void *data) {
    cvec2_grow(vector, vector->len + 1);
    cvec2_memcpy(&vector->ptr[CVEC2_GET_BYTE_INDEX_OF(vector, vector->len)],
                 data, vector->elem_size);
    vector->len += 1;
}

CVEC2_EXPORT void cvec2_pop(struct cvec2 *vector, void *into) {
    cvec2_memcpy(into,
                 &vector->ptr[CVEC2_GET_BYTE_INDEX_OF(vector, vector->len - 1)],
                 vector->elem_size);
    vector->len -= 1;
}

CVEC2_EXPORT void cvec2_append(struct cvec2 *into, struct cvec2 *from) {
    cvec2_grow(into, into->cap + from->cap);

    for (size_t i = 0; i < from->len; i++) {
        cvec2_memcpy(into->ptr + (i + into->len) * into->elem_size,
                     from->ptr + i * into->elem_size, into->elem_size);
    }

    into->len += from->len;
}

CVEC2_EXPORT void cvec2_append_list(struct cvec2 *into, void *from,
                                    size_t len) {
    cvec2_grow(into, into->len + len);

    for (size_t i = 0; i < len; i++) {
        cvec2_memcpy(into->ptr + (i + into->len) * into->elem_size,
                     &((uint8_t *)from)[i * into->elem_size], into->elem_size);
    }

    into->len += len;
}

CVEC2_EXPORT void cvec2_insert_vec(struct cvec2 *into, struct cvec2 *from,
                                   size_t whence) {
    const size_t elem_size = into->elem_size;

    cvec2_grow(into, into->len + from->len);

    for (size_t i = into->len - 1; i > whence; i--) {
        void *from_ptr = into->ptr + i * elem_size;
        void *into_ptr = into->ptr + (i + from->len) * elem_size;
        cvec2_memcpy(into_ptr, from_ptr, elem_size);
    }

    for (size_t i = 0; i < from->len; i++) {
        void *from_ptr = from->ptr + i * elem_size;
        void *into_ptr = into->ptr + (i + whence) * elem_size;
        cvec2_memcpy(into_ptr, from_ptr, elem_size);
    }
    into->len += from->len;
}

// must preserve order
CVEC2_EXPORT void cvec2_insert(struct cvec2 *vector, void *from,
                               size_t whence) {
    cvec2_grow(vector, vector->len + 1);

    // move all elements in front of whence one forward
    for (size_t i = vector->len; i > whence; i--) {
        const size_t byte_idx_here = CVEC2_GET_BYTE_INDEX_OF(vector, i);
        const size_t byte_idx_before = CVEC2_GET_BYTE_INDEX_OF(vector, i - 1);

        cvec2_memcpy(&vector->ptr[byte_idx_here], &vector->ptr[byte_idx_before],
                     vector->elem_size);
    }

    // now move from into idx whence
    cvec2_memcpy(&vector->ptr[CVEC2_GET_BYTE_INDEX_OF(vector, whence)], from,
                 vector->elem_size);

    vector->len += 1;
}

CVEC2_EXPORT void cvec2_insert_fast(struct cvec2 *vector, void *from,
                                    size_t whence) {
    cvec2_grow(vector, vector->len + 1);

    cvec2_memcpy(&vector->ptr[vector->len],
                 &vector->ptr[CVEC2_GET_BYTE_INDEX_OF(vector, whence)],
                 vector->elem_size);
    cvec2_memcpy(&vector->ptr[CVEC2_GET_BYTE_INDEX_OF(vector, whence)], from,
                 vector->elem_size);
}

// must preserve order
CVEC2_EXPORT void cvec2_remove(struct cvec2 *vector, size_t whence) {
    for (size_t i = whence; i < vector->len; i++)
        cvec2_memcpy(&vector->ptr[CVEC2_GET_BYTE_INDEX_OF(vector, i)],
                     &vector->ptr[CVEC2_GET_BYTE_INDEX_OF(vector, i + 1)],
                     vector->elem_size);
    vector->len -= 1;
}

CVEC2_EXPORT void cvec2_remove_fast(struct cvec2 *vector, size_t whence) {
    cvec2_memcpy(&vector->ptr[CVEC2_GET_BYTE_INDEX_OF(vector, whence)],
                 &vector->ptr[CVEC2_GET_BYTE_INDEX_OF(vector, vector->len - 1)],
                 vector->elem_size);
    vector->len -= 1;
}

#endif
