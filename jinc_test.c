#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jinc.h"
#include "test_helpers.h"

#define CHECK_EQ(t, tok_start, tok_end, tok_type) \
    ((t).start == tok_start \
     && (t).end == tok_end  \
     && (t).type == (tok_type))

#define CHECK_STRING(js, t, s) \
    (strncmp(js+(t).start, s, (t).end - (t).start) == 0 \
     && strlen(s) == (t).end - (t).start)

#define TOKEN_PRINT(t) \
    printf("start: %d, end: %d, type: %d, size: %d\n", \
            (t).start, (t).end, (t).type, (t).size)

int test_empty_dict()
{
    const char *js = "{}";

    struct TokenParsingContext context = {0};
    TokenParsingContext_init(&context, 32);
    enum ErrorType r = TokenParsingContext_Parse(&context, js);

    check(r == SUCCESS);
    check(context.tokens[0].type == TOKEN_OBJECT);
    check(context.tokens[0].start == 0 && context.tokens[0].end == 2);

    TokenParsingContext_free(&context);

    return 0;
}

int test_empty_array()
{
    const char *js = "[]";

    struct TokenParsingContext context = {0};
    TokenParsingContext_init(&context, 32);
    enum ErrorType r = TokenParsingContext_Parse(&context, js);

    check(r == SUCCESS);
    check(context.tokens[0].type == TOKEN_ARRAY);
    check(context.tokens[0].start == 0 && context.tokens[0].end == 2);

    TokenParsingContext_free(&context);

    return 0;
}

int test_empty_dict_2()
{
    const char *js = "{\"a\":[]}";

    struct TokenParsingContext context = {0};
    TokenParsingContext_init(&context, 32);
    enum ErrorType r = TokenParsingContext_Parse(&context, js);

    check(r == SUCCESS);
    check(context.tokens[0].type == TOKEN_OBJECT && context.tokens[0].start == 0 && context.tokens[0].end == 8);
    check(context.tokens[1].type == TOKEN_STRING && context.tokens[1].start == 2 && context.tokens[1].end == 3);
    check(context.tokens[2].type == TOKEN_ARRAY && context.tokens[2].start == 5 && context.tokens[2].end == 7);

    TokenParsingContext_free(&context);

    return 0;
}

int test_empty_array_2()
{
    const char *js = "[{},{}]";

    struct TokenParsingContext context = {0};
    TokenParsingContext_init(&context, 32);
    enum ErrorType r = TokenParsingContext_Parse(&context, js);

    check(r == SUCCESS);
    check(context.tokens[0].type == TOKEN_ARRAY && context.tokens[0].start == 0 && context.tokens[0].end == 7);
    check(context.tokens[1].type == TOKEN_OBJECT && context.tokens[1].start == 1 && context.tokens[1].end == 3);
    check(context.tokens[2].type == TOKEN_OBJECT && context.tokens[2].start == 4 && context.tokens[2].end == 6);

    TokenParsingContext_free(&context);

    return 0;
}

int test_simple_1()
{
    const char *js = "{\"a\": 0}";

    struct TokenParsingContext context = {0};
    TokenParsingContext_init(&context, 32);
    enum ErrorType r = TokenParsingContext_Parse(&context, js);

    check(r == SUCCESS);
    check(CHECK_EQ(context.tokens[0], 0, 8, TOKEN_OBJECT));
    check(CHECK_EQ(context.tokens[1], 2, 3, TOKEN_STRING));
    check(CHECK_EQ(context.tokens[2], 6, 7, TOKEN_PRIMITIVE));

    check(CHECK_STRING(js, context.tokens[0], js));
    check(CHECK_STRING(js, context.tokens[1], "a"));
    check(CHECK_STRING(js, context.tokens[2], "0"));

    return 0;
}

int test_simple_2()
{
    const char *js = "[\"a\":{},\"b\":{}]";

    struct TokenParsingContext context = {0};
    TokenParsingContext_init(&context, 32);
    enum ErrorType r = TokenParsingContext_Parse(&context, js);

    check(r == SUCCESS);
    check(CHECK_STRING(js, context.tokens[0], js));

    return 0;
}

int test_simple_3()
{
    const char *js = "{\n \"Day\": 26,\n \"Month\": 9,\n \"Year\": 12\n }";

    struct TokenParsingContext context = {0};
    TokenParsingContext_init(&context, 32);
    enum ErrorType r = TokenParsingContext_Parse(&context, js);

    check(r == SUCCESS);
    check(CHECK_STRING(js, context.tokens[0], js));

    return 0;
}

int main()
{
    test(test_empty_dict, "general test for a empty JSON objects/arrays");
    test(test_empty_array, "general test for a empty JSON objects/arrays");
    test(test_empty_dict_2, "general test for a empty JSON objects/arrays");
    test(test_empty_array_2, "general test for a empty JSON objects/arrays");
    test(test_simple_1, "general test for a simple JSON string");
    test(test_simple_2, "general test for a simple JSON string");
    test(test_simple_3, "general test for a simple JSON string");
    printf("\nPASSED: %d\nFAILED: %d\n", test_passed, test_failed);
    return 0;
}

