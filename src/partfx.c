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
    do {
        if (!yaml_parser_parse(&parser, &e)) {
            printf("Could not parse next event!\n");
            print_problem((&parser));
            exit(EXIT_FAILURE);
        }
        if (e.type == YAML_STREAM_END_EVENT) {
            break;
        }

        //TODO: Goal here is to use current and stack to keep status between loops
        // For now, just create a list of top level nodes
        if (e.type == YAML_SCALAR_EVENT) {
            printf("[%s] -> '%s'\n", event_str(e.type), e.data.scalar.value);
        }
        else {
            printf("[%s]\n", event_str(e.type));
        }
        if (current == NULL) {
            //TODO: Temp set
            current = (partfx_node_t *)1;
        }
        else {
            if (e.type == YAML_MAPPING_START_EVENT) {
                stack++;
            }
            else if (e.type == YAML_MAPPING_END_EVENT) {
                stack--;
            }
            if (stack == 0) {
                current = NULL;
                printf("\n");
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
