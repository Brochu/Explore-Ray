
typedef enum {
    LIFETIME,
    MAX_PARTICLES,
    TEXTURE,
    GEN_RATE,
    PROP_COUNT,
} ParticleProps;

typedef enum {
    CONST,
    RAND,
    QUERY_COUNT,
} PropQuery;

typedef struct {
    PropQuery query;
} partfx_prop_t;

typedef struct {
    size_t length;
    size_t p; //Progress in frame #
    float t; //Progress in normalized value

    partfx_prop_t *_props[PROP_COUNT];
} partfx_t;

void partfx_init(partfx_t *pfx);
void partfx_parse(partfx_t *pfx, const char *data, size_t length);
void partfx_query(partfx_t *pfx, ParticleProps prop, void *out);
void partfx_delete(partfx_t *pfx);
