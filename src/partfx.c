#include "partfx.h"

#include <assert.h>
#include <stdio.h>
#include "yaml.h"

void partfx_init(partfx_t *pfx) {
    memset(pfx, 0, sizeof(partfx_t));
}

void partfx_parse(partfx_t *pfx, const char *data, size_t length) {
    yaml_parser_t parser;
    yaml_event_t  event;

    // Initialize parser
    if(!yaml_parser_initialize(&parser)) {
        fputs("Failed to initialize parser!\n", stderr);
    }
    // Set input string
    yaml_parser_set_input_string(&parser, (unsigned char *)data, length);

    void *target = NULL;
    do {
        if (!yaml_parser_parse(&parser, &event)) {
            printf("Parser error %d\n", parser.error);
            exit(EXIT_FAILURE);
        }

        if (target != NULL) {
            if (event.type == YAML_MAPPING_START_EVENT) {
                do {
                    yaml_parser_parse(&parser, &event);
                    yaml_parser_parse(&parser, &event);
                    int *val = (int*)target;
                    *val = atoi((char*)event.data.scalar.value + 2); //WOW, check how to parse hex values

                    yaml_parser_parse(&parser, &event);
                    //TODO: Need to split this in functions to handle mappings + different types
                } while(event.type != YAML_MAPPING_END_EVENT);
            }
            target = NULL;
        }

        if (event.type == YAML_SCALAR_EVENT && strcmp((char *)event.data.scalar.value, "PSLT") == 0) {
            target = &pfx->lifetime;
        }
        //TODO: Add more needed parameters here

        //switch(event.type)
        //{ 
        //    case YAML_NO_EVENT: assert(0); break;
        //    // Stream start/end - Do we need these events?
        //    case YAML_STREAM_START_EVENT: break;
        //    case YAML_STREAM_END_EVENT: break;
        //    // Block delimeters
        //    case YAML_DOCUMENT_START_EVENT: break;
        //    case YAML_DOCUMENT_END_EVENT: break;
        //    case YAML_SEQUENCE_START_EVENT: puts("<b>Start Sequence</b>"); break;
        //    case YAML_SEQUENCE_END_EVENT:   puts("<b>End Sequence</b>");   break;
        //    case YAML_MAPPING_START_EVENT:  puts("<b>Start Mapping</b>");  break;
        //    case YAML_MAPPING_END_EVENT:    puts("<b>End Mapping</b>");    break;
        //    // Data
        //    case YAML_ALIAS_EVENT:  printf("Got alias (anchor %s)\n", event.data.alias.anchor); break;
        //    case YAML_SCALAR_EVENT: printf("Got scalar (value %s)\n", event.data.scalar.value); break;
        //}
        if(event.type != YAML_STREAM_END_EVENT) {
            yaml_event_delete(&event);
        }
    } while(event.type != YAML_STREAM_END_EVENT);
    yaml_event_delete(&event);

    // Cleanup
    yaml_parser_delete(&parser);
}

void partfx_delete(partfx_t *pfx) {
    //TODO: Cleanup resources
    memset(pfx, 0, sizeof(partfx_t));
}
