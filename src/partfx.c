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

#define MAX_ITEMS 1024
#define print_problem(parser)                    \
    do {                                         \
        printf("[YAML] ERROR:\n%s : %zu (%i)\n", \
            parser.problem,                      \
            parser.problem_offset,               \
            parser.problem_value);               \
    } while(0)

typedef struct {
    partfx_node_t node;
    partfx_node_t *value;
} partfx_map_t;

void partfx_init(partfx_t *pfx) {
    memset(pfx, 0, sizeof(partfx_t));

    if (arena == NULL) {
        arena_init();
    }
}

void partfx_reset(partfx_t *pfx) {
    arena_reset();
    memset(pfx, 0, sizeof(partfx_t));
}

void partfx_parse(partfx_t *pfx, const char *data, size_t length) {
    yaml_parser_t parser;
    yaml_document_t doc;
    memset(&doc, 0, sizeof(yaml_document_t));
    yaml_node_t *node = NULL;

    // Initialize parser
    if(!yaml_parser_initialize(&parser)) {
        printf("Failed to initialize parser!\n");
        print_problem(parser);
        exit(EXIT_FAILURE);
    }
    // Set input string
    yaml_parser_set_input_string(&parser, (unsigned char *)data, length);
    if (!yaml_parser_load(&parser, &doc)) {
        printf("Failed to parse yaml document!\n");
        print_problem(parser);
        yaml_parser_delete(&parser);
        exit(EXIT_FAILURE);
    }

    int i = 4;
    size_t idx = 0;
    partfx_node_t *current = NULL;
    while (1) {
        node = yaml_document_get_node(&doc, i);
        if (node == NULL || i > 11) break;

        current = arena_alloc(sizeof(partfx_map_t));
        printf("Parsing prop: '%s'\n", node->data.scalar.value);
        strncpy_s(current->type, 32, (char *)node->data.scalar.value, node->data.scalar.length);

        node = yaml_document_get_node(&doc, ++i);
        if (node->type == YAML_SCALAR_NODE) {
            printf("[VAL] '%s'\n", node->data.scalar.value);
        }
        else if (node->type == YAML_MAPPING_NODE) {
            yaml_node_pair_t *pair = node->data.mapping.pairs.start;
            yaml_node_t *key = yaml_document_get_node(&doc, pair->key);
            yaml_node_t *value = yaml_document_get_node(&doc, pair->value);
            printf("[MAP] [%i] '%s' -> [%i] '%s'\n", pair->key, key->data.scalar.value, pair->value, value->data.scalar.value);
            i = pair->value + 1;
        }
        else if (node->type == YAML_SEQUENCE_NODE) {
            printf("[Node at %i][SEQ]\n", i);
        }

        pfx->_props[idx++] = current;
        /*
        else if (node->type == YAML_SEQUENCE_NODE) {
            yaml_node_item_t *start = node->data.sequence.items.start;
            yaml_node_item_t *top = node->data.sequence.items.top;

            yaml_node_item_t list[MAX_ITEMS];
            size_t len = 0;
            while (*start != *top) {
                list[len++] = *start;
                start++;
            }
            printf("[Node at %i][SEQ]\n", i);
            for (int j = 0; j < len; ++j) {
                printf("\t- at: %i\n", list[j]);
            }
        }
        */
    }
    // Cleanup
    yaml_document_delete(&doc);
    yaml_parser_delete(&parser);
}

void partfx_query(partfx_t *pfx, const char *name, void *out) {
}

void partfx_delete(partfx_t *pfx) {
    arena_clear();
    memset(pfx, 0, sizeof(partfx_t));
}
