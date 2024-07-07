#ifndef PARTFX_H
#define PARTFX_H

#define TYPE_SIZE 32

typedef enum {
    PSTL,
    MAXP,
    PROP_COUNT,
} Partfx_Prop;

typedef struct {
    char type[TYPE_SIZE];
} partfx_node_t;

typedef struct {
    size_t length; //Total amount of frames
    size_t p; //Progress in frame #
    float t; //Progress in normalized value

    //TODO: Find a way to preallocate X props (with max concrete prop size), let's say X = 100?
    //TODO: Maybe we only store indices in the yaml_document_t??
    partfx_node_t *_props[PROP_COUNT];
} partfx_t;

void partfx_init(partfx_t *pfx);
void partfx_reset(partfx_t *pfx);
void partfx_parse(partfx_t *pfx, const char *data, size_t length);
void partfx_query(partfx_t *pfx, const char *name, void *out);
void partfx_delete(partfx_t *pfx);

#endif // PARTFX_H
