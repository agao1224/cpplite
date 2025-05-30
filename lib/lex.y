%{
#include "parse.tab.h"   // Bison-generated token definitions
#include <string.h>
#include <stdlib.h>

extern YYSTYPE yylval;
%}

%%

"--".*                        ; // Skip single-line comments starting with --
[ \t\r\n]+                    ; // Skip whitespace

"CREATE"                     { return CREATE; }
"TABLE"                      { return TABLE; }
"INSERT"                     { return INSERT; }
"INTO"                       { return INTO; }
"VALUES"                     { return VALUES; }
"SELECT"                     { return SELECT; }
"FROM"                       { return FROM; }
"DELETE"                     { return DELETE; }
"WHERE"                      { return WHERE; }
"UPDATE"                     { return UPDATE; }
"SET"                        { return SET; }

"quit()"                     { return QUIT; }

","                          { return COMMA; }
";"                          { return SEMICOLON; }
"("                          { return LPAREN; }
")"                          { return RPAREN; }
"="                          { return EQUAL; }

[0-9]+                       {
                               yylval.str = strdup(yytext);
                               return NUMBER_LITERAL;
                             }

\"[^\"]*\"                   {
                               yylval.str = strdup(yytext + 1);
                               yylval.str[strlen(yylval.str) - 1] = '\0';
                               return STRING_LITERAL;
                             }

[a-zA-Z_][a-zA-Z0-9_]*        {
                               yylval.str = strdup(yytext);
                               return IDENTIFIER;
                             }

.                            { printf("Unexpected character: %s\n", yytext); }

%%

int yywrap() {
  return 1;
}
