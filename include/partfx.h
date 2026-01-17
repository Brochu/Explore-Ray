#ifndef PARTFX_H
#define PARTFX_H

#include <stddef.h>
#include <stdbool.h>

// Node types in the particle effect tree
typedef enum {
    NODE_NULL,
    NODE_INT,
    NODE_FLOAT,
    NODE_STRING,
    NODE_BOOL,
    NODE_MAP,      // Dictionary/object
    NODE_SEQUENCE  // Array
} partfx_node_type_t;

// Forward declaration
typedef struct partfx_node_s partfx_node_t;

// For MAP nodes - key-value pairs
typedef struct {
    char *key;
    partfx_node_t *value;
} partfx_map_entry_t;

// For SEQUENCE nodes - array of nodes
typedef struct {
    partfx_node_t **items;
    size_t count;
    size_t capacity;
} partfx_sequence_t;

// Generic node structure
struct partfx_node_s {
    partfx_node_type_t type;
    union {
        int as_int;
        float as_float;
        char *as_string;
        bool as_bool;
        struct {
            partfx_map_entry_t *entries;
            size_t count;
            size_t capacity;
        } as_map;
        partfx_sequence_t as_sequence;
    } data;
};

// Main particle effect structure
typedef struct {
    partfx_node_t *root;  // Root map containing all properties

    size_t num_frames;
    size_t p;
    float t;
} partfx_t;

// Core API
void partfx_init(partfx_t *pfx);
void partfx_reset(partfx_t *pfx);
bool partfx_parse(partfx_t *pfx, const char *data, size_t length);
void partfx_delete(partfx_t *pfx);

// Query API - navigate the tree structure
partfx_node_t *partfx_get(partfx_node_t *node, const char *key);
partfx_node_t *partfx_get_path(partfx_t *pfx, const char *path); // e.g. "PMCL/KEYE/keys"

// Helper getters with defaults
int partfx_get_int(partfx_node_t *node, const char *key, int default_val);
float partfx_get_float(partfx_node_t *node, const char *key, float default_val);
const char *partfx_get_string(partfx_node_t *node, const char *key, const char *default_val);
bool partfx_get_bool(partfx_node_t *node, const char *key, bool default_val);

// Sequence helpers
size_t partfx_sequence_length(partfx_node_t *seq);
partfx_node_t *partfx_sequence_get(partfx_node_t *seq, size_t index);

// Debug
void partfx_print_tree(partfx_node_t *node, int indent);

#endif // PARTFX_H
