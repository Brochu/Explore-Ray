
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

typedef enum {
    INT,
    STRING,
    FLOAT,
    TYPE_COUNT,
} PropType;

typedef struct {
    PropQuery query;
    PropType type;
} partfx_prop_t;

typedef struct {
    size_t length;
    size_t p; //Progress in frame #
    float t; //Progress in normalized value

    //TODO: Find a way to preallocate X props (with max concrete prop size), let's say X = 100?
    partfx_prop_t *_props[PROP_COUNT];
} partfx_t;

void partfx_init(partfx_t *pfx);
void partfx_parse(partfx_t *pfx, const char *data, size_t length);
void partfx_query(partfx_t *pfx, ParticleProps prop, void *out);
void partfx_delete(partfx_t *pfx);
