
typedef struct {
    size_t size;

    char **names;
} Catalog;

Catalog ParseCatalog();
void DeleteCatalog(Catalog *c);
