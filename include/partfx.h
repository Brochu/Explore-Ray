#ifndef PARTFX_H
#define PARTFX_H

#define TYPE_SIZE 32
#define MAX_PROPS 64

struct partfx_node_t {
    char type[TYPE_SIZE];

    size_t size;
    struct partfx_node_t *n;
};
typedef struct partfx_node_t partfx_node_t;

typedef struct {
    size_t length; //Total amount of frames
    size_t p; //Progress in frame #
    float t; //Progress in normalized value

    size_t _prop_len;
    partfx_node_t *_props[MAX_PROPS];
} partfx_t;

void partfx_init(partfx_t *pfx);
void partfx_reset(partfx_t *pfx);
void partfx_parse(partfx_t *pfx, const char *data, size_t length);
void partfx_query(partfx_t *pfx, const char *name, void *out);
void partfx_delete(partfx_t *pfx);

#endif // PARTFX_H
