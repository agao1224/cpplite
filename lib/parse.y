%code requires {
  #include "query.h"
}

%code {
  ParseQuery_t* parse_query = NULL;
}

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void yyerror(const char* s);
int yylex(void);
%}

%union {
  char* str;
  ParseColumnDef_t* column_def;
}

%token CREATE TABLE INSERT INTO VALUES SELECT FROM DELETE WHERE UPDATE SET
%token IDENTIFIER STRING_LITERAL NUMBER_LITERAL
%token COMMA SEMICOLON LPAREN RPAREN EQUAL
%token QUIT
%token COLUMN_TYPE

%type <str> IDENTIFIER STRING_LITERAL NUMBER_LITERAL
%type <column_def> column_def column_defs
%%

program:
  statements
  ;

statements:
  statements statement
  | statement
  ;

statement:
  create_stmt SEMICOLON
  | insert_stmt SEMICOLON
  | select_stmt SEMICOLON
  | delete_stmt SEMICOLON
  | update_stmt SEMICOLON
  | system_stmt 
  ;

system_stmt:
  QUIT
  ;

create_stmt:
  CREATE TABLE IDENTIFIER LPAREN column_defs RPAREN
    { 
      printf("CREATE TABLE %s\n", $3);
      parse_query = (ParseQuery_t*) malloc(sizeof(ParseQuery_t));
      parse_query->query_type = CREATE_TABLE;
      parse_query->table_name = $3;
      parse_query->query_columns.column_def = $5;
    }
  ;

column_defs:
  column_defs COMMA column_def
  {
    $1->next_column_def = $3;
  }
  | column_def
  {
    $$ = $1;
  }
  ;

column_def:
  IDENTIFIER IDENTIFIER   // name and type
    { 
      #ifdef DEBUG
      printf("Column %s of type %s\n", $1, $2); 
      #endif

      ParseColumnDef_t* column_def = (ParseColumnDef_t*) malloc(sizeof(ParseColumnDef_t));
      column_def->name = $1;
      if (strcmp($2, "INT") == 0) {
        column_def->column_type = COLUMN_INT;
      } else if (strcmp($2, "TEXT") == 0) {
        column_def->column_type = COLUMN_TEXT;
      } else if (strcmp($2, "BOOL") == 0) {
        column_def->column_type = COLUMN_BOOL;
      }
      $$ = column_def;
    }
  ;

insert_stmt:
  INSERT INTO IDENTIFIER VALUES LPAREN value_list RPAREN
    { printf("INSERT INTO %s\n", $3); }
  ;

value_list:
  value_list COMMA value
  | value
  ;

value:
  STRING_LITERAL
  | NUMBER_LITERAL
  ;

select_stmt:
  SELECT select_list FROM IDENTIFIER
    { printf("SELECT from %s\n", $4); }
  ;

select_list:
  select_list COMMA IDENTIFIER
  | IDENTIFIER
  ;

delete_stmt:
  DELETE FROM IDENTIFIER where_clause_opt
    { printf("DELETE FROM %s\n", $3); }
  ;

update_stmt:
  UPDATE IDENTIFIER SET assignment_list where_clause_opt
    { printf("UPDATE %s\n", $2); }
  ;

assignment_list:
  assignment_list COMMA assignment
  | assignment
  ;

assignment:
  IDENTIFIER EQUAL value
    { printf("SET %s = ...\n", $1); }
  ;

where_clause_opt:
  WHERE condition
  |
  ;

condition:
  IDENTIFIER EQUAL value
    { printf("WHERE %s = ...\n", $1); }
  ;

%%

void yyerror(const char* s) {
  fprintf(stderr, "Parse error: %s\n", s);
}
