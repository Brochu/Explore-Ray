
typedef struct {
    size_t size;

    const char **names;
    char **paths;
} Catalog;

void ParseCatalog(Catalog *c);
void DeleteCatalog(Catalog *c);
