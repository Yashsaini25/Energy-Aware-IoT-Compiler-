#include "ast.h"

// Create program node
ASTNode* create_program_node() {
    ProgramNode *node = (ProgramNode*)malloc(sizeof(ProgramNode));
    node->base.type = NODE_PROGRAM;
    node->base.line_number = 0;
    node->base.next = NULL;
    node->functions = NULL;
    return (ASTNode*)node;
}

// Create function node
ASTNode* create_function_node(char *name, DataType return_type, ASTNode *body) {
    FunctionNode *node = (FunctionNode*)malloc(sizeof(FunctionNode));
    node->base.type = NODE_FUNCTION;
    node->base.next = NULL;
    node->name = strdup(name);
    node->return_type = return_type;
    node->parameters = NULL;
    node->body = body;
    return (ASTNode*)node;
}

// Create compound statement node
ASTNode* create_compound_node() {
    CompoundNode *node = (CompoundNode*)malloc(sizeof(CompoundNode));
    node->base.type = NODE_COMPOUND;
    node->base.next = NULL;
    node->statements = NULL;
    node->statement_count = 0;
    return (ASTNode*)node;
}

// Create for loop node
ASTNode* create_for_loop_node(ASTNode *init, ASTNode *cond, ASTNode *inc, ASTNode *body) {
    ForLoopNode *node = (ForLoopNode*)malloc(sizeof(ForLoopNode));
    node->base.type = NODE_FOR_LOOP;
    node->base.next = NULL;
    node->init = init;
    node->condition = cond;
    node->increment = inc;
    node->body = body;
    node->estimated_iterations = -1;  // Unknown initially
    return (ASTNode*)node;
}

// Create while loop node
ASTNode* create_while_loop_node(ASTNode *condition, ASTNode *body) {
    WhileLoopNode *node = (WhileLoopNode*)malloc(sizeof(WhileLoopNode));
    node->base.type = NODE_WHILE_LOOP;
    node->base.next = NULL;
    node->condition = condition;
    node->body = body;
    node->estimated_iterations = -1;
    return (ASTNode*)node;
}

// Create declaration node
ASTNode* create_declaration_node(char *name, DataType type, ASTNode *init) {
    DeclarationNode *node = (DeclarationNode*)malloc(sizeof(DeclarationNode));
    node->base.type = NODE_DECLARATION;
    node->base.next = NULL;
    node->var_name = strdup(name);
    node->data_type = type;
    node->initializer = init;
    node->is_in_loop = 0;
    return (ASTNode*)node;
}

// Create assignment node
ASTNode* create_assignment_node(char *name, ASTNode *value) {
    AssignmentNode *node = (AssignmentNode*)malloc(sizeof(AssignmentNode));
    node->base.type = NODE_ASSIGNMENT;
    node->base.next = NULL;
    node->var_name = strdup(name);
    node->value = value;
    return (ASTNode*)node;
}

// Create binary operation node
ASTNode* create_binary_op_node(char *op, ASTNode *left, ASTNode *right) {
    BinaryOpNode *node = (BinaryOpNode*)malloc(sizeof(BinaryOpNode));
    node->base.type = NODE_BINARY_OP;
    node->base.next = NULL;
    node->operator = strdup(op);
    node->left = left;
    node->right = right;
    return (ASTNode*)node;
}

// Create identifier node
ASTNode* create_identifier_node(char *name) {
    IdentifierNode *node = (IdentifierNode*)malloc(sizeof(IdentifierNode));
    node->base.type = NODE_IDENTIFIER;
    node->base.next = NULL;
    node->name = strdup(name);
    return (ASTNode*)node;
}

// Create constant node
ASTNode* create_constant_node(int value) {
    ConstantNode *node = (ConstantNode*)malloc(sizeof(ConstantNode));
    node->base.type = NODE_CONSTANT;
    node->base.next = NULL;
    node->int_value = value;
    node->value_type = TYPE_INT;
    return (ASTNode*)node;
}

// Create if node
ASTNode* create_if_node(ASTNode *condition, ASTNode *then_br, ASTNode *else_br) {
    IfNode *node = (IfNode*)malloc(sizeof(IfNode));
    node->base.type = NODE_IF_STMT;
    node->base.next = NULL;
    node->condition = condition;
    node->then_branch = then_br;
    node->else_branch = else_br;
    return (ASTNode*)node;
}

// Create return node
ASTNode* create_return_node(ASTNode *value) {
    ReturnNode *node = (ReturnNode*)malloc(sizeof(ReturnNode));
    node->base.type = NODE_RETURN;
    node->base.next = NULL;
    node->return_value = value;
    return (ASTNode*)node;
}

// Add statement to compound node
void add_statement(CompoundNode *compound, ASTNode *statement) {
    if (!compound->statements) {
        compound->statements = statement;
    } else {
        ASTNode *current = compound->statements;
        while (current->next) {
            current = current->next;
        }
        current->next = statement;
    }
    compound->statement_count++;
}

// Print AST (for debugging)
void print_ast(ASTNode *node, int indent) {
    if (!node) return;
    
    for (int i = 0; i < indent; i++) printf("  ");
    
    switch (node->type) {
        case NODE_PROGRAM:
            printf("PROGRAM\n");
            print_ast(((ProgramNode*)node)->functions, indent + 1);
            break;
            
        case NODE_FUNCTION: {
            FunctionNode *func = (FunctionNode*)node;
            printf("FUNCTION: %s\n", func->name);
            print_ast(func->body, indent + 1);
            break;
        }
        
        case NODE_COMPOUND: {
            CompoundNode *comp = (CompoundNode*)node;
            printf("COMPOUND (%d statements)\n", comp->statement_count);
            ASTNode *stmt = comp->statements;
            while (stmt) {
                print_ast(stmt, indent + 1);
                stmt = stmt->next;
            }
            break;
        }
        
        case NODE_FOR_LOOP: {
            ForLoopNode *loop = (ForLoopNode*)node;
            printf("FOR LOOP\n");
            for (int i = 0; i <= indent; i++) printf("  ");
            printf("Init:\n");
            print_ast(loop->init, indent + 2);
            for (int i = 0; i <= indent; i++) printf("  ");
            printf("Condition:\n");
            print_ast(loop->condition, indent + 2);
            for (int i = 0; i <= indent; i++) printf("  ");
            printf("Increment:\n");
            print_ast(loop->increment, indent + 2);
            for (int i = 0; i <= indent; i++) printf("  ");
            printf("Body:\n");
            print_ast(loop->body, indent + 2);
            break;
        }
        
        case NODE_DECLARATION: {
            DeclarationNode *decl = (DeclarationNode*)node;
            printf("DECLARATION: %s (type: %d)%s\n", 
                   decl->var_name, decl->data_type,
                   decl->is_in_loop ? " [IN LOOP]" : "");
            if (decl->initializer) {
                print_ast(decl->initializer, indent + 1);
            }
            break;
        }
        
        case NODE_ASSIGNMENT: {
            AssignmentNode *assign = (AssignmentNode*)node;
            printf("ASSIGNMENT: %s =\n", assign->var_name);
            print_ast(assign->value, indent + 1);
            break;
        }
        
        case NODE_BINARY_OP: {
            BinaryOpNode *binop = (BinaryOpNode*)node;
            printf("BINARY OP: %s\n", binop->operator);
            print_ast(binop->left, indent + 1);
            print_ast(binop->right, indent + 1);
            break;
        }
        
        case NODE_IDENTIFIER: {
            IdentifierNode *id = (IdentifierNode*)node;
            printf("IDENTIFIER: %s\n", id->name);
            break;
        }
        
        case NODE_CONSTANT: {
            ConstantNode *constant = (ConstantNode*)node;
            printf("CONSTANT: %d\n", constant->int_value);
            break;
        }
        
        case NODE_IF_STMT: {
            IfNode *if_stmt = (IfNode*)node;
            printf("IF STATEMENT\n");
            for (int i = 0; i <= indent; i++) printf("  ");
            printf("Condition:\n");
            print_ast(if_stmt->condition, indent + 2);
            for (int i = 0; i <= indent; i++) printf("  ");
            printf("Then:\n");
            print_ast(if_stmt->then_branch, indent + 2);
            if (if_stmt->else_branch) {
                for (int i = 0; i <= indent; i++) printf("  ");
                printf("Else:\n");
                print_ast(if_stmt->else_branch, indent + 2);
            }
            break;
        }
        
        case NODE_RETURN: {
            ReturnNode *ret = (ReturnNode*)node;
            printf("RETURN");
            if (ret->return_value) {
                printf("\n");
                print_ast(ret->return_value, indent + 1);
            } else {
                printf(" (void)\n");
            }
            break;
        }
        
        case NODE_WHILE_LOOP: {
            WhileLoopNode *loop = (WhileLoopNode*)node;
            printf("WHILE LOOP\n");
            for (int i = 0; i <= indent; i++) printf("  ");
            printf("Condition:\n");
            print_ast(loop->condition, indent + 2);
            for (int i = 0; i <= indent; i++) printf("  ");
            printf("Body:\n");
            print_ast(loop->body, indent + 2);
            break;
        }
    }
}

// Free AST memory
void free_ast(ASTNode *node) {
    if (!node) return;
    
    // Free based on node type
    switch (node->type) {
        case NODE_PROGRAM:
            free_ast(((ProgramNode*)node)->functions);
            break;
        case NODE_FUNCTION:
            free(((FunctionNode*)node)->name);
            free_ast(((FunctionNode*)node)->parameters);
            free_ast(((FunctionNode*)node)->body);
            break;
        case NODE_COMPOUND: {
            CompoundNode *comp = (CompoundNode*)node;
            free_ast(comp->statements);
            break;
        }
        case NODE_FOR_LOOP: {
            ForLoopNode *loop = (ForLoopNode*)node;
            free_ast(loop->init);
            free_ast(loop->condition);
            free_ast(loop->increment);
            free_ast(loop->body);
            break;
        }
        case NODE_WHILE_LOOP: {
            WhileLoopNode *loop = (WhileLoopNode*)node;
            free_ast(loop->condition);
            free_ast(loop->body);
            break;
        }
        case NODE_DECLARATION:
            free(((DeclarationNode*)node)->var_name);
            free_ast(((DeclarationNode*)node)->initializer);
            break;
        case NODE_ASSIGNMENT:
            free(((AssignmentNode*)node)->var_name);
            free_ast(((AssignmentNode*)node)->value);
            break;
        case NODE_BINARY_OP:
            free(((BinaryOpNode*)node)->operator);
            free_ast(((BinaryOpNode*)node)->left);
            free_ast(((BinaryOpNode*)node)->right);
            break;
        case NODE_IDENTIFIER:
            free(((IdentifierNode*)node)->name);
            break;
        case NODE_IF_STMT: {
            IfNode *if_stmt = (IfNode*)node;
            free_ast(if_stmt->condition);
            free_ast(if_stmt->then_branch);
            free_ast(if_stmt->else_branch);
            break;
        }
        case NODE_RETURN:
            free_ast(((ReturnNode*)node)->return_value);
            break;
        case NODE_CONSTANT:
            break;  // No pointers to free
    }
    
    // Free next in chain
    if (node->next) {
        free_ast(node->next);
    }
    
    free(node);
}