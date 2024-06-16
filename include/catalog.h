
typedef struct {
    size_t size;

    char *_nameblock;
    char **names;
} Catalog;

Catalog ParseCatalog();
void DeleteCatalog(Catalog *c);
