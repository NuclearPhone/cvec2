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
};

int main() {
    for (size_t i = 0; i < sizeof tests / sizeof(test_t); i++) {
        execute_test(tests[i]);
    }
}
