#include "catalog.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "raylib.h"

#define PATH "gundata/!catalog.yaml"
#define MAX_COUNT 128
#define NAME_SIZE 32
#define NAME_IDX(i) (i * NAME_SIZE)

Catalog ParseCatalog() {
    //TODO: Should we all keep the data static in here?
    char *txt = LoadFileText(PATH);

    Catalog res = { .size = 0 };
    res._nameblock = malloc(MAX_COUNT * NAME_SIZE);
    res.names = calloc(MAX_COUNT, sizeof(char*));

    char *token, *next_token;
    token = strtok_s(txt, "\n", &next_token);
    while(token) {
        char *split = strstr(token, ":");
        size_t len = split - token;
        memcpy_s(&res._nameblock[NAME_IDX(res.size)], len, token, len);
        res._nameblock[NAME_IDX(res.size) + len] = '\0';
        res.names[res.size] = &res._nameblock[NAME_IDX(res.size)];
        res.size++;
        token = strtok_s(NULL, "\n", &next_token);
    }

    return res;
}

void DeleteCatalog(Catalog *c) {
    free(c->_nameblock);
    free(c->names);
}
