#include "catalog.h"

#include <stdio.h>
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

Catalog ParseCatalog() {
    char *txt = LoadFileText(PATH);

    _nameblock = malloc(MAX_COUNT * NAME_SIZE);
    _pathblock = malloc(MAX_COUNT * PATH_SIZE);
    Catalog res = {
        .size = 0,
        .names = calloc(MAX_COUNT, sizeof(char*)),
        .paths = calloc(MAX_COUNT, sizeof(char*)),
    };

    char *token, *next_token;
    token = strtok_s(txt, "\n", &next_token);
    while(token) {
        char *name, *path = NULL;
        size_t nsize, psize = 0;
        SplitOnce(token, ":", &name, &nsize, &path, &psize);
        //printf("[NAME] (%zu) -> '%s'\n", nsize, name);
        //printf("[PATH] (%zu) -> '%s'\n", psize, path);

        memcpy_s(NAME_AT(res.size), nsize, name, nsize);
        _nameblock[res.size * NAME_SIZE + nsize] = '\0';//'*';
        _nameblock[res.size * NAME_SIZE + NAME_SIZE - 1] = '|';

        memcpy_s(PATH_AT(res.size), psize, path, psize);
        _pathblock[res.size * PATH_SIZE + psize] = '\0';//'*';
        _pathblock[res.size * PATH_SIZE + PATH_SIZE - 1] = '|';

        res.names[res.size] = NAME_AT(res.size);
        res.paths[res.size] = PATH_AT(res.size);

        res.size++;
        token = strtok_s(NULL, "\n", &next_token);
    }

    return res;
}

void DeleteCatalog(Catalog *c) {
    free(c->paths);
    free(c->names);
    free(_pathblock);
    free(_nameblock);
}
