
typedef struct {
    size_t size;

    char **names;
    char **paths;
} Catalog;

Catalog ParseCatalog();
void DeleteCatalog(Catalog *c);
