#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "skibidilang.h"

#define MAX_CODE_LENGTH 10000
#define MAX_ERROR_LENGTH 100

typedef enum {
    TOKEN_IDENTIFIER,
    TOKEN_KEYWORD,
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

char* keywords[] = {"SKIBIDI", "DOP", "YES", "BRRRR", "SKIBID", "DOPE", "DOPAMINE", "BRRROOM", "SKBID"};
int keyword_count = 9;

void free_token(Token* token) {
    if (token->value) {
        free(token->value);
    }
}

void free_tokens(Token* tokens) {
    for (int i = 0; tokens[i].type != TOKEN_EOF; i++) {
        free_token(&tokens[i]);
    }
    free(tokens);
}

void free_ast_node(ASTNode* node) {
    if (node->value) {
        free(node->value);
    }
    if (node->children) {
        for (int i = 0; i < node->child_count; i++) {
            free_ast_node(node->children[i]);
        }
        free(node->children);
    }
    if (node->left) {
        free_ast_node(node->left);
    }
    if (node->right) {
        free_ast_node(node->right);
    }
    free(node);
}

void report_error(const char* message, int line, int column) {
    fprintf(stderr, "Error at line %d, column %d: %s\n", line, column, message);
    exit(1);
}

Token* create_token(TokenType type, const char* value, int length) {
    Token* token = malloc(sizeof(Token));
    token->type = type;
    token->value = malloc(length + 1);
    strncpy(token->value, value, length);
    token->value[length] = '\0';
    return token;
}

Token* lex(const char* code) {
    Token* tokens = malloc(sizeof(Token) * MAX_CODE_LENGTH);
    int token_count = 0;
    const char* token_start = code;
    const char* current = code;
    int line = 1;
    int column = 1;

    while (*current != '\0') {
        if (*current == '\n') {
            line++;
            column = 1;
            current++;
            token_start = current;
        } else if (isspace(*current)) {
            column++;
            current++;
            token_start = current;
        } else if (isalpha(*current)) {
            while (isalnum(*current)) {
                current++;
                column++;
            }
            int length = current - token_start;
            char* value = malloc(length + 1);
            strncpy(value, token_start, length);
            value[length] = '\0';

            TokenType type = TOKEN_IDENTIFIER;
            for (int i = 0; i < keyword_count; i++) {
                if (strcmp(value, keywords[i]) == 0) {
                    type = TOKEN_KEYWORD;
                    break;
                }
            }

            tokens[token_count++] = (Token){type, value};
            token_start = current;
        } else if (isdigit(*current)) {
            while (isdigit(*current)) {
                current++;
                column++;
            }
            int length = current - token_start;
            tokens[token_count++] = *create_token(TOKEN_NUMBER, token_start, length);
            token_start = current;
        } else if (*current == '"') {
            current++;
            column++;
            token_start = current;
            while (*current != '"' && *current != '\0') {
                if (*current == '\n') {
                    report_error("Unterminated string", line, column);
                }
                current++;
                column++;
            }
            if (*current == '\0') {
                report_error("Unterminated string", line, column);
            }
            int length = current - token_start;
            tokens[token_count++] = *create_token(TOKEN_STRING, token_start, length);
            current++;
            column++;
            token_start = current;
        } else if (strchr("+-*/=<>!", *current) != NULL) {
            tokens[token_count++] = *create_token(TOKEN_OPERATOR, current, 1);
            current++;
            column++;
            token_start = current;
        } else if (strchr("(){}[];,", *current) != NULL) {
            tokens[token_count++] = *create_token(TOKEN_SEPARATOR, current, 1);
            current++;
            column++;
            token_start = current;
        } else {
            char error_message[MAX_ERROR_LENGTH];
            snprintf(error_message, MAX_ERROR_LENGTH, "Unknown character '%c'", *current);
            report_error(error_message, line, column);
        }
    }

    tokens[token_count] = (Token){TOKEN_EOF, NULL};
    return tokens;
}

ASTNode* create_ast_node(ASTNodeType type, const char* value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = type;
    if (value) {
        node->value = strdup(value);
    } else {
        node->value = NULL;
    }
    node->left = NULL;
    node->right = NULL;
    node->children = NULL;
    node->child_count = 0;
    return node;
}

ASTNode* parse(Token* tokens) {
    ASTNode* root = create_ast_node(NODE_PROGRAM, NULL);
    root->children = malloc(sizeof(ASTNode*) * MAX_CODE_LENGTH);

    int current = 0;
    while (tokens[current].type != TOKEN_EOF) {
        if (tokens[current].type == TOKEN_KEYWORD) {
            if (strcmp(tokens[current].value, "SKIBIDI") == 0) {
                current++;
                if (tokens[current].type != TOKEN_STRING) {
                    report_error("Expected string after SKIBIDI", 0, 0);
                }
                ASTNode* print_node = create_ast_node(NODE_PRINT, tokens[current].value);
                root->children[root->child_count++] = print_node;
            } else if (strcmp(tokens[current].value, "DOP") == 0) {
                current++;
                if (tokens[current].type != TOKEN_IDENTIFIER) {
                    report_error("Expected identifier after DOP", 0, 0);
                }
                char* var_name = tokens[current].value;
                current++;
                if (tokens[current].type != TOKEN_OPERATOR || strcmp(tokens[current].value, "=") != 0) {
                    report_error("Expected '=' after variable name", 0, 0);
                }
                current++;
                if (tokens[current].type != TOKEN_NUMBER) {
                    report_error("Expected number after '='", 0, 0);
                }
                ASTNode* var_node = create_ast_node(NODE_VAR_DECL, var_name);
                var_node->right = create_ast_node(NODE_NUMBER, tokens[current].value);
                root->children[root->child_count++] = var_node;
            } else {
                char error_message[MAX_ERROR_LENGTH];
                snprintf(error_message, MAX_ERROR_LENGTH, "Unexpected keyword '%s'", tokens[current].value);
                report_error(error_message, 0, 0);
            }
        }
        current++;
    }

    return root;
}

void interpret(ASTNode* node) {
    switch (node->type) {
        case NODE_PROGRAM:
            for (int i = 0; i < node->child_count; i++) {
                interpret(node->children[i]);
            }
            break;
        case NODE_PRINT:
            printf("%s\n", node->value);
            break;
        case NODE_VAR_DECL:
            printf("Variable %s declared with value %s\n", node->value, node->right->value);
            break;
        default:
            fprintf(stderr, "Error: Unknown node type\n");
            exit(1);
    }
}

int main() {
    char code[MAX_CODE_LENGTH];
    printf("Enter SkibidiLang code:\n");
    fgets(code, sizeof(code), stdin);

    Token* tokens = lex(code);
    ASTNode* ast = parse(tokens);
    interpret(ast);
    
    free_tokens(tokens);
    free_ast_node(ast);

    return 0;
}