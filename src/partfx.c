#include "partfx.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "yaml.h"

typedef enum {
    INT,
    STR,
    FLOAT,
    TYPE_COUNT,
} PropType;

typedef struct {
    partfx_prop_t prop;
    int intval;
    char strval[64];
    float flval;
    //TODO: How should we handle vectors?
} partfx_cnst_t;

typedef struct {
    partfx_prop_t prop;
    float a;
    float b;
} partfx_rand_t;

#define print_problem(parser)                    \
    do {                                         \
        printf("[YAML] ERROR:\n%s : %zu (%i)\n", \
            parser.problem,                      \
            parser.problem_offset,               \
            parser.problem_value);               \
    } while(0)

static const char *nameLUT[PROP_COUNT] = { "PSLT", "MAXP", "TEXR", "GRTE" };
static const size_t off0LUT[PROP_COUNT] = { 1, 1, 1, 1 };
static const size_t off1LUT[PROP_COUNT] = { 0, 0, 2, 1 };
static const PropType typeLUT[PROP_COUNT] = { INT, INT, STR, FLOAT };

#define check_prop(prop, idx, type)                                        \
    do {                                                                   \
        if (strcmp((char *)node->data.scalar.value, nameLUT[prop]) == 0) { \
            targetProp = prop;                                             \
            type = typeLUT[prop];                                          \
            idx += off0LUT[prop];                                          \
            query = yaml_document_get_node(&doc, ++idx);                   \
            idx += off1LUT[prop];                                          \
            value = yaml_document_get_node(&doc, ++idx);                   \
        }                                                                  \
    } while (0)

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
            PropType type = TYPE_COUNT;

            //TODO: Still not too happy with this
            check_prop(LIFETIME, i, type);
            check_prop(MAX_PARTICLES, i, type);
            check_prop(TEXTURE, i, type);
            check_prop(GEN_RATE, i, type);

            if (targetProp != -1) {
                partfx_prop_t *c = NULL;
                //TODO: Handle different query types
                if (strcmp((char *)query->data.scalar.value, "CNST") == 0) {
                    c = malloc(sizeof(partfx_cnst_t));
                    c->query = CONST;

                    partfx_cnst_t *cnst = (partfx_cnst_t *)c;
                    if (type == STR) {
                        strncpy_s(cnst->strval, 64, (char *)value->data.scalar.value, value->data.scalar.length);
                    }
                    else if (type == INT) {
                        cnst->intval = strtol((char *)value->data.scalar.value, NULL, 0);
                    }
                    else if (type == FLOAT) {
                        cnst->flval = strtof((char *)value->data.scalar.value, NULL);
                    }
                }
                else if (strcmp((char *)query->data.scalar.value, "RAND") == 0) {
                    //TODO: Handle random parsing
                    c = malloc(sizeof(partfx_rand_t));
                    c->query = RAND;

                    partfx_rand_t *rand = (partfx_rand_t *)c;
                    i += 3;
                    yaml_node_t *nodeA = yaml_document_get_node(&doc, i);
                    i += 4;
                    yaml_node_t *nodeB = yaml_document_get_node(&doc, i);
                    rand->a = strtof((char *)nodeA->data.scalar.value, NULL);
                    rand->b = strtof((char *)nodeB->data.scalar.value, NULL);
                }
                pfx->_props[targetProp] = c;
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
    else if (pfx->_props[prop]->query == RAND) {
        partfx_rand_t *r = (partfx_rand_t *)pfx->_props[prop];
        float *value = (float *)out;
        *value = r->a + ((float)rand() / RAND_MAX) * (r->b - r->a);
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
