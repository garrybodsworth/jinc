#ifndef __JINC_H_
#define __JINC_H_

/* The error types that can be emitted from the parser */
enum ErrorType
{
    /* Invalid character inside JSON string */
    ERROR_INVALID_CHAR = -1,
    /* The string is not a full JSON packet, more bytes expected */
    ERROR_INCOMPLETE_JSON = -2,
    /* Unable to allocate or resize token storage */
    ERROR_ALLOCATION_FAILED = -3,
    /* Everything was fine */
    SUCCESS = 0
};

/* The types of tokens that can be parsed. */
enum TokenType
{
    /* No value (default) */
    TOKEN_NONE = 0,
    /* A JSON primitive (eg, number, bool, null) */
    TOKEN_PRIMITIVE = 1,
    /* A JSON object (a dictionary) */
    TOKEN_OBJECT = 2,
    /* A JSON array of items */
    TOKEN_ARRAY = 3,
    /* A JSON string */
    TOKEN_STRING = 4,
    /* A unicode string */
    TOKEN_UNICODE = 5,
};

/* Stored a parsed token by indexes into the string
 * Preferred default values listed after the colon (as it is a struct and ends up with null values)
 */
struct Token
{
    /* Type of token : TOKEN_NONE */
    enum TokenType type;
    /* Start index of token : -1 */
    int start;
    /* End index of token : -1 */
    int end;
    /* Number of sub-elements in the token : 0 */
    int size;
    /* Index into the token array of the parent of this item (if it is a child) : -1 */
    int parent;
};

/* This structure stores the working information of the parser. */
struct TokenParsingContext
{
    /* Current position of the parsing in the array */
    int position;
    /* Offset if it is continuing the parsing of a stream */
    int offset;

    /* The current "parent" or super token */
    int token_super;

    /* For an item like a primitive or string, the index that the parsing started. */
    int parse_start;
    /* The offset value of the index the parsing started. */
    int parse_start_offset;
    /* The type of token that is currently in the process of being parsed. */
    enum TokenType parse_token_type;

    /* The initial capacity of the token list. */
    size_t initial_capacity;
    /* An array of token structs. */
    struct Token* tokens;
    /* Current capacity of the list. */
    size_t capacity;
    /* The next free token in the array of tokens */
    size_t length;
};

/* Initialise the parsing structure. */
void TokenParsingContext_init(struct TokenParsingContext* context, size_t initial_capacity);
/* Free any dynamic allocations in the parsing structure */
void TokenParsingContext_free(struct TokenParsingContext* context);
/* Parse the input json string */
enum ErrorType TokenParsingContext_Parse(struct TokenParsingContext* context, const char* json);

#endif /* __JINC_H_ */
