#include "partfx.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "yaml.h"

// ARENA ------------------------
// This works since we always have only one partfx active at once
//TODO: Look into making this in a way that we can load multiple particles at once?
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
    memset(res, 0, size);
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
            parser->problem,                     \
            parser->problem_offset,              \
            parser->problem_value);              \
    } while(0)

const char *event_str(yaml_event_type_t type) {
    switch (type) {
        case YAML_NO_EVENT: return "NOP";
        case YAML_STREAM_START_EVENT: return "ST+";
        case YAML_STREAM_END_EVENT: return "ST-";
        case YAML_DOCUMENT_START_EVENT: return "DC+";
        case YAML_DOCUMENT_END_EVENT: return "DC-";
        case YAML_ALIAS_EVENT: return "ALI";
        case YAML_SCALAR_EVENT: return "VAL";
        case YAML_SEQUENCE_START_EVENT: return "SQ+";
        case YAML_SEQUENCE_END_EVENT: return "SQ-";
        case YAML_MAPPING_START_EVENT: return "MP+";
        case YAML_MAPPING_END_EVENT: return "MP-";
        default: return "N/A";
    }
}

#define MAX_VAL 64

typedef struct {
    partfx_node_t node;
    char val[MAX_VAL];
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
    yaml_event_t e = { 0 };

    // Initialize parser
    if(!yaml_parser_initialize(&parser)) {
        printf("Failed to initialize parser!\n");
        print_problem((&parser));
        exit(EXIT_FAILURE);
    }

    // Set input string
    yaml_parser_set_input_string(&parser, (unsigned char *)data, length);

    while (e.type != YAML_MAPPING_START_EVENT) {
        if (!yaml_parser_parse(&parser, &e)) {
            printf("Could not parse next event!\n");
            print_problem((&parser));
            exit(EXIT_FAILURE);
        }
    }

    partfx_node_t *current = NULL;
    int stack = 0;
    char name[TYPE_SIZE];
    name[0] = '\0';

    do {
        if (!yaml_parser_parse(&parser, &e)) {
            printf("Could not parse next event!\n");
            print_problem((&parser));
            exit(EXIT_FAILURE);
        }

        if (name[0] == '\0' && e.data.scalar.value != NULL) {
            printf("[TYPE=%s] value = '%s'\n", event_str(e.type), e.data.scalar.value);
            strncpy_s(name, TYPE_SIZE, (char *)e.data.scalar.value, e.data.scalar.length);
        }
        else {
            if (e.type == YAML_SCALAR_EVENT) {
                printf("[TYPE=%s] '%s'\n", event_str(e.type), e.data.scalar.value);
                partfx_val_t *valnode = arena_alloc(sizeof(partfx_val_t));
                strncpy_s(valnode->node.type, TYPE_SIZE, name, strlen(name));
                strncpy_s(valnode->val, 64, (char *)e.data.scalar.value, e.data.scalar.length);
                current = (partfx_node_t *)valnode;
            }
            else {
                printf("[TYPE=%s]\n", event_str(e.type));
            }

            //TODO: Look into how to handle nested structures here
            // Also need to find how to handle sequences (maybe some struct with a pointer and a count?)
            if (e.type == YAML_MAPPING_START_EVENT) {
                stack++;
            }
            else if (e.type == YAML_MAPPING_END_EVENT) {
                stack--;
            }
            if (stack == 0) {
                pfx->_props[pfx->_prop_len++] = current;

                name[0] = '\0';
                current = NULL;
                printf("---------\n");

                //TEMP
                if (pfx->_prop_len > 2) {
                    break;
                }
                //-------------------------
            }
        }
    } while(e.type != YAML_STREAM_END_EVENT);

    // Cleanup
    yaml_event_delete(&e);
    yaml_parser_delete(&parser);
}

void partfx_query(partfx_t *pfx, const char *name, void *out) {
    //TODO: Working on new parsing, need different queries
}

void partfx_delete(partfx_t *pfx) {
    arena_clear();
    memset(pfx, 0, sizeof(partfx_t));
}
