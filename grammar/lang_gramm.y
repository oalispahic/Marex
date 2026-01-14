%{
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
void yyerror(const char *s);
%}

/* ===== TOKENS ===== */
%token IDENT NUMBER STRING

%token NEW PRINT IF ELSE FI LOOP DONE

%token ASSIGN_OP      /* := */
%token OR AND
%token EQ NEQ         /* == != */
%token LT GT          /* < > */
%token ARROW          /* -> */

/* ===== PRECEDENCE ===== */
%left OR
%left AND
%left EQ NEQ
%left LT GT
%left '+' '-'
%left '*' '/'
%right UMINUS

%start program

%%

/* ===== PROGRAM ===== */

program
    : stmt_list
    ;

/* ===== STATEMENTS ===== */

stmt_list
    : /* empty */
    | stmt_list statement
    ;

statement
    : var_decl ';'
    | assign ';'
    | print_stmt ';'
    | expr_stmt ';'
    | if_stmt
    | loop_stmt
    ;

/* ===== DECLARATIONS & ASSIGNMENT ===== */

var_decl
    : NEW IDENT ASSIGN_OP expr
    ;

assign
    : IDENT ASSIGN_OP expr
    ;

/* ===== PRINT ===== */

print_stmt
    : PRINT '(' expr ')'
    ;

/* ===== IF ===== */

if_stmt
    : IF '(' expr ')' stmt_list FI
    | IF '(' expr ')' stmt_list ELSE stmt_list FI
    ;

/* ===== LOOPS ===== */

loop_stmt
    : for_loop
    | range_loop
    ;

range_loop
    : LOOP '(' IDENT ARROW IDENT ')' stmt_list DONE
    ;

for_loop
    : LOOP '(' opt_var_decl ';' expr ';' assign ')' stmt_list DONE
    ;

opt_var_decl
    : /* empty */
    | var_decl
    ;

/* ===== EXPRESSIONS ===== */

expr
    : logic_or
    ;

logic_or
    : logic_and
    | logic_or OR logic_and
    ;

logic_and
    : equality
    | logic_and AND equality
    ;

equality
    : comparison
    | equality EQ comparison
    | equality NEQ comparison
    ;

comparison
    : term
    | comparison LT term
    | comparison GT term
    ;

term
    : factor
    | term '+' factor
    | term '-' factor
    ;

factor
    : unary
    | factor '*' unary
    | factor '/' unary
    ;

unary
    : '-' unary %prec UMINUS
    | primary
    ;

primary
    : NUMBER
    | STRING
    | IDENT
    | '(' expr ')'
    ;

/* ===== EXPRESSION STATEMENT ===== */

expr_stmt
    : expr
    ;

%%

void yyerror(const char *s)
{
    fprintf(stderr, "Parse error: %s\n", s);
}
