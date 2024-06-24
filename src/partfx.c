#include "partfx.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "yaml.h"

#define print_problem(parser) \
    do { \
        printf("[YAML] ERROR:\n%s : %zu (%i)\n", \
            parser.problem,                      \
            parser.problem_offset,               \
            parser.problem_value);               \
    } while(0)

typedef struct {
    partfx_prop_t prop;
    int intval;
    char strval[64];
    //TODO: Handle different constant types (float, vectors, ...)
} partfx_cnst_t;

void partfx_init(partfx_t *pfx) {
    memset(pfx, 0, sizeof(partfx_t));
}

void partfx_parse(partfx_t *pfx, const char *data, size_t length) {
    yaml_parser_t parser;
    yaml_document_t doc;
    memset(&doc, 0, sizeof(yaml_document_t));
    yaml_node_t *node = NULL;

    // Initialize parser
    if(!yaml_parser_initialize(&parser)) {
        printf("Failed to initialize parser!\n");
        print_problem(parser);
        exit(EXIT_FAILURE);
    }
    // Set input string
    yaml_parser_set_input_string(&parser, (unsigned char *)data, length);
    if (!yaml_parser_load(&parser, &doc)) {
        printf("Failed to parse yaml document!\n");
        print_problem(parser);
        yaml_parser_delete(&parser);
        exit(EXIT_FAILURE);
    }

    int i = 2;
    while (1) {
        node = yaml_document_get_node(&doc, i);
        if (!node) break;

        if (node->type == YAML_SCALAR_NODE) {
            yaml_node_t *query = NULL;
            yaml_node_t *value = NULL;
            ParticleProps targetProp = -1;

            if (strcmp((char *)node->data.scalar.value, "PSLT") == 0) {
                printf("[YAML] Found PSLT value\n");
                targetProp = LIFETIME;
                ++i; // Have to skip mapping node
                query = yaml_document_get_node(&doc, ++i);
                value = yaml_document_get_node(&doc, ++i);
            }
            else if (strcmp((char *)node->data.scalar.value, "MAXP") == 0) {
                printf("[YAML] Found MAXP value\n");
                targetProp = MAX_PARTICLES;
                ++i; // Have to skip mapping node
                query = yaml_document_get_node(&doc, ++i);
                value = yaml_document_get_node(&doc, ++i);
            }
            else if (strcmp((char *)node->data.scalar.value, "TEXR") == 0) {
                printf("[YAML] Found TEXR value\n");
                targetProp = TEXTURE;
                ++i; // Have to skip mapping node
                query = yaml_document_get_node(&doc, ++i);
                i+=2;
                value = yaml_document_get_node(&doc, ++i);
            }

            if (targetProp != -1) {
                partfx_cnst_t *c = malloc(sizeof(partfx_cnst_t));
                c->prop.query = CONST;
                if (targetProp == TEXTURE) {
                    strncpy_s(c->strval, 64, (char *)value->data.scalar.value, value->data.scalar.length);
                }
                else {
                    c->intval = strtol((char *)value->data.scalar.value, NULL, 0);
                }
                pfx->_props[targetProp] = (partfx_prop_t *)c;
                printf("[%s] '%s'\n", query->data.scalar.value, value->data.scalar.value);
            }
        }
        ++i;
    }
    // Cleanup
    yaml_document_delete(&doc);
    yaml_parser_delete(&parser);
}

void partfx_query(partfx_t *pfx, ParticleProps prop, void *out) {
    if (pfx->_props[prop] == NULL) return;

    if (pfx->_props[prop]->query == CONST) {
        if (prop == TEXTURE) {
            char **value = (char**)out;
            *value = ((partfx_cnst_t*)pfx->_props[prop])->strval;
        }
        else {
            int *value = (int*)out;
            *value = ((partfx_cnst_t*)pfx->_props[prop])->intval;
        }
    }
}

void partfx_delete(partfx_t *pfx) {
    for (int i = 0; i < PROP_COUNT; ++i) {
        if (pfx->_props[i] != NULL) {
            free(pfx->_props[i]);
        }
    }

    memset(pfx, 0, sizeof(partfx_t));
}
