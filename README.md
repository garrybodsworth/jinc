JINC
====

A minimal single-pass zero-copy streaming JSON parser for high-performance and minimal overhead.

Why?
----

Why another JSON parser?  Most of the time a simple tokeniser is what people require, so this is what it is.  This makes a single pass of the UTF-8 encoded string and stores an array of tokens that can be used to convert the string into any output format required.  It is re-entrant so it can parse chunks of data at a time rather than requiring the whole string.

This is based off a project I wrote in C# called Jings performing the same tokenisation as it seems all parsers for that language work with reflection and its overheads.  It was pretty fast so I wondered if it would do the same for C code.  A nod to the jsmn project which served as the initial inspiration http://zserge.bitbucket.org/jsmn.html

Features
--------

* Single pass parsing
* Re-entrant so can parse one string in multiple chunks
* The token storage will expand to the necessary number of tokens

How to use
----------

Simple example:

'''
char* js = "{}";

struct TokenParsingContext context = {0};
TokenParsingContext_init(&context, 32);
enum ErrorType r = TokenParsingContext_Parse(&context, js);

if (r == SUCCESS) {
    /* Do stuff with the tokens */
}

TokenParsingContext_free(&context);
'''

TODO
----

* Lots more testing.
* More examples of how to use the tokens.
