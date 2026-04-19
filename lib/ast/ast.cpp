
#include "ast/ast.h"
#include "parser/SQLiteParser.h"

std::any ASTBuilder::visitCreate_table_stmt(
    SQLiteParser::Create_table_stmtContext *ctx) {
  std::string table_name = ctx->table_name()->getText();
  std::vector<ast::ColumnDef> ast_column_defs;

  for (auto column_def : ctx->column_def()) {
    ast::ColumnDef ast_col;
    ast_col.name = column_def->column_name()->getText();
    ast_col.datatype_str = column_def->type_name()->getText();

    std::vector<SQLiteParser::Column_constraintContext *> constraints;
    for (auto constraint : column_def->column_constraint()) {
      if (constraint->PRIMARY_() != nullptr)
        ast_col.is_primary_key = true;
      if (constraint->NULL_() != nullptr)
        ast_col.is_nullable = true;
    }
    ast_column_defs.push_back(ast_col);
  }

  ast::CreateTableNode create_tbl_node;
  create_tbl_node.columns = ast_column_defs;
  create_tbl_node.name = table_name;
  return create_tbl_node;
}

std::any ASTBuilder::visitDrop_stmt(SQLiteParser::Drop_stmtContext *ctx) {
  ast::DropTableNode drop_tbl_node;
  drop_tbl_node.name = ctx->any_name()->getText();
  return drop_tbl_node;
}
