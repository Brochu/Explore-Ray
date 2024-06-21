#include "partfx.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "yaml.h"

typedef struct {
    partfx_prop_t prop;
    int val;
    //TODO: Handle different constant types (float, vectors, ...)
} partfx_cnst_t;

void partfx_init(partfx_t *pfx) {
    memset(pfx, 0, sizeof(partfx_t));
}

void partfx_parse(partfx_t *pfx, const char *data, size_t length) {
    yaml_parser_t parser;
    yaml_event_t  event;

    // Initialize parser
    if(!yaml_parser_initialize(&parser)) {
        printf("Failed to initialize parser!\n");
        exit(EXIT_FAILURE);
    }
    // Set input string
    yaml_parser_set_input_string(&parser, (unsigned char *)data, length);

    int targetIdx = -1;
    do {
        if (!yaml_parser_parse(&parser, &event)) {
            printf("Parser error %d\n", parser.error);
            exit(EXIT_FAILURE);
        }

        if (targetIdx != -1) {
            if (event.type == YAML_MAPPING_START_EVENT) {
                do {
                    yaml_parser_parse(&parser, &event);
                    yaml_parser_parse(&parser, &event);

                    partfx_cnst_t *c = malloc(sizeof(partfx_cnst_t));
                    c->prop.query = CONST;
                    c->val = atoi((char*)event.data.scalar.value + 2);
                    pfx->_props[targetIdx] = (partfx_prop_t *)c;

                    yaml_parser_parse(&parser, &event);
                    //TODO: Need to split this in functions to handle mappings + different types
                } while(event.type != YAML_MAPPING_END_EVENT);
            }
            targetIdx = -1;
        }

        if (event.type == YAML_SCALAR_EVENT) {
            if (strcmp((char *)event.data.scalar.value, "PSLT") == 0) {
                targetIdx = LIFETIME;
            }
            if (strcmp((char *)event.data.scalar.value, "MAXP") == 0) {
                targetIdx = MAX_PARTICLES;
            }
            //TODO: Add more needed parameters here
        }

        // case YAML_NO_EVENT: assert(0); break;
        // case YAML_STREAM_START_EVENT: break;
        // case YAML_STREAM_END_EVENT: break;
        // case YAML_DOCUMENT_START_EVENT: break;
        // case YAML_DOCUMENT_END_EVENT: break;
        // case YAML_SEQUENCE_START_EVENT: break;
        // case YAML_SEQUENCE_END_EVENT: break;
        // case YAML_MAPPING_START_EVENT: break;
        // case YAML_MAPPING_END_EVENT: break;
        // case YAML_ALIAS_EVENT:  printf("Got alias (anchor %s)\n", event.data.alias.anchor); break;
        // case YAML_SCALAR_EVENT: printf("Got scalar (value %s)\n", event.data.scalar.value); break;
        if(event.type != YAML_STREAM_END_EVENT) {
            yaml_event_delete(&event);
        }
    } while(event.type != YAML_STREAM_END_EVENT);
    yaml_event_delete(&event);

    // Cleanup
    yaml_parser_delete(&parser);
}

void partfx_query(partfx_t *pfx, ParticleProps prop, void *out) {
    if (pfx->_props[prop]->query == CONST) {
        int *value = (int*)out;
        *value = ((partfx_cnst_t*)pfx->_props[prop])->val;
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
