#ifndef SKIBIDILANG_H
#define SKIBIDILANG_H

typedef enum {
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_OPERATOR,
    TOKEN_SEPARATOR,
    TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    char* value;
} Token;

typedef enum {
    NODE_PROGRAM,
    NODE_PRINT,
    NODE_VAR_DECL,
    NODE_NUMBER,
    NODE_STRING,
    NODE_BINARY_OP
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    char* value;
    struct ASTNode* left;
    struct ASTNode* right;
    struct ASTNode** children;
    int child_count;
} ASTNode;

Token* lex(const char* code);
ASTNode* parse(Token* tokens);
void interpret(ASTNode* node);
void free_tokens(Token* tokens);
void free_ast_node(ASTNode* node);

#endif // SKIBIDILANG_H
