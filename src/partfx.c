#include "partfx.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"
#include "yaml.h"

typedef struct {
    partfx_prop_t prop;
    //TODO: Look into using a union here
    int intval;
    char strval[64];
    float flval;
    Vector3 v3val;
    Vector4 v4val;
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

//TODO: Is there a way to sync this with the props with XMacros
static const char *nameLUT[PROP_COUNT] = { "PSLT", "MAXP", "TEXR", "GRTE" };
static const PropType typeLUT[PROP_COUNT] = { INT, INT, STRING, FLOAT };

void check_prop(yaml_document_t *doc, int *i, yaml_node_t *n, ParticleProps *target, PropType *type) {
    for (int i = 0; i < PROP_COUNT; ++i) {
        if (strcmp(nameLUT[i], (char *)n->data.scalar.value) == 0) {
            printf("Found valid prop: '%s'\n", n->data.scalar.value);
            *target = (ParticleProps)i;
            *type = typeLUT[i];
        }
    }
}

void parse_prop_cnst(yaml_document_t *doc, int *i, PropType type, partfx_prop_t **p) {
    yaml_node_t *node = NULL;
    while (1) {
        node = yaml_document_get_node(doc, ++(*i));
        if (node->type != YAML_MAPPING_NODE && strcmp((char *)node->data.scalar.value, "tex") != 0) {
            break;
        }
    }
    printf("[PARSE] Need to parse a constant prop, '%i', '%s'\n", type, node->data.scalar.value);

    partfx_cnst_t *cnst = malloc(sizeof(partfx_cnst_t));
    if (type == INT) {
        cnst->intval = strtol((char *)node->data.scalar.value, NULL, 0);
    }
    else if (type == STRING) {
        strncpy_s(cnst->strval, 64, (char *)node->data.scalar.value, node->data.scalar.length);
    }
    else if (type == FLOAT) {
        cnst->flval = strtof((char *)node->data.scalar.value, NULL);
    }
    *p = (partfx_prop_t *)cnst;
}

void parse_prop_rand(yaml_document_t *doc, int *i, PropType type, partfx_prop_t **p) {
    yaml_node_t *node = NULL;
    while (1) {
        node = yaml_document_get_node(doc, ++(*i));
        if (node->type != YAML_MAPPING_NODE) {
            break;
        }
    }
    //TODO: I would like to find a better way to handle offets with MAPPING_START/_END nodes
    (*i)+= 3;
    yaml_node_t *a = yaml_document_get_node(doc, (*i));
    (*i)+= 4;
    yaml_node_t *b = yaml_document_get_node(doc, (*i));
    printf("[PARSE] Need to parse a random prop, a='%s'; b'%s'\n", a->data.scalar.value, b->data.scalar.value);

    partfx_rand_t *rand = malloc(sizeof(partfx_rand_t));
    rand->a = strtof((char *)a->data.scalar.value, NULL);
    rand->b = strtof((char *)b->data.scalar.value, NULL);
    *p = (partfx_prop_t *)rand;
}

void parse_prop(yaml_document_t *doc, int *i, PropType type, PropQuery *query, partfx_prop_t **p) {
    yaml_node_t *ntype = NULL;
    while (1) {
        ntype = yaml_document_get_node(doc, ++(*i));
        if (ntype->type == YAML_MAPPING_NODE) continue;

        if (ntype->type == YAML_SCALAR_NODE && strcmp((char *)ntype->data.scalar.value, "CNST") == 0) {
            parse_prop_cnst(doc, i, type, p);
            *query = CONST;
            return;
        }
        else if (ntype->type == YAML_SCALAR_NODE && strcmp((char *)ntype->data.scalar.value, "RAND") == 0) {
            parse_prop_rand(doc, i, type, p);
            *query = RAND;
            return;
        }
    }
}

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
            ParticleProps target = -1;
            PropType type = -1;
            PropQuery query = -1;
            check_prop(&doc, &i, node, &target, &type);

            if (target >= 0) {
                partfx_prop_t *p = NULL;
                parse_prop(&doc, &i, type, &query, &p);

                if (p != NULL) {
                    p->type = type;
                    p->query = query;
                    pfx->_props[target] = p;
                }
            }
        }
        ++i;
    }
    // Cleanup
    yaml_document_delete(&doc);
    yaml_parser_delete(&parser);
}

void partfx_query(partfx_t *pfx, ParticleProps prop, void *out) {
    partfx_prop_t *p = pfx->_props[prop];
    if (p == NULL) return;

    if (p->query == CONST) {
        if (p->type == INT) {
            int *value = (int *)out;
            *value = ((partfx_cnst_t *)pfx->_props[prop])->intval;
        }
        else if (p->type == STRING) {
            char **value = (char **)out;
            *value = ((partfx_cnst_t *)pfx->_props[prop])->strval;
        }
        else if (p->type == FLOAT) {
            float *value = (float *)out;
            *value = ((partfx_cnst_t *)pfx->_props[prop])->flval;
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
