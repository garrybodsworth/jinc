#include <stddef.h>
#include <stdlib.h>

#include "jinc.h"

/* Whether the array of tokens needs resizing */
#define NEED_RESIZE(context) ((context)->length >= (context)->capacity)

/* Resize the token storage array */
enum ErrorType TokenParsingContext_Resize(struct TokenParsingContext* context);

/* Initialise the parsing structure. */
void TokenParsingContext_init(struct TokenParsingContext* context, size_t initial_capacity)
{
    context->position = 0;
    context->offset = 0;
    context->parse_start = 0;
    context->parse_start_offset = 0;
    context->parse_token_type = TOKEN_NONE;
    context->capacity = 0;
    context->length = 0;

    context->initial_capacity = initial_capacity;
    context->token_super = -1;
}

/* Free any dynamic allocations in the parsing structure */
void TokenParsingContext_free(struct TokenParsingContext* context)
{
    /* Clear the tokens in the list */
    if (context->tokens != NULL) {
        free(context->tokens);
        context->tokens = NULL;
    }
}

/* Resize the token storage array */
enum ErrorType TokenParsingContext_Resize(struct TokenParsingContext* context)
{
    size_t new_capacity = (context->capacity < context->initial_capacity) ? context->initial_capacity : context->capacity * 2;
    struct Token* new_items = (struct Token*)realloc(context->tokens, new_capacity * sizeof(struct Token));
    if (new_items == NULL) {
        return ERROR_ALLOCATION_FAILED;
    }
    context->tokens = new_items;
    context->capacity = new_capacity;
    return SUCCESS;
}

/* Parse the input json string */
enum ErrorType TokenParsingContext_Parse(struct TokenParsingContext* context, const char* json)
{
    if (json == NULL) {
        return ERROR_INCOMPLETE_JSON;
    }

    if (context->position != 0) {
        /* We are re-entering the parser with new data */
        context->offset += context->position;
        context->position = 0;
    }

    for (; json[context->position] != '\0'; context->position++) {
        char c = json[context->position];

        /* We are in the process of parsing some token */
        if (context->parse_token_type == TOKEN_STRING || context->parse_token_type == TOKEN_UNICODE) {
            /* Quote: end of string */
            if (c == '\"') {
                if (NEED_RESIZE(context)) {
                    enum ErrorType error_type = TokenParsingContext_Resize(context);
                    if (error_type != SUCCESS) {
                        return error_type;
                    }
                }
                context->tokens[context->length].type = context->parse_token_type;
                context->tokens[context->length].start = context->parse_start_offset + context->parse_start;
                context->tokens[context->length].end = context->offset + context->position;
                context->tokens[context->length].size = 0;
                context->tokens[context->length++].parent = context->token_super;

                if (context->token_super != -1) {
                    context->tokens[context->token_super].size++;
                }
                context->parse_start = 0;
                context->parse_start_offset = 0;
                context->parse_token_type = TOKEN_NONE;
                continue;
            }

            /* Backslash: Quoted symbol expected */
            if (c == '\\') {
                switch (json[++context->position]) {
                    /* Allowed escaped symbols */
                    case '\"': case '/': case '\\': case 'b':
                    case 'f': case 'r': case 'n': case 't':
                        break;
                    /* Allows escaped symbol \uXXXX */
                    case 'u':
                        context->parse_token_type = TOKEN_UNICODE;
                        break;
                    /* Unexpected symbol */
                    default:
                        context->position = context->parse_start;
                        return ERROR_INVALID_CHAR;
                }
            }

            continue;
        }

        if (context->parse_token_type == TOKEN_PRIMITIVE) {
            switch (c) {
                case '\t': case '\r': case '\n': case ' ':
                case ',': case ']': case '}':
                    {
                        if (NEED_RESIZE(context)) {
                            enum ErrorType error_type = TokenParsingContext_Resize(context);
                            if (error_type != SUCCESS) {
                                return error_type;
                            }
                        }
                        context->tokens[context->length].type = context->parse_token_type;
                        context->tokens[context->length].start = context->parse_start_offset + context->parse_start;
                        context->tokens[context->length].end = context->offset + context->position;
                        context->tokens[context->length].size = 0;
                        context->tokens[context->length++].parent = context->token_super;

                        context->parse_start = 0;
                        context->parse_start_offset = 0;
                        context->parse_token_type = TOKEN_NONE;

                        if (context->token_super != -1) {
                            context->tokens[context->token_super].size++;
                        }

                        context->position--;
                    }
                    break;
                default:
                    if (c < 32 || c >= 127) {
                        context->position = context->parse_start;
                        return ERROR_INVALID_CHAR;
                    }
                    break;
            }

            continue;
        }

        switch (c)
        {
            case '{': case '[':
                {
                    int tokparent = -1;
                    if (context->token_super != -1) {
                        context->tokens[context->token_super].size++;
                        tokparent = context->token_super;
                    }

                    if (NEED_RESIZE(context)) {
                        enum ErrorType error_type = TokenParsingContext_Resize(context);
                        if (error_type != SUCCESS) {
                            return error_type;
                        }
                    }
                    context->tokens[context->length].type = (c == '{' ? TOKEN_OBJECT : TOKEN_ARRAY);
                    context->tokens[context->length].start = context->offset + context->position;
                    context->tokens[context->length].end = -1;
                    context->tokens[context->length].size = 0;
                    context->tokens[context->length++].parent = tokparent;

                    context->token_super = context->length - 1;
                }
                break;
            case '}': case ']':
                if (context->length < 1) {
                    return ERROR_INVALID_CHAR;
                }
                int token = context->length - 1;
                while (1) {
                    struct Token* currentToken = &(context->tokens[token]);
                    if (currentToken->start != -1 && currentToken->end == -1) {
                        if (currentToken->type != (c == '}' ? TOKEN_OBJECT : TOKEN_ARRAY)) {
                            return ERROR_INVALID_CHAR;
                        }
                        currentToken->end = context->offset + context->position + 1;
                        context->token_super = currentToken->parent;
                        break;
                    }
                    if (currentToken->parent == -1) {
                        break;
                    }
                    token = currentToken->parent;
                }
                break;
            case '\"':
                context->parse_token_type = TOKEN_STRING;
                /* Skip starting quote */
                context->parse_start = context->position + 1;
                context->parse_start_offset = context->offset;
                break;
            case '\t': case '\r': case '\n': case ':':
            case ',': case ' ':
                break;
            /* Primitives are numbers and booleans */
            case '-': case '0': case '1': case '2':
            case '3': case '4': case '5': case '6':
            case '7': case '8': case '9': case 't':
            case 'f': case 'n':
                context->parse_token_type = TOKEN_PRIMITIVE;
                context->parse_start = context->position;
                context->parse_start_offset = context->offset;
                break;
            /* Unexpected char */
            default:
                return ERROR_INVALID_CHAR;
        }
    }

    for (int i ; i < context->length ; i++) {
        /* Unmatched opened object or array */
        if (context->tokens[i].end == -1) {
            return ERROR_INCOMPLETE_JSON;
        }
    }

    return SUCCESS;
}
