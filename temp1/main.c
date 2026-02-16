#include <stdio.h>
#include "ast.h"

extern ASTNode *root;
extern int yyparse();

int main() {
    if (yyparse() == 0) {
        printf("Parse successful!\n");
        if (root) {
            printf("AST root type: %d\n", root->type);
            print_ast(root, 0);
            free_ast(root);
        }
    } else {
        printf("Parse failed!\n");
        return 1;
    }
    return 0;
}
