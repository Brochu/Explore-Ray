
typedef struct {
    int lifetime;
} partfx_t;

void partfx_init(partfx_t *pfx);
void partfx_parse(partfx_t *pfx, const char *data, size_t length);
void partfx_delete(partfx_t *pfx);
