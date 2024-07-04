#ifndef PARTFX_H
#define PARTFX_H

#define MAX_NODES 512

struct partfx_val_t {
    struct partfx_val_t *value;
};
typedef struct partfx_val_t partfx_val_t;

typedef struct {
    const char *name;
    struct partfx_val_t *val;
} partfx_node_t;

typedef struct {
    size_t length; //Total amount of frames
    size_t p; //Progress in frame #
    float t; //Progress in normalized value

    //TODO: Find a way to preallocate X props (with max concrete prop size), let's say X = 100?
    partfx_node_t *_props[MAX_NODES];
} partfx_t;

void partfx_init(partfx_t *pfx);
void partfx_reset(partfx_t *pfx);
void partfx_parse(partfx_t *pfx, const char *data, size_t length);
void partfx_query(partfx_t *pfx, const char *name, void *out);
void partfx_delete(partfx_t *pfx);

#endif // PARTFX_H
