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

const char *event_str(yaml_event_type_t type) {
    switch (type) {
    case YAML_NO_EVENT:             return "NOP";
    case YAML_STREAM_START_EVENT:   return "ST+";
    case YAML_STREAM_END_EVENT:     return "ST-";
    case YAML_DOCUMENT_START_EVENT: return "DC+";
    case YAML_DOCUMENT_END_EVENT:   return "DC-";
    case YAML_ALIAS_EVENT:          return "ALS";
    case YAML_SCALAR_EVENT:         return "SCL";
    case YAML_SEQUENCE_START_EVENT: return "SQ+";
    case YAML_SEQUENCE_END_EVENT:   return "SQ-";
    case YAML_MAPPING_START_EVENT:  return "MP+";
    case YAML_MAPPING_END_EVENT:    return "MP-";
    }

    return "N/A";
}

#define print_problem(parser)                    \
    do {                                         \
        printf("[YAML] ERROR:\n%s : %zu (%i)\n", \
            parser->problem,                      \
            parser->problem_offset,               \
            parser->problem_value);               \
    } while(0)
#define MAX_ITEMS 512

void next_event(yaml_parser_t *parser, yaml_event_t *e) {
    if (!yaml_parser_parse(parser, e)) {
        printf("Failed to parse next event!\n");
        print_problem(parser);
        exit(EXIT_FAILURE);
    }
}
void print_event(yaml_event_t e) {
    printf("[type=%s]", event_str(e.type));
    if (e.type == YAML_SCALAR_EVENT) {
        printf(" '%s'\n", e.data.scalar.value);
    } else {
        printf("\n");
    }
}

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

    yaml_node_t *root = yaml_document_get_root_node(&doc);
    if (root->type == YAML_SCALAR_NODE) {
        printf("[SCALAR] Value = '%s'\n", root->data.scalar.value);
    }
    else if (root->type == YAML_MAPPING_NODE) {
        yaml_node_pair_t *pair = root->data.mapping.pairs.start;
        printf("[MAPPING] %i -> %i\n", pair->key, pair->value);
    }
    else if (root->type == YAML_SEQUENCE_NODE) {
        printf("[SEQUENCE]\n");
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
