#include "partfx.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "yaml.h"

// ARENA ------------------------
#define ARENA_SIZE 4096
static char *arena = NULL;
static char *next = NULL;

void arena_init() {
    arena = malloc(ARENA_SIZE);
    if (arena == NULL) {
        assert(0 && "Could not allocate arena for partfx");
    }
    memset(arena, 0, ARENA_SIZE);
    next = arena;
}
void *arena_alloc(size_t size) {
    if ((next + size) > arena + ARENA_SIZE) {
        assert(0 && "partfx arena ran out of memory");
    }

    void *res = next;
    next += size;
    return res;
}
void arena_reset() {
    next = arena;
}
void arena_clear() {
    free(arena);

    arena = NULL;
    next = NULL;
}
// ARENA ------------------------

#define print_problem(parser)                    \
    do {                                         \
        printf("[YAML] ERROR:\n%s : %zu (%i)\n", \
            parser->problem,                      \
            parser->problem_offset,               \
            parser->problem_value);               \
    } while(0)
#define MAX_ITEMS 512

typedef struct {
    partfx_node_t node;
    partfx_node_t *value;
} partfx_map_t;

typedef struct {
    partfx_node_t node;
    partfx_node_t *value[MAX_ITEMS];
    size_t len;
} partfx_seq_t;

typedef struct {
    partfx_node_t node;
} partfx_val_t;

void partfx_init(partfx_t *pfx) {
    memset(pfx, 0, sizeof(partfx_t));

    if (arena == NULL) {
        arena_init();
    }
}

//TODO: Find a way to sync this with enum with Xmacros
static const char *prop_names[PROP_COUNT] = { "PSLT", "MAXP" };

void partfx_reset(partfx_t *pfx) {
    arena_reset();
    memset(pfx, 0, sizeof(partfx_t));
}

void partfx_parse(partfx_t *pfx, const char *data, size_t length) {
    yaml_parser_t parser = { 0 };
    yaml_document_t doc;

    // Initialize parser
    if(!yaml_parser_initialize(&parser)) {
        printf("Failed to initialize parser!\n");
        print_problem((&parser));
        exit(EXIT_FAILURE);
    }

    // Set input string
    yaml_parser_set_input_string(&parser, (unsigned char *)data, length);
    if (!yaml_parser_load(&parser, &doc)) {
        printf("Failed to load yaml document!\n");
        print_problem((&parser));
        exit(EXIT_FAILURE);
    }

    int i = 2;
    while (1) {
        yaml_node_t *node = yaml_document_get_node(&doc, i);
        if (node == NULL) break;

        if (node->type == YAML_SCALAR_NODE) {
            printf("[SCALAR] Value = '%s'\n", node->data.scalar.value);
        }
        else if (node->type == YAML_MAPPING_NODE) {
            yaml_node_pair_t *pair = node->data.mapping.pairs.start;
            printf("[MAPPING] %i -> %i\n", pair->key, pair->value);
        }
        else if (node->type == YAML_SEQUENCE_NODE) {
            printf("[SEQUENCE]\n");
        }
        ++i;
    }

    // Cleanup
    yaml_document_delete(&doc);
    yaml_parser_delete(&parser);

    for (int i = 0; i < PROP_COUNT; ++i) {
        printf("[PROP TYPE] -> '%s'\n", prop_names[i]);
    }
}

void partfx_query(partfx_t *pfx, const char *name, void *out) {
}

void partfx_delete(partfx_t *pfx) {
    arena_clear();
    memset(pfx, 0, sizeof(partfx_t));
}
