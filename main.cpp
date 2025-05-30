#include <cstdio>
#include <stdio.h>
#include "lib/query.h"

extern int yyparse();
extern void yyrestart(FILE*);
extern void yy_switch_to_buffer(struct yy_buffer_state*);
extern struct yy_buffer_state* yy_scan_string(const char*);
extern void yy_delete_buffer(struct yy_buffer_state*);
extern ParseQuery_t* parse_query;

int main() {
  char in[1024];

  while (true) {
    printf("cpplite> ");
    fflush(stdout);
    if (fgets(in, sizeof(in), stdin) == NULL) break;

    if (in[0] == '\n') continue;

    struct yy_buffer_state* buffer = yy_scan_string(in);
    if (yyparse() != 0) {
      fprintf(stderr, "Parse error\n");
    } else {
      printf("parse_query->query_type: %s\n", parse_query->table_name);
    }
    yy_delete_buffer(buffer);
  } 
  return 0;
}
