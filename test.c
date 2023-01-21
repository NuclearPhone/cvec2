#include <stdlib.h>

#define CVEC2_IMPLEMENTATION
#include "testc/test.h"

#include "cvec2.h"

static const char *create_destroy() {
    struct cvec2 vec = cvec2_init_default(sizeof(int));
    cvec2_destroy(&vec);
    return NULL;
}

static const char *push() {
    struct cvec2 vec = cvec2_init_default(sizeof(int));

    int data = 4;
    cvec2_push(&vec, &data);

    if (CVEC2_GET(&vec, int, 0) != 4)
        return TESTC_BASIC_ERR;

    cvec2_destroy(&vec);
    return NULL;
}

static const char *insert() {
    struct cvec2 vec = cvec2_init_default(sizeof(int));

    int v0 = 0, v1 = 1, v2 = 2;
    cvec2_push(&vec, &v0);
    cvec2_push(&vec, &v2);
    cvec2_insert(&vec, &v1, 1);

    if (CVEC2_GET(&vec, int, 1) != 1)
        return TESTC_BASIC_ERR;

    cvec2_destroy(&vec);
    return NULL;
}

static const char *array_append() {
    struct cvec2 into = cvec2_init_default(sizeof(int));
    const int from[] = {
        0, 1, 2, 3, 4,
    };

    cvec2_append_list(&into, (void *)from, sizeof from / sizeof(int));

    for (size_t i = 0; i < 5; i++)
        if (CVEC2_GET(&into, int, i) != (int)i)
            return TESTC_BASIC_ERR;

    cvec2_destroy(&into);
    return NULL;
}

static const char *vec_insert() {
    const int into_init[] = {
        0,
        1,
        4,
        5,
    };

    const int from_init[] = {
        2,
        3,
    };

    struct cvec2 into = cvec2_init_default(sizeof(int));
    struct cvec2 from = cvec2_init_default(sizeof(int));

    cvec2_append_list(&into, (void *)into_init,
                      sizeof(into_init) / sizeof(int));
    cvec2_append_list(&from, (void *)from_init,
                      sizeof(from_init) / sizeof(int));

    cvec2_insert_vec(&into, &from, 2);

    for (size_t i = 0; i < 6; i++)
        if (CVEC2_GET(&into, int, i) != (int)i)
            return TESTC_BASIC_ERR;

    cvec2_destroy(&into);
    return NULL;
}

static test_t tests[] = {
    (test_t){
        .ptr = create_destroy,
        .name = "create destroy",
        .desc = "",
    },

    (test_t){
        .ptr = push,
        .name = "push data",
        .desc = "",
    },

    (test_t){
        .ptr = insert,
        .name = "insert in middle",
        .desc = "",
    },

    (test_t){
        .ptr = array_append,
        .name = "append to vec from array",
        .desc = "",
    },

    (test_t){
        .ptr = vec_insert,
        .name = "insert into array from another",
        .desc = "",
    },
};

int main() {
    for (size_t i = 0; i < sizeof tests / sizeof(test_t); i++) {
        execute_test(tests[i]);
    }
}
