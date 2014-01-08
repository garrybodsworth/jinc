#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jinc.h"
#include "greatest.h"

SUITE(jinc_suite);

#define CHECK_EQ(t, tok_start, tok_end, tok_type) \
    ((t).start == tok_start \
     && (t).end == tok_end  \
     && (t).type == (tok_type))

#define CHECK_STRING(js, t, s) \
    (strncmp(js+(t).start, s, (t).end - (t).start) == 0 \
     && strlen(s) == (t).end - (t).start)

TEST test_empty_dict(void)
{
    const char *js = "{}";

    struct TokenParsingContext context = {0};
    TokenParsingContext_init(&context, 32);
    enum ErrorType r = TokenParsingContext_Parse(&context, js);

    ASSERT(r == SUCCESS);
    ASSERT(context.tokens[0].type == TOKEN_OBJECT);
    ASSERT(context.tokens[0].start == 0 && context.tokens[0].end == 2);

    TokenParsingContext_free(&context);

    PASS();
}

TEST test_empty_array(void)
{
    const char *js = "[]";

    struct TokenParsingContext context = {0};
    TokenParsingContext_init(&context, 32);
    enum ErrorType r = TokenParsingContext_Parse(&context, js);

    ASSERT(r == SUCCESS);
    ASSERT(context.tokens[0].type == TOKEN_ARRAY);
    ASSERT(context.tokens[0].start == 0 && context.tokens[0].end == 2);

    TokenParsingContext_free(&context);

    PASS();
}

TEST test_empty_dict_2(void)
{
    const char *js = "{\"a\":[]}";

    struct TokenParsingContext context = {0};
    TokenParsingContext_init(&context, 32);
    enum ErrorType r = TokenParsingContext_Parse(&context, js);

    ASSERT(r == SUCCESS);
    ASSERT(context.tokens[0].type == TOKEN_OBJECT && context.tokens[0].start == 0 && context.tokens[0].end == 8);
    ASSERT(context.tokens[1].type == TOKEN_STRING && context.tokens[1].start == 2 && context.tokens[1].end == 3);
    ASSERT(context.tokens[2].type == TOKEN_ARRAY && context.tokens[2].start == 5 && context.tokens[2].end == 7);

    TokenParsingContext_free(&context);

    PASS();
}

TEST test_empty_array_2(void)
{
    const char *js = "[{},{}]";

    struct TokenParsingContext context = {0};
    TokenParsingContext_init(&context, 32);
    enum ErrorType r = TokenParsingContext_Parse(&context, js);

    ASSERT(r == SUCCESS);
    ASSERT(context.tokens[0].type == TOKEN_ARRAY && context.tokens[0].start == 0 && context.tokens[0].end == 7);
    ASSERT(context.tokens[1].type == TOKEN_OBJECT && context.tokens[1].start == 1 && context.tokens[1].end == 3);
    ASSERT(context.tokens[2].type == TOKEN_OBJECT && context.tokens[2].start == 4 && context.tokens[2].end == 6);

    TokenParsingContext_free(&context);

    PASS();
}

TEST test_simple_1(void)
{
    const char *js = "{\"a\": 0}";

    struct TokenParsingContext context = {0};
    TokenParsingContext_init(&context, 32);
    enum ErrorType r = TokenParsingContext_Parse(&context, js);

    ASSERT(r == SUCCESS);
    ASSERT(CHECK_EQ(context.tokens[0], 0, 8, TOKEN_OBJECT));
    ASSERT(CHECK_EQ(context.tokens[1], 2, 3, TOKEN_STRING));
    ASSERT(CHECK_EQ(context.tokens[2], 6, 7, TOKEN_PRIMITIVE));

    ASSERT(CHECK_STRING(js, context.tokens[0], js));
    ASSERT(CHECK_STRING(js, context.tokens[1], "a"));
    ASSERT(CHECK_STRING(js, context.tokens[2], "0"));

    PASS();
}

TEST test_simple_2(void)
{
    const char *js = "[\"a\":{},\"b\":{}]";

    struct TokenParsingContext context = {0};
    TokenParsingContext_init(&context, 32);
    enum ErrorType r = TokenParsingContext_Parse(&context, js);

    ASSERT(r == SUCCESS);
    ASSERT(CHECK_STRING(js, context.tokens[0], js));

    PASS();
}

TEST test_simple_3(void)
{
    const char *js = "{\n \"Day\": 26,\n \"Month\": 9,\n \"Year\": 12\n }";

    struct TokenParsingContext context = {0};
    TokenParsingContext_init(&context, 32);
    enum ErrorType r = TokenParsingContext_Parse(&context, js);

    ASSERT(r == SUCCESS);
    ASSERT(CHECK_STRING(js, context.tokens[0], js));

    PASS();
}

GREATEST_SUITE(jinc_suite)
{
    RUN_TEST(test_empty_dict);
    RUN_TEST(test_empty_array);
    RUN_TEST(test_empty_dict_2);
    RUN_TEST(test_empty_array_2);
    RUN_TEST(test_simple_1);
    RUN_TEST(test_simple_2);
    RUN_TEST(test_simple_3);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();      /* command-line arguments, initialization. */
    RUN_SUITE(jinc_suite);
    GREATEST_MAIN_END();        /* display results */
}

