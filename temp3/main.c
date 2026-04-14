#include <stdio.h>
#include "ast.h"

extern int yyparse();
extern Node *root;

int main() {
    if (yyparse() == 0) {
        printf("Parse successful!\n");
        if (root) {
            print_ast(root, 0);
        }
    } else {
        printf("Parse failed!\n");
        return 1;
    }
    return 0;
}