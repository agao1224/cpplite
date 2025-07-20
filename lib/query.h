#pragma once

typedef enum {
  SYSTEM_QUIT,
  CREATE_TABLE,
} ParseQueryType;

typedef enum {
  NODE_CREATE,
  NODE_INSERT,
  NODE_UPDATE,
  NODE_DELETE
} ParseNodeType;

typedef enum {
  COLUMN_INT,
  COLUMN_TEXT,
  COLUMN_BOOL
} ParseColumnType;

typedef struct ParseColumnDef ParseColumnDef_t;
typedef struct ParseColumn ParseColumn_t;

typedef struct ParseColumnDef {
  char *name;
  ParseColumnType column_type; 
  ParseColumnDef_t *next_column_def;
} ParseColumnDef_t;

typedef struct ParseColumn {
  char *name;
  ParseColumn_t *next_column;
} ParseColumn_t;

typedef struct ParseQuery {
  ParseQueryType query_type;
  char *table_name;
  union { 
    ParseColumnDef_t *column_def; 
    ParseColumn_t *column;
  } query_columns;
} ParseQuery_t;
