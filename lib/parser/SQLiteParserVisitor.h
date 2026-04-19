
// Generated from grammar/SQLiteParser.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "SQLiteParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by SQLiteParser.
 */
class  SQLiteParserVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by SQLiteParser.
   */
    virtual std::any visitParse(SQLiteParser::ParseContext *context) = 0;

    virtual std::any visitSql_stmt_list(SQLiteParser::Sql_stmt_listContext *context) = 0;

    virtual std::any visitSql_stmt(SQLiteParser::Sql_stmtContext *context) = 0;

    virtual std::any visitAlter_table_stmt(SQLiteParser::Alter_table_stmtContext *context) = 0;

    virtual std::any visitAnalyze_stmt(SQLiteParser::Analyze_stmtContext *context) = 0;

    virtual std::any visitAttach_stmt(SQLiteParser::Attach_stmtContext *context) = 0;

    virtual std::any visitBegin_stmt(SQLiteParser::Begin_stmtContext *context) = 0;

    virtual std::any visitCommit_stmt(SQLiteParser::Commit_stmtContext *context) = 0;

    virtual std::any visitRollback_stmt(SQLiteParser::Rollback_stmtContext *context) = 0;

    virtual std::any visitSavepoint_stmt(SQLiteParser::Savepoint_stmtContext *context) = 0;

    virtual std::any visitRelease_stmt(SQLiteParser::Release_stmtContext *context) = 0;

    virtual std::any visitCreate_index_stmt(SQLiteParser::Create_index_stmtContext *context) = 0;

    virtual std::any visitIndexed_column(SQLiteParser::Indexed_columnContext *context) = 0;

    virtual std::any visitCreate_table_stmt(SQLiteParser::Create_table_stmtContext *context) = 0;

    virtual std::any visitTable_options(SQLiteParser::Table_optionsContext *context) = 0;

    virtual std::any visitColumn_def(SQLiteParser::Column_defContext *context) = 0;

    virtual std::any visitType_name(SQLiteParser::Type_nameContext *context) = 0;

    virtual std::any visitColumn_constraint(SQLiteParser::Column_constraintContext *context) = 0;

    virtual std::any visitSigned_number(SQLiteParser::Signed_numberContext *context) = 0;

    virtual std::any visitTable_constraint(SQLiteParser::Table_constraintContext *context) = 0;

    virtual std::any visitForeign_key_clause(SQLiteParser::Foreign_key_clauseContext *context) = 0;

    virtual std::any visitConflict_clause(SQLiteParser::Conflict_clauseContext *context) = 0;

    virtual std::any visitCreate_trigger_stmt(SQLiteParser::Create_trigger_stmtContext *context) = 0;

    virtual std::any visitCreate_view_stmt(SQLiteParser::Create_view_stmtContext *context) = 0;

    virtual std::any visitCreate_virtual_table_stmt(SQLiteParser::Create_virtual_table_stmtContext *context) = 0;

    virtual std::any visitWith_clause(SQLiteParser::With_clauseContext *context) = 0;

    virtual std::any visitCommon_table_expression(SQLiteParser::Common_table_expressionContext *context) = 0;

    virtual std::any visitCte_table_name(SQLiteParser::Cte_table_nameContext *context) = 0;

    virtual std::any visitDelete_stmt(SQLiteParser::Delete_stmtContext *context) = 0;

    virtual std::any visitDetach_stmt(SQLiteParser::Detach_stmtContext *context) = 0;

    virtual std::any visitDrop_stmt(SQLiteParser::Drop_stmtContext *context) = 0;

    virtual std::any visitExpr(SQLiteParser::ExprContext *context) = 0;

    virtual std::any visitExpr_or(SQLiteParser::Expr_orContext *context) = 0;

    virtual std::any visitExpr_and(SQLiteParser::Expr_andContext *context) = 0;

    virtual std::any visitExpr_not(SQLiteParser::Expr_notContext *context) = 0;

    virtual std::any visitExpr_binary(SQLiteParser::Expr_binaryContext *context) = 0;

    virtual std::any visitExpr_comparison(SQLiteParser::Expr_comparisonContext *context) = 0;

    virtual std::any visitExpr_bitwise(SQLiteParser::Expr_bitwiseContext *context) = 0;

    virtual std::any visitExpr_addition(SQLiteParser::Expr_additionContext *context) = 0;

    virtual std::any visitExpr_multiplication(SQLiteParser::Expr_multiplicationContext *context) = 0;

    virtual std::any visitExpr_string(SQLiteParser::Expr_stringContext *context) = 0;

    virtual std::any visitExpr_collate(SQLiteParser::Expr_collateContext *context) = 0;

    virtual std::any visitExpr_unary(SQLiteParser::Expr_unaryContext *context) = 0;

    virtual std::any visitExpr_base(SQLiteParser::Expr_baseContext *context) = 0;

    virtual std::any visitExpr_recursive(SQLiteParser::Expr_recursiveContext *context) = 0;

    virtual std::any visitRaise_function(SQLiteParser::Raise_functionContext *context) = 0;

    virtual std::any visitLiteral_value(SQLiteParser::Literal_valueContext *context) = 0;

    virtual std::any visitPercentile_clause(SQLiteParser::Percentile_clauseContext *context) = 0;

    virtual std::any visitValue_row(SQLiteParser::Value_rowContext *context) = 0;

    virtual std::any visitValues_clause(SQLiteParser::Values_clauseContext *context) = 0;

    virtual std::any visitInsert_stmt(SQLiteParser::Insert_stmtContext *context) = 0;

    virtual std::any visitReturning_clause(SQLiteParser::Returning_clauseContext *context) = 0;

    virtual std::any visitUpsert_clause(SQLiteParser::Upsert_clauseContext *context) = 0;

    virtual std::any visitPragma_stmt(SQLiteParser::Pragma_stmtContext *context) = 0;

    virtual std::any visitPragma_value(SQLiteParser::Pragma_valueContext *context) = 0;

    virtual std::any visitReindex_stmt(SQLiteParser::Reindex_stmtContext *context) = 0;

    virtual std::any visitSelect_stmt(SQLiteParser::Select_stmtContext *context) = 0;

    virtual std::any visitJoin_clause(SQLiteParser::Join_clauseContext *context) = 0;

    virtual std::any visitSelect_core(SQLiteParser::Select_coreContext *context) = 0;

    virtual std::any visitTable_or_subquery(SQLiteParser::Table_or_subqueryContext *context) = 0;

    virtual std::any visitResult_column(SQLiteParser::Result_columnContext *context) = 0;

    virtual std::any visitJoin_operator(SQLiteParser::Join_operatorContext *context) = 0;

    virtual std::any visitJoin_constraint(SQLiteParser::Join_constraintContext *context) = 0;

    virtual std::any visitCompound_operator(SQLiteParser::Compound_operatorContext *context) = 0;

    virtual std::any visitUpdate_stmt(SQLiteParser::Update_stmtContext *context) = 0;

    virtual std::any visitColumn_name_list(SQLiteParser::Column_name_listContext *context) = 0;

    virtual std::any visitQualified_table_name(SQLiteParser::Qualified_table_nameContext *context) = 0;

    virtual std::any visitVacuum_stmt(SQLiteParser::Vacuum_stmtContext *context) = 0;

    virtual std::any visitFilter_clause(SQLiteParser::Filter_clauseContext *context) = 0;

    virtual std::any visitWindow_defn(SQLiteParser::Window_defnContext *context) = 0;

    virtual std::any visitOver_clause(SQLiteParser::Over_clauseContext *context) = 0;

    virtual std::any visitFrame_spec(SQLiteParser::Frame_specContext *context) = 0;

    virtual std::any visitFrame_clause(SQLiteParser::Frame_clauseContext *context) = 0;

    virtual std::any visitOrder_clause(SQLiteParser::Order_clauseContext *context) = 0;

    virtual std::any visitLimit_clause(SQLiteParser::Limit_clauseContext *context) = 0;

    virtual std::any visitOrdering_term(SQLiteParser::Ordering_termContext *context) = 0;

    virtual std::any visitAsc_desc(SQLiteParser::Asc_descContext *context) = 0;

    virtual std::any visitFrame_left(SQLiteParser::Frame_leftContext *context) = 0;

    virtual std::any visitFrame_right(SQLiteParser::Frame_rightContext *context) = 0;

    virtual std::any visitFrame_single(SQLiteParser::Frame_singleContext *context) = 0;

    virtual std::any visitError_message(SQLiteParser::Error_messageContext *context) = 0;

    virtual std::any visitFilename(SQLiteParser::FilenameContext *context) = 0;

    virtual std::any visitModule_argument(SQLiteParser::Module_argumentContext *context) = 0;

    virtual std::any visitModule_argument_outer(SQLiteParser::Module_argument_outerContext *context) = 0;

    virtual std::any visitModule_argument_inner(SQLiteParser::Module_argument_innerContext *context) = 0;

    virtual std::any visitFallback_excluding_conflicts(SQLiteParser::Fallback_excluding_conflictsContext *context) = 0;

    virtual std::any visitJoin_keyword(SQLiteParser::Join_keywordContext *context) = 0;

    virtual std::any visitFallback(SQLiteParser::FallbackContext *context) = 0;

    virtual std::any visitName(SQLiteParser::NameContext *context) = 0;

    virtual std::any visitFunction_name(SQLiteParser::Function_nameContext *context) = 0;

    virtual std::any visitSchema_name(SQLiteParser::Schema_nameContext *context) = 0;

    virtual std::any visitTable_name(SQLiteParser::Table_nameContext *context) = 0;

    virtual std::any visitTable_or_index_name(SQLiteParser::Table_or_index_nameContext *context) = 0;

    virtual std::any visitColumn_name(SQLiteParser::Column_nameContext *context) = 0;

    virtual std::any visitColumn_name_excluding_string(SQLiteParser::Column_name_excluding_stringContext *context) = 0;

    virtual std::any visitColumn_alias(SQLiteParser::Column_aliasContext *context) = 0;

    virtual std::any visitCollation_name(SQLiteParser::Collation_nameContext *context) = 0;

    virtual std::any visitForeign_table(SQLiteParser::Foreign_tableContext *context) = 0;

    virtual std::any visitIndex_name(SQLiteParser::Index_nameContext *context) = 0;

    virtual std::any visitTrigger_name(SQLiteParser::Trigger_nameContext *context) = 0;

    virtual std::any visitView_name(SQLiteParser::View_nameContext *context) = 0;

    virtual std::any visitModule_name(SQLiteParser::Module_nameContext *context) = 0;

    virtual std::any visitPragma_name(SQLiteParser::Pragma_nameContext *context) = 0;

    virtual std::any visitSavepoint_name(SQLiteParser::Savepoint_nameContext *context) = 0;

    virtual std::any visitTable_alias(SQLiteParser::Table_aliasContext *context) = 0;

    virtual std::any visitTable_alias_excluding_joins(SQLiteParser::Table_alias_excluding_joinsContext *context) = 0;

    virtual std::any visitWindow_name(SQLiteParser::Window_nameContext *context) = 0;

    virtual std::any visitAlias(SQLiteParser::AliasContext *context) = 0;

    virtual std::any visitBase_window_name(SQLiteParser::Base_window_nameContext *context) = 0;

    virtual std::any visitTable_function_name(SQLiteParser::Table_function_nameContext *context) = 0;

    virtual std::any visitAny_name_excluding_raise(SQLiteParser::Any_name_excluding_raiseContext *context) = 0;

    virtual std::any visitAny_name_excluding_joins(SQLiteParser::Any_name_excluding_joinsContext *context) = 0;

    virtual std::any visitAny_name_excluding_string(SQLiteParser::Any_name_excluding_stringContext *context) = 0;

    virtual std::any visitAny_name(SQLiteParser::Any_nameContext *context) = 0;


};

