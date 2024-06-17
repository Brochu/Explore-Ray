#include "catalog.h"

#include <stdlib.h>
#include <string.h>
#include "raylib.h"

#define PATH "gundata/!catalog.yaml"
#define MAX_COUNT 128
#define NAME_SIZE 32
#define PATH_SIZE 64
#define NAME_AT(i) (_nameblock + (i * NAME_SIZE))
#define PATH_AT(i) (_pathblock + (i * PATH_SIZE))

void SplitOnce(char *str, const char *delim, char **f, size_t *fs, char **s, size_t *ss) {
    size_t len = strlen(str);
    char *pos = strstr(str, delim);

    if (*pos != '\0') {
        *f = str;
        *fs = pos - str;
        *s = pos + 2;
        *ss = len - *fs - 2;
    }
}

char *_nameblock = NULL;
char *_pathblock = NULL;

void ParseCatalog(Catalog *c) {
    char *txt = LoadFileText(PATH);

    _nameblock = malloc(MAX_COUNT * NAME_SIZE);
    _pathblock = malloc(MAX_COUNT * PATH_SIZE);
    c->size = 0;
    c->names = calloc(MAX_COUNT, sizeof(char*));
    c->paths = calloc(MAX_COUNT, sizeof(char*));

    char *token, *next_token;
    token = strtok_s(txt, "\n", &next_token);
    while(token) {
        char *name, *path = NULL;
        size_t nsize, psize = 0;
        SplitOnce(token, ":", &name, &nsize, &path, &psize);

        memcpy_s(NAME_AT(c->size), nsize, name, nsize);
        _nameblock[c->size * NAME_SIZE + nsize] = '\0';//'*';
        _nameblock[c->size * NAME_SIZE + NAME_SIZE - 1] = '|';

        memcpy_s(PATH_AT(c->size), psize, path, psize);
        _pathblock[c->size * PATH_SIZE + psize] = '\0';//'*';
        _pathblock[c->size * PATH_SIZE + PATH_SIZE - 1] = '|';

        c->names[c->size] = NAME_AT(c->size);
        c->paths[c->size] = PATH_AT(c->size);

        c->size++;
        token = strtok_s(NULL, "\n", &next_token);
    }
}

void DeleteCatalog(Catalog *c) {
    free(c->paths);
    free(c->names);
    free(_pathblock);
    free(_nameblock);
}
