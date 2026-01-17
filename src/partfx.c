#include "partfx.h"
#include "yaml.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Arena allocator (same as yours but exposed functions)
#define ARENA_SIZE (1024 * 1024)  // 1MB should be enough
static char *arena = NULL;
static char *next = NULL;

static void arena_init() {
    if (arena != NULL) return;
    arena = malloc(ARENA_SIZE);
    assert(arena != NULL && "Could not allocate arena");
    next = arena;
}

static void *arena_alloc(size_t size) {
    if ((next + size) > arena + ARENA_SIZE) {
        assert(0 && "Arena out of memory");
    }
    void *res = next;
    next += size;
    memset(res, 0, size);
    return res;
}

static void arena_reset() {
    next = arena;
}

static void arena_clear() {
    free(arena);
    arena = NULL;
    next = NULL;
}

// String duplication in arena
static char *arena_strdup(const char *str) {
    size_t len = strlen(str) + 1;
    char *copy = arena_alloc(len);
    memcpy(copy, str, len);
    return copy;
}

// Node creation helpers
static partfx_node_t *create_node(partfx_node_type_t type) {
    partfx_node_t *node = arena_alloc(sizeof(partfx_node_t));
    node->type = type;
    return node;
}

static partfx_node_t *create_int_node(int value) {
    partfx_node_t *node = create_node(NODE_INT);
    node->data.as_int = value;
    return node;
}

static partfx_node_t *create_float_node(float value) {
    partfx_node_t *node = create_node(NODE_FLOAT);
    node->data.as_float = value;
    return node;
}

static partfx_node_t *create_string_node(const char *value) {
    partfx_node_t *node = create_node(NODE_STRING);
    node->data.as_string = arena_strdup(value);
    return node;
}

static partfx_node_t *create_bool_node(bool value) {
    partfx_node_t *node = create_node(NODE_BOOL);
    node->data.as_bool = value;
    return node;
}

static partfx_node_t *create_map_node() {
    partfx_node_t *node = create_node(NODE_MAP);
    node->data.as_map.capacity = 16;
    node->data.as_map.entries = arena_alloc(sizeof(partfx_map_entry_t) * 16);
    return node;
}

static partfx_node_t *create_sequence_node() {
    partfx_node_t *node = create_node(NODE_SEQUENCE);
    node->data.as_sequence.capacity = 16;
    node->data.as_sequence.items = arena_alloc(sizeof(partfx_node_t*) * 16);
    return node;
}

// Add to map
static void map_add(partfx_node_t *map, const char *key, partfx_node_t *value) {
    assert(map->type == NODE_MAP);
    
    if (map->data.as_map.count >= map->data.as_map.capacity) {
        // Grow capacity
        size_t new_cap = map->data.as_map.capacity * 2;
        partfx_map_entry_t *new_entries = arena_alloc(sizeof(partfx_map_entry_t) * new_cap);
        memcpy(new_entries, map->data.as_map.entries, 
               sizeof(partfx_map_entry_t) * map->data.as_map.count);
        map->data.as_map.entries = new_entries;
        map->data.as_map.capacity = new_cap;
    }
    
    map->data.as_map.entries[map->data.as_map.count].key = arena_strdup(key);
    map->data.as_map.entries[map->data.as_map.count].value = value;
    map->data.as_map.count++;
}

// Add to sequence
static void sequence_add(partfx_node_t *seq, partfx_node_t *value) {
    assert(seq->type == NODE_SEQUENCE);
    
    if (seq->data.as_sequence.count >= seq->data.as_sequence.capacity) {
        size_t new_cap = seq->data.as_sequence.capacity * 2;
        partfx_node_t **new_items = arena_alloc(sizeof(partfx_node_t*) * new_cap);
        memcpy(new_items, seq->data.as_sequence.items,
               sizeof(partfx_node_t*) * seq->data.as_sequence.count);
        seq->data.as_sequence.items = new_items;
        seq->data.as_sequence.capacity = new_cap;
    }
    
    seq->data.as_sequence.items[seq->data.as_sequence.count++] = value;
}

// Parse scalar value and determine type
static partfx_node_t *parse_scalar(const char *value) {
    if (strcmp(value, "True") == 0 || strcmp(value, "true") == 0) {
        return create_bool_node(true);
    }
    if (strcmp(value, "False") == 0 || strcmp(value, "false") == 0) {
        return create_bool_node(false);
    }
    
    // Check if it's a hex number (0x prefix)
    if (strncmp(value, "0x", 2) == 0) {
        int hex_val = (int)strtol(value, NULL, 16);
        return create_int_node(hex_val);
    }
    
    // Check if it's a number
    char *endptr;
    long int_val = strtol(value, &endptr, 10);
    if (*endptr == '\0') {
        return create_int_node((int)int_val);
    }
    
    float float_val = strtof(value, &endptr);
    if (*endptr == '\0') {
        return create_float_node(float_val);
    }
    
    // Otherwise it's a string
    return create_string_node(value);
}

// Stack for tracking context during parsing
typedef struct {
    partfx_node_t *node;
    char *key;  // For maps, the pending key
    bool is_key_next;  // True if next scalar is a key
} parse_context_t;

#define MAX_DEPTH 64

// Main parsing function
bool partfx_parse(partfx_t *pfx, const char *data, size_t length) {
    yaml_parser_t parser;
    yaml_event_t event;
    
    if (!yaml_parser_initialize(&parser)) {
        fprintf(stderr, "Failed to initialize YAML parser\n");
        return false;
    }
    
    yaml_parser_set_input_string(&parser, (const unsigned char *)data, length);
    
    parse_context_t stack[MAX_DEPTH];
    int depth = -1;
    
    bool done = false;
    bool success = true;
    
    while (!done) {
        if (!yaml_parser_parse(&parser, &event)) {
            fprintf(stderr, "YAML parse error: %s\n", parser.problem);
            success = false;
            break;
        }
        
        switch (event.type) {
            case YAML_STREAM_START_EVENT:
            case YAML_DOCUMENT_START_EVENT:
                break;
                
            case YAML_MAPPING_START_EVENT: {
                partfx_node_t *map = create_map_node();
                
                if (depth == -1) {
                    // Root map
                    pfx->root = map;
                } else {
                    // Add to parent
                    parse_context_t *parent = &stack[depth];
                    if (parent->node->type == NODE_MAP) {
                        map_add(parent->node, parent->key, map);
                        parent->key = NULL;
                        parent->is_key_next = true;
                    } else if (parent->node->type == NODE_SEQUENCE) {
                        sequence_add(parent->node, map);
                    }
                }
                
                depth++;
                assert(depth < MAX_DEPTH);
                stack[depth].node = map;
                stack[depth].key = NULL;
                stack[depth].is_key_next = true;
                break;
            }
            
            case YAML_SEQUENCE_START_EVENT: {
                partfx_node_t *seq = create_sequence_node();
                
                if (depth >= 0) {
                    parse_context_t *parent = &stack[depth];
                    if (parent->node->type == NODE_MAP) {
                        map_add(parent->node, parent->key, seq);
                        parent->key = NULL;
                        parent->is_key_next = true;
                    } else if (parent->node->type == NODE_SEQUENCE) {
                        sequence_add(parent->node, seq);
                    }
                }
                
                depth++;
                assert(depth < MAX_DEPTH);
                stack[depth].node = seq;
                stack[depth].key = NULL;
                stack[depth].is_key_next = false;
                break;
            }
            
            case YAML_SCALAR_EVENT: {
                const char *value = (const char *)event.data.scalar.value;
                parse_context_t *ctx = &stack[depth];
                
                if (ctx->node->type == NODE_MAP) {
                    if (ctx->is_key_next) {
                        // This scalar is a key
                        ctx->key = arena_strdup(value);
                        ctx->is_key_next = false;
                    } else {
                        // This scalar is a value
                        partfx_node_t *val_node = parse_scalar(value);
                        map_add(ctx->node, ctx->key, val_node);
                        ctx->key = NULL;
                        ctx->is_key_next = true;
                    }
                } else if (ctx->node->type == NODE_SEQUENCE) {
                    partfx_node_t *val_node = parse_scalar(value);
                    sequence_add(ctx->node, val_node);
                }
                break;
            }
            
            case YAML_MAPPING_END_EVENT:
            case YAML_SEQUENCE_END_EVENT:
                depth--;
                break;
                
            case YAML_STREAM_END_EVENT:
            case YAML_DOCUMENT_END_EVENT:
                done = true;
                break;
                
            default:
                break;
        }
        
        yaml_event_delete(&event);
    }
    
    yaml_parser_delete(&parser);
    return success;
}

// Public API implementation
void partfx_init(partfx_t *pfx) {
    memset(pfx, 0, sizeof(partfx_t));
    arena_init();
}

void partfx_reset(partfx_t *pfx) {
    arena_reset();
    memset(pfx, 0, sizeof(partfx_t));
}

void partfx_delete(partfx_t *pfx) {
    arena_clear();
    memset(pfx, 0, sizeof(partfx_t));
}

// Query functions
partfx_node_t *partfx_get(partfx_node_t *node, const char *key) {
    if (node == NULL || node->type != NODE_MAP) return NULL;
    
    for (size_t i = 0; i < node->data.as_map.count; i++) {
        if (strcmp(node->data.as_map.entries[i].key, key) == 0) {
            return node->data.as_map.entries[i].value;
        }
    }
    return NULL;
}

partfx_node_t *partfx_get_path(partfx_t *pfx, const char *path) {
    if (pfx->root == NULL) return NULL;
    
    char path_copy[256];
    strncpy(path_copy, path, sizeof(path_copy) - 1);
    path_copy[sizeof(path_copy) - 1] = '\0';
    
    partfx_node_t *current = pfx->root;
    char *token = strtok(path_copy, "/");
    
    while (token != NULL && current != NULL) {
        current = partfx_get(current, token);
        token = strtok(NULL, "/");
    }
    
    return current;
}

int partfx_get_int(partfx_node_t *node, const char *key, int default_val) {
    partfx_node_t *child = partfx_get(node, key);
    if (child == NULL) return default_val;
    
    if (child->type == NODE_INT) return child->data.as_int;
    if (child->type == NODE_FLOAT) return (int)child->data.as_float;
    
    // Handle CNST wrapper
    if (child->type == NODE_MAP) {
        partfx_node_t *cnst = partfx_get(child, "CNST");
        if (cnst && cnst->type == NODE_INT) return cnst->data.as_int;
        if (cnst && cnst->type == NODE_FLOAT) return (int)cnst->data.as_float;
    }
    
    return default_val;
}

float partfx_get_float(partfx_node_t *node, const char *key, float default_val) {
    partfx_node_t *child = partfx_get(node, key);
    if (child == NULL) return default_val;
    
    if (child->type == NODE_FLOAT) return child->data.as_float;
    if (child->type == NODE_INT) return (float)child->data.as_int;
    
    // Handle CNST wrapper
    if (child->type == NODE_MAP) {
        partfx_node_t *cnst = partfx_get(child, "CNST");
        if (cnst && cnst->type == NODE_FLOAT) return cnst->data.as_float;
        if (cnst && cnst->type == NODE_INT) return (float)cnst->data.as_int;
    }
    
    return default_val;
}

const char *partfx_get_string(partfx_node_t *node, const char *key, const char *default_val) {
    partfx_node_t *child = partfx_get(node, key);
    if (child == NULL) return default_val;
    
    if (child->type == NODE_STRING) return child->data.as_string;
    
    // Handle CNST wrapper
    if (child->type == NODE_MAP) {
        partfx_node_t *cnst = partfx_get(child, "CNST");
        if (cnst && cnst->type == NODE_STRING) return cnst->data.as_string;
    }
    
    return default_val;
}

bool partfx_get_bool(partfx_node_t *node, const char *key, bool default_val) {
    partfx_node_t *child = partfx_get(node, key);
    if (child == NULL) return default_val;
    if (child->type == NODE_BOOL) return child->data.as_bool;
    return default_val;
}

size_t partfx_sequence_length(partfx_node_t *seq) {
    if (seq == NULL || seq->type != NODE_SEQUENCE) return 0;
    return seq->data.as_sequence.count;
}

partfx_node_t *partfx_sequence_get(partfx_node_t *seq, size_t index) {
    if (seq == NULL || seq->type != NODE_SEQUENCE) return NULL;
    if (index >= seq->data.as_sequence.count) return NULL;
    return seq->data.as_sequence.items[index];
}

// Debug printing
void partfx_print_tree(partfx_node_t *node, int indent) {
    if (node == NULL) return;
    
    for (int i = 0; i < indent; i++) printf("  ");
    
    switch (node->type) {
        case NODE_INT:
            printf("INT: %d\n", node->data.as_int);
            break;
        case NODE_FLOAT:
            printf("FLOAT: %f\n", node->data.as_float);
            break;
        case NODE_STRING:
            printf("STRING: %s\n", node->data.as_string);
            break;
        case NODE_BOOL:
            printf("BOOL: %s\n", node->data.as_bool ? "true" : "false");
            break;
        case NODE_MAP:
            printf("MAP (%zu entries):\n", node->data.as_map.count);
            for (size_t i = 0; i < node->data.as_map.count; i++) {
                for (int j = 0; j < indent + 1; j++) printf("  ");
                printf("%s: ", node->data.as_map.entries[i].key);
                if (node->data.as_map.entries[i].value->type == NODE_MAP || 
                    node->data.as_map.entries[i].value->type == NODE_SEQUENCE) {
                    printf("\n");
                    partfx_print_tree(node->data.as_map.entries[i].value, indent + 2);
                } else {
                    partfx_print_tree(node->data.as_map.entries[i].value, 0);
                }
            }
            break;
        case NODE_SEQUENCE:
            printf("SEQUENCE (%zu items):\n", node->data.as_sequence.count);
            for (size_t i = 0; i < node->data.as_sequence.count; i++) {
                partfx_print_tree(node->data.as_sequence.items[i], indent + 1);
            }
            break;
        default:
            printf("UNKNOWN\n");
    }
}
