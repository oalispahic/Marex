%{
#include <stdio.h>
#include <stdlib.h>
%}

%token VAR PRINT IF ELSE FI IDENT NUMBER
%left '+' '-'
%left '*' '/'

%%
program:
      /* empty */
    | program statement
    ;

statement:
      var_decl
    | assign
    | print_stmt
    | if_stmt
    | expr
    ;

var_decl:
      VAR IDENT ":=" expr
    ;

assign:
      IDENT ":=" expr
    ;

print_stmt:
      PRINT '(' expr ')'
    ;

if_stmt:
      IF '(' expr ')' statement FI
    | IF '(' expr ')' statement ELSE statement FI
    ;

expr:
      term
    | expr '+' term
    | expr '-' term
    ;

term:
      factor
    | term '*' factor
    | term '/' factor
    ;

factor:
      NUMBER
    | IDENT
    | '(' expr ')'
    ;

%%

int main(void) {
    return yyparse();
}

int yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
    return 0;
}

