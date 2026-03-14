
// Generated from SQLiteParser.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  SQLiteParser : public antlr4::Parser {
public:
  enum {
    SCOL = 1, DOT = 2, OPEN_PAR = 3, CLOSE_PAR = 4, COMMA = 5, ASSIGN = 6, 
    STAR = 7, PLUS = 8, MINUS = 9, TILDE = 10, PIPE2 = 11, DIV = 12, MOD = 13, 
    LT2 = 14, GT2 = 15, AMP = 16, PIPE = 17, LT = 18, LT_EQ = 19, GT = 20, 
    GT_EQ = 21, EQ = 22, NOT_EQ1 = 23, NOT_EQ2 = 24, JPTR = 25, JPTR2 = 26, 
    ABORT_ = 27, ACTION_ = 28, ADD_ = 29, AFTER_ = 30, ALL_ = 31, ALTER_ = 32, 
    ALWAYS_ = 33, ANALYZE_ = 34, AND_ = 35, AS_ = 36, ASC_ = 37, ATTACH_ = 38, 
    AUTOINCREMENT_ = 39, BEFORE_ = 40, BEGIN_ = 41, BETWEEN_ = 42, BY_ = 43, 
    CASCADE_ = 44, CASE_ = 45, CAST_ = 46, CHECK_ = 47, COLLATE_ = 48, COLUMN_ = 49, 
    COMMIT_ = 50, CONFLICT_ = 51, CONSTRAINT_ = 52, CREATE_ = 53, CROSS_ = 54, 
    CURRENT_ = 55, CURRENT_DATE_ = 56, CURRENT_TIME_ = 57, CURRENT_TIMESTAMP_ = 58, 
    DATABASE_ = 59, DEFAULT_ = 60, DEFERRABLE_ = 61, DEFERRED_ = 62, DELETE_ = 63, 
    DESC_ = 64, DETACH_ = 65, DISTINCT_ = 66, DO_ = 67, DROP_ = 68, EACH_ = 69, 
    ELSE_ = 70, END_ = 71, ESCAPE_ = 72, EXCEPT_ = 73, EXCLUDE_ = 74, EXCLUSIVE_ = 75, 
    EXISTS_ = 76, EXPLAIN_ = 77, FAIL_ = 78, FALSE_ = 79, FILTER_ = 80, 
    FIRST_ = 81, FOLLOWING_ = 82, FOR_ = 83, FOREIGN_ = 84, FROM_ = 85, 
    FULL_ = 86, GENERATED_ = 87, GLOB_ = 88, GROUP_ = 89, GROUPS_ = 90, 
    HAVING_ = 91, IF_ = 92, IGNORE_ = 93, IMMEDIATE_ = 94, IN_ = 95, INDEX_ = 96, 
    INDEXED_ = 97, INITIALLY_ = 98, INNER_ = 99, INSERT_ = 100, INSTEAD_ = 101, 
    INTERSECT_ = 102, INTO_ = 103, IS_ = 104, ISNULL_ = 105, JOIN_ = 106, 
    KEY_ = 107, LAST_ = 108, LEFT_ = 109, LIKE_ = 110, LIMIT_ = 111, MATCH_ = 112, 
    MATERIALIZED_ = 113, NATURAL_ = 114, NO_ = 115, NOT_ = 116, NOTHING_ = 117, 
    NOTNULL_ = 118, NULL_ = 119, NULLS_ = 120, OF_ = 121, OFFSET_ = 122, 
    ON_ = 123, OR_ = 124, ORDER_ = 125, OTHERS_ = 126, OUTER_ = 127, OVER_ = 128, 
    PARTITION_ = 129, PLAN_ = 130, PRAGMA_ = 131, PRECEDING_ = 132, PRIMARY_ = 133, 
    QUERY_ = 134, RAISE_ = 135, RANGE_ = 136, RECURSIVE_ = 137, REFERENCES_ = 138, 
    REGEXP_ = 139, REINDEX_ = 140, RELEASE_ = 141, RENAME_ = 142, REPLACE_ = 143, 
    RESTRICT_ = 144, RETURNING_ = 145, RIGHT_ = 146, ROLLBACK_ = 147, ROW_ = 148, 
    ROWID_ = 149, ROWS_ = 150, SAVEPOINT_ = 151, SELECT_ = 152, SET_ = 153, 
    STORED_ = 154, STRICT_ = 155, TABLE_ = 156, TEMP_ = 157, TEMPORARY_ = 158, 
    THEN_ = 159, TIES_ = 160, TO_ = 161, TRANSACTION_ = 162, TRIGGER_ = 163, 
    TRUE_ = 164, UNBOUNDED_ = 165, UNION_ = 166, UNIQUE_ = 167, UPDATE_ = 168, 
    USING_ = 169, VACUUM_ = 170, VALUES_ = 171, VIEW_ = 172, VIRTUAL_ = 173, 
    WHEN_ = 174, WHERE_ = 175, WINDOW_ = 176, WITH_ = 177, WITHIN_ = 178, 
    WITHOUT_ = 179, IDENTIFIER = 180, NUMERIC_LITERAL = 181, BIND_PARAMETER = 182, 
    STRING_LITERAL = 183, BLOB_LITERAL = 184, SINGLE_LINE_COMMENT = 185, 
    MULTILINE_COMMENT = 186, SPACES = 187, UNEXPECTED_CHAR = 188
  };

  enum {
    RuleParse = 0, RuleSql_stmt_list = 1, RuleSql_stmt = 2, RuleAlter_table_stmt = 3, 
    RuleAnalyze_stmt = 4, RuleAttach_stmt = 5, RuleBegin_stmt = 6, RuleCommit_stmt = 7, 
    RuleRollback_stmt = 8, RuleSavepoint_stmt = 9, RuleRelease_stmt = 10, 
    RuleCreate_index_stmt = 11, RuleIndexed_column = 12, RuleCreate_table_stmt = 13, 
    RuleTable_options = 14, RuleColumn_def = 15, RuleType_name = 16, RuleColumn_constraint = 17, 
    RuleSigned_number = 18, RuleTable_constraint = 19, RuleForeign_key_clause = 20, 
    RuleConflict_clause = 21, RuleCreate_trigger_stmt = 22, RuleCreate_view_stmt = 23, 
    RuleCreate_virtual_table_stmt = 24, RuleWith_clause = 25, RuleCommon_table_expression = 26, 
    RuleCte_table_name = 27, RuleDelete_stmt = 28, RuleDetach_stmt = 29, 
    RuleDrop_stmt = 30, RuleExpr = 31, RuleExpr_or = 32, RuleExpr_and = 33, 
    RuleExpr_not = 34, RuleExpr_binary = 35, RuleExpr_comparison = 36, RuleExpr_bitwise = 37, 
    RuleExpr_addition = 38, RuleExpr_multiplication = 39, RuleExpr_string = 40, 
    RuleExpr_collate = 41, RuleExpr_unary = 42, RuleExpr_base = 43, RuleExpr_recursive = 44, 
    RuleRaise_function = 45, RuleLiteral_value = 46, RulePercentile_clause = 47, 
    RuleValue_row = 48, RuleValues_clause = 49, RuleInsert_stmt = 50, RuleReturning_clause = 51, 
    RuleUpsert_clause = 52, RulePragma_stmt = 53, RulePragma_value = 54, 
    RuleReindex_stmt = 55, RuleSelect_stmt = 56, RuleJoin_clause = 57, RuleSelect_core = 58, 
    RuleTable_or_subquery = 59, RuleResult_column = 60, RuleJoin_operator = 61, 
    RuleJoin_constraint = 62, RuleCompound_operator = 63, RuleUpdate_stmt = 64, 
    RuleColumn_name_list = 65, RuleQualified_table_name = 66, RuleVacuum_stmt = 67, 
    RuleFilter_clause = 68, RuleWindow_defn = 69, RuleOver_clause = 70, 
    RuleFrame_spec = 71, RuleFrame_clause = 72, RuleOrder_clause = 73, RuleLimit_clause = 74, 
    RuleOrdering_term = 75, RuleAsc_desc = 76, RuleFrame_left = 77, RuleFrame_right = 78, 
    RuleFrame_single = 79, RuleError_message = 80, RuleFilename = 81, RuleModule_argument = 82, 
    RuleModule_argument_outer = 83, RuleModule_argument_inner = 84, RuleFallback_excluding_conflicts = 85, 
    RuleJoin_keyword = 86, RuleFallback = 87, RuleName = 88, RuleFunction_name = 89, 
    RuleSchema_name = 90, RuleTable_name = 91, RuleTable_or_index_name = 92, 
    RuleColumn_name = 93, RuleColumn_name_excluding_string = 94, RuleColumn_alias = 95, 
    RuleCollation_name = 96, RuleForeign_table = 97, RuleIndex_name = 98, 
    RuleTrigger_name = 99, RuleView_name = 100, RuleModule_name = 101, RulePragma_name = 102, 
    RuleSavepoint_name = 103, RuleTable_alias = 104, RuleTable_alias_excluding_joins = 105, 
    RuleWindow_name = 106, RuleAlias = 107, RuleBase_window_name = 108, 
    RuleTable_function_name = 109, RuleAny_name_excluding_raise = 110, RuleAny_name_excluding_joins = 111, 
    RuleAny_name_excluding_string = 112, RuleAny_name = 113
  };

  explicit SQLiteParser(antlr4::TokenStream *input);

  SQLiteParser(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~SQLiteParser() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class ParseContext;
  class Sql_stmt_listContext;
  class Sql_stmtContext;
  class Alter_table_stmtContext;
  class Analyze_stmtContext;
  class Attach_stmtContext;
  class Begin_stmtContext;
  class Commit_stmtContext;
  class Rollback_stmtContext;
  class Savepoint_stmtContext;
  class Release_stmtContext;
  class Create_index_stmtContext;
  class Indexed_columnContext;
  class Create_table_stmtContext;
  class Table_optionsContext;
  class Column_defContext;
  class Type_nameContext;
  class Column_constraintContext;
  class Signed_numberContext;
  class Table_constraintContext;
  class Foreign_key_clauseContext;
  class Conflict_clauseContext;
  class Create_trigger_stmtContext;
  class Create_view_stmtContext;
  class Create_virtual_table_stmtContext;
  class With_clauseContext;
  class Common_table_expressionContext;
  class Cte_table_nameContext;
  class Delete_stmtContext;
  class Detach_stmtContext;
  class Drop_stmtContext;
  class ExprContext;
  class Expr_orContext;
  class Expr_andContext;
  class Expr_notContext;
  class Expr_binaryContext;
  class Expr_comparisonContext;
  class Expr_bitwiseContext;
  class Expr_additionContext;
  class Expr_multiplicationContext;
  class Expr_stringContext;
  class Expr_collateContext;
  class Expr_unaryContext;
  class Expr_baseContext;
  class Expr_recursiveContext;
  class Raise_functionContext;
  class Literal_valueContext;
  class Percentile_clauseContext;
  class Value_rowContext;
  class Values_clauseContext;
  class Insert_stmtContext;
  class Returning_clauseContext;
  class Upsert_clauseContext;
  class Pragma_stmtContext;
  class Pragma_valueContext;
  class Reindex_stmtContext;
  class Select_stmtContext;
  class Join_clauseContext;
  class Select_coreContext;
  class Table_or_subqueryContext;
  class Result_columnContext;
  class Join_operatorContext;
  class Join_constraintContext;
  class Compound_operatorContext;
  class Update_stmtContext;
  class Column_name_listContext;
  class Qualified_table_nameContext;
  class Vacuum_stmtContext;
  class Filter_clauseContext;
  class Window_defnContext;
  class Over_clauseContext;
  class Frame_specContext;
  class Frame_clauseContext;
  class Order_clauseContext;
  class Limit_clauseContext;
  class Ordering_termContext;
  class Asc_descContext;
  class Frame_leftContext;
  class Frame_rightContext;
  class Frame_singleContext;
  class Error_messageContext;
  class FilenameContext;
  class Module_argumentContext;
  class Module_argument_outerContext;
  class Module_argument_innerContext;
  class Fallback_excluding_conflictsContext;
  class Join_keywordContext;
  class FallbackContext;
  class NameContext;
  class Function_nameContext;
  class Schema_nameContext;
  class Table_nameContext;
  class Table_or_index_nameContext;
  class Column_nameContext;
  class Column_name_excluding_stringContext;
  class Column_aliasContext;
  class Collation_nameContext;
  class Foreign_tableContext;
  class Index_nameContext;
  class Trigger_nameContext;
  class View_nameContext;
  class Module_nameContext;
  class Pragma_nameContext;
  class Savepoint_nameContext;
  class Table_aliasContext;
  class Table_alias_excluding_joinsContext;
  class Window_nameContext;
  class AliasContext;
  class Base_window_nameContext;
  class Table_function_nameContext;
  class Any_name_excluding_raiseContext;
  class Any_name_excluding_joinsContext;
  class Any_name_excluding_stringContext;
  class Any_nameContext; 

  class  ParseContext : public antlr4::ParserRuleContext {
  public:
    ParseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Sql_stmt_listContext *sql_stmt_list();
    antlr4::tree::TerminalNode *EOF();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ParseContext* parse();

  class  Sql_stmt_listContext : public antlr4::ParserRuleContext {
  public:
    Sql_stmt_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Sql_stmtContext *> sql_stmt();
    Sql_stmtContext* sql_stmt(size_t i);
    std::vector<antlr4::tree::TerminalNode *> SCOL();
    antlr4::tree::TerminalNode* SCOL(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Sql_stmt_listContext* sql_stmt_list();

  class  Sql_stmtContext : public antlr4::ParserRuleContext {
  public:
    Sql_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Alter_table_stmtContext *alter_table_stmt();
    Analyze_stmtContext *analyze_stmt();
    Attach_stmtContext *attach_stmt();
    Begin_stmtContext *begin_stmt();
    Commit_stmtContext *commit_stmt();
    Create_index_stmtContext *create_index_stmt();
    Create_table_stmtContext *create_table_stmt();
    Create_trigger_stmtContext *create_trigger_stmt();
    Create_view_stmtContext *create_view_stmt();
    Create_virtual_table_stmtContext *create_virtual_table_stmt();
    Delete_stmtContext *delete_stmt();
    Detach_stmtContext *detach_stmt();
    Drop_stmtContext *drop_stmt();
    Insert_stmtContext *insert_stmt();
    Pragma_stmtContext *pragma_stmt();
    Reindex_stmtContext *reindex_stmt();
    Release_stmtContext *release_stmt();
    Rollback_stmtContext *rollback_stmt();
    Savepoint_stmtContext *savepoint_stmt();
    Select_stmtContext *select_stmt();
    Update_stmtContext *update_stmt();
    Vacuum_stmtContext *vacuum_stmt();
    antlr4::tree::TerminalNode *EXPLAIN_();
    antlr4::tree::TerminalNode *QUERY_();
    antlr4::tree::TerminalNode *PLAN_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Sql_stmtContext* sql_stmt();

  class  Alter_table_stmtContext : public antlr4::ParserRuleContext {
  public:
    SQLiteParser::Table_nameContext *new_table_name = nullptr;
    SQLiteParser::Column_nameContext *old_column_name = nullptr;
    SQLiteParser::Column_nameContext *new_column_name = nullptr;
    Alter_table_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ALTER_();
    antlr4::tree::TerminalNode *TABLE_();
    std::vector<Table_nameContext *> table_name();
    Table_nameContext* table_name(size_t i);
    antlr4::tree::TerminalNode *RENAME_();
    antlr4::tree::TerminalNode *ADD_();
    Column_defContext *column_def();
    antlr4::tree::TerminalNode *DROP_();
    std::vector<Column_nameContext *> column_name();
    Column_nameContext* column_name(size_t i);
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *TO_();
    antlr4::tree::TerminalNode *COLUMN_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Alter_table_stmtContext* alter_table_stmt();

  class  Analyze_stmtContext : public antlr4::ParserRuleContext {
  public:
    Analyze_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ANALYZE_();
    Schema_nameContext *schema_name();
    Table_or_index_nameContext *table_or_index_name();
    antlr4::tree::TerminalNode *DOT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Analyze_stmtContext* analyze_stmt();

  class  Attach_stmtContext : public antlr4::ParserRuleContext {
  public:
    Attach_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ATTACH_();
    ExprContext *expr();
    antlr4::tree::TerminalNode *AS_();
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *DATABASE_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Attach_stmtContext* attach_stmt();

  class  Begin_stmtContext : public antlr4::ParserRuleContext {
  public:
    Begin_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *BEGIN_();
    antlr4::tree::TerminalNode *TRANSACTION_();
    antlr4::tree::TerminalNode *DEFERRED_();
    antlr4::tree::TerminalNode *IMMEDIATE_();
    antlr4::tree::TerminalNode *EXCLUSIVE_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Begin_stmtContext* begin_stmt();

  class  Commit_stmtContext : public antlr4::ParserRuleContext {
  public:
    Commit_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMIT_();
    antlr4::tree::TerminalNode *END_();
    antlr4::tree::TerminalNode *TRANSACTION_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Commit_stmtContext* commit_stmt();

  class  Rollback_stmtContext : public antlr4::ParserRuleContext {
  public:
    Rollback_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ROLLBACK_();
    antlr4::tree::TerminalNode *TRANSACTION_();
    antlr4::tree::TerminalNode *TO_();
    Savepoint_nameContext *savepoint_name();
    antlr4::tree::TerminalNode *SAVEPOINT_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Rollback_stmtContext* rollback_stmt();

  class  Savepoint_stmtContext : public antlr4::ParserRuleContext {
  public:
    Savepoint_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SAVEPOINT_();
    Savepoint_nameContext *savepoint_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Savepoint_stmtContext* savepoint_stmt();

  class  Release_stmtContext : public antlr4::ParserRuleContext {
  public:
    Release_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *RELEASE_();
    Savepoint_nameContext *savepoint_name();
    antlr4::tree::TerminalNode *SAVEPOINT_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Release_stmtContext* release_stmt();

  class  Create_index_stmtContext : public antlr4::ParserRuleContext {
  public:
    Create_index_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CREATE_();
    antlr4::tree::TerminalNode *INDEX_();
    Index_nameContext *index_name();
    antlr4::tree::TerminalNode *ON_();
    Table_nameContext *table_name();
    antlr4::tree::TerminalNode *OPEN_PAR();
    std::vector<Indexed_columnContext *> indexed_column();
    Indexed_columnContext* indexed_column(size_t i);
    antlr4::tree::TerminalNode *CLOSE_PAR();
    antlr4::tree::TerminalNode *UNIQUE_();
    antlr4::tree::TerminalNode *IF_();
    antlr4::tree::TerminalNode *NOT_();
    antlr4::tree::TerminalNode *EXISTS_();
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *DOT();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    antlr4::tree::TerminalNode *WHERE_();
    ExprContext *expr();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Create_index_stmtContext* create_index_stmt();

  class  Indexed_columnContext : public antlr4::ParserRuleContext {
  public:
    Indexed_columnContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExprContext *expr();
    antlr4::tree::TerminalNode *COLLATE_();
    Collation_nameContext *collation_name();
    Asc_descContext *asc_desc();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Indexed_columnContext* indexed_column();

  class  Create_table_stmtContext : public antlr4::ParserRuleContext {
  public:
    Create_table_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CREATE_();
    antlr4::tree::TerminalNode *TABLE_();
    Table_nameContext *table_name();
    antlr4::tree::TerminalNode *OPEN_PAR();
    std::vector<Column_defContext *> column_def();
    Column_defContext* column_def(size_t i);
    antlr4::tree::TerminalNode *CLOSE_PAR();
    antlr4::tree::TerminalNode *AS_();
    Select_stmtContext *select_stmt();
    antlr4::tree::TerminalNode *IF_();
    antlr4::tree::TerminalNode *NOT_();
    antlr4::tree::TerminalNode *EXISTS_();
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *TEMP_();
    antlr4::tree::TerminalNode *TEMPORARY_();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    std::vector<Table_constraintContext *> table_constraint();
    Table_constraintContext* table_constraint(size_t i);
    Table_optionsContext *table_options();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Create_table_stmtContext* create_table_stmt();

  class  Table_optionsContext : public antlr4::ParserRuleContext {
  public:
    Table_optionsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> WITHOUT_();
    antlr4::tree::TerminalNode* WITHOUT_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> ROWID_();
    antlr4::tree::TerminalNode* ROWID_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> STRICT_();
    antlr4::tree::TerminalNode* STRICT_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Table_optionsContext* table_options();

  class  Column_defContext : public antlr4::ParserRuleContext {
  public:
    Column_defContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Column_nameContext *column_name();
    Type_nameContext *type_name();
    std::vector<Column_constraintContext *> column_constraint();
    Column_constraintContext* column_constraint(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Column_defContext* column_def();

  class  Type_nameContext : public antlr4::ParserRuleContext {
  public:
    Type_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<NameContext *> name();
    NameContext* name(size_t i);
    antlr4::tree::TerminalNode *OPEN_PAR();
    std::vector<Signed_numberContext *> signed_number();
    Signed_numberContext* signed_number(size_t i);
    antlr4::tree::TerminalNode *CLOSE_PAR();
    antlr4::tree::TerminalNode *COMMA();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Type_nameContext* type_name();

  class  Column_constraintContext : public antlr4::ParserRuleContext {
  public:
    Column_constraintContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *PRIMARY_();
    antlr4::tree::TerminalNode *KEY_();
    antlr4::tree::TerminalNode *CHECK_();
    antlr4::tree::TerminalNode *OPEN_PAR();
    ExprContext *expr();
    antlr4::tree::TerminalNode *CLOSE_PAR();
    antlr4::tree::TerminalNode *DEFAULT_();
    antlr4::tree::TerminalNode *COLLATE_();
    Collation_nameContext *collation_name();
    Foreign_key_clauseContext *foreign_key_clause();
    antlr4::tree::TerminalNode *AS_();
    antlr4::tree::TerminalNode *CONSTRAINT_();
    NameContext *name();
    antlr4::tree::TerminalNode *NULL_();
    antlr4::tree::TerminalNode *UNIQUE_();
    Signed_numberContext *signed_number();
    Literal_valueContext *literal_value();
    Asc_descContext *asc_desc();
    Conflict_clauseContext *conflict_clause();
    antlr4::tree::TerminalNode *AUTOINCREMENT_();
    antlr4::tree::TerminalNode *GENERATED_();
    antlr4::tree::TerminalNode *ALWAYS_();
    antlr4::tree::TerminalNode *STORED_();
    antlr4::tree::TerminalNode *VIRTUAL_();
    antlr4::tree::TerminalNode *NOT_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Column_constraintContext* column_constraint();

  class  Signed_numberContext : public antlr4::ParserRuleContext {
  public:
    Signed_numberContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NUMERIC_LITERAL();
    antlr4::tree::TerminalNode *PLUS();
    antlr4::tree::TerminalNode *MINUS();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Signed_numberContext* signed_number();

  class  Table_constraintContext : public antlr4::ParserRuleContext {
  public:
    Table_constraintContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OPEN_PAR();
    std::vector<Indexed_columnContext *> indexed_column();
    Indexed_columnContext* indexed_column(size_t i);
    antlr4::tree::TerminalNode *CLOSE_PAR();
    antlr4::tree::TerminalNode *CHECK_();
    ExprContext *expr();
    antlr4::tree::TerminalNode *FOREIGN_();
    antlr4::tree::TerminalNode *KEY_();
    std::vector<Column_nameContext *> column_name();
    Column_nameContext* column_name(size_t i);
    Foreign_key_clauseContext *foreign_key_clause();
    antlr4::tree::TerminalNode *CONSTRAINT_();
    NameContext *name();
    antlr4::tree::TerminalNode *PRIMARY_();
    antlr4::tree::TerminalNode *UNIQUE_();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    Conflict_clauseContext *conflict_clause();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Table_constraintContext* table_constraint();

  class  Foreign_key_clauseContext : public antlr4::ParserRuleContext {
  public:
    Foreign_key_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *REFERENCES_();
    Foreign_tableContext *foreign_table();
    antlr4::tree::TerminalNode *OPEN_PAR();
    std::vector<Column_nameContext *> column_name();
    Column_nameContext* column_name(size_t i);
    antlr4::tree::TerminalNode *CLOSE_PAR();
    std::vector<antlr4::tree::TerminalNode *> ON_();
    antlr4::tree::TerminalNode* ON_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> MATCH_();
    antlr4::tree::TerminalNode* MATCH_(size_t i);
    std::vector<NameContext *> name();
    NameContext* name(size_t i);
    antlr4::tree::TerminalNode *DEFERRABLE_();
    std::vector<antlr4::tree::TerminalNode *> DELETE_();
    antlr4::tree::TerminalNode* DELETE_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> UPDATE_();
    antlr4::tree::TerminalNode* UPDATE_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> SET_();
    antlr4::tree::TerminalNode* SET_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> CASCADE_();
    antlr4::tree::TerminalNode* CASCADE_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> RESTRICT_();
    antlr4::tree::TerminalNode* RESTRICT_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> NO_();
    antlr4::tree::TerminalNode* NO_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> ACTION_();
    antlr4::tree::TerminalNode* ACTION_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    std::vector<antlr4::tree::TerminalNode *> NULL_();
    antlr4::tree::TerminalNode* NULL_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> DEFAULT_();
    antlr4::tree::TerminalNode* DEFAULT_(size_t i);
    antlr4::tree::TerminalNode *NOT_();
    antlr4::tree::TerminalNode *INITIALLY_();
    antlr4::tree::TerminalNode *DEFERRED_();
    antlr4::tree::TerminalNode *IMMEDIATE_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Foreign_key_clauseContext* foreign_key_clause();

  class  Conflict_clauseContext : public antlr4::ParserRuleContext {
  public:
    Conflict_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ON_();
    antlr4::tree::TerminalNode *CONFLICT_();
    antlr4::tree::TerminalNode *ROLLBACK_();
    antlr4::tree::TerminalNode *ABORT_();
    antlr4::tree::TerminalNode *FAIL_();
    antlr4::tree::TerminalNode *IGNORE_();
    antlr4::tree::TerminalNode *REPLACE_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Conflict_clauseContext* conflict_clause();

  class  Create_trigger_stmtContext : public antlr4::ParserRuleContext {
  public:
    Create_trigger_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CREATE_();
    antlr4::tree::TerminalNode *TRIGGER_();
    Trigger_nameContext *trigger_name();
    antlr4::tree::TerminalNode *ON_();
    Table_nameContext *table_name();
    antlr4::tree::TerminalNode *BEGIN_();
    antlr4::tree::TerminalNode *END_();
    antlr4::tree::TerminalNode *DELETE_();
    antlr4::tree::TerminalNode *INSERT_();
    antlr4::tree::TerminalNode *UPDATE_();
    antlr4::tree::TerminalNode *IF_();
    antlr4::tree::TerminalNode *NOT_();
    antlr4::tree::TerminalNode *EXISTS_();
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *BEFORE_();
    antlr4::tree::TerminalNode *AFTER_();
    antlr4::tree::TerminalNode *INSTEAD_();
    std::vector<antlr4::tree::TerminalNode *> OF_();
    antlr4::tree::TerminalNode* OF_(size_t i);
    antlr4::tree::TerminalNode *FOR_();
    antlr4::tree::TerminalNode *EACH_();
    antlr4::tree::TerminalNode *ROW_();
    antlr4::tree::TerminalNode *WHEN_();
    ExprContext *expr();
    std::vector<antlr4::tree::TerminalNode *> SCOL();
    antlr4::tree::TerminalNode* SCOL(size_t i);
    antlr4::tree::TerminalNode *TEMP_();
    antlr4::tree::TerminalNode *TEMPORARY_();
    std::vector<Column_nameContext *> column_name();
    Column_nameContext* column_name(size_t i);
    std::vector<Update_stmtContext *> update_stmt();
    Update_stmtContext* update_stmt(size_t i);
    std::vector<Insert_stmtContext *> insert_stmt();
    Insert_stmtContext* insert_stmt(size_t i);
    std::vector<Delete_stmtContext *> delete_stmt();
    Delete_stmtContext* delete_stmt(size_t i);
    std::vector<Select_stmtContext *> select_stmt();
    Select_stmtContext* select_stmt(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Create_trigger_stmtContext* create_trigger_stmt();

  class  Create_view_stmtContext : public antlr4::ParserRuleContext {
  public:
    Create_view_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CREATE_();
    antlr4::tree::TerminalNode *VIEW_();
    View_nameContext *view_name();
    antlr4::tree::TerminalNode *AS_();
    Select_stmtContext *select_stmt();
    antlr4::tree::TerminalNode *IF_();
    antlr4::tree::TerminalNode *NOT_();
    antlr4::tree::TerminalNode *EXISTS_();
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *OPEN_PAR();
    std::vector<Column_nameContext *> column_name();
    Column_nameContext* column_name(size_t i);
    antlr4::tree::TerminalNode *CLOSE_PAR();
    antlr4::tree::TerminalNode *TEMP_();
    antlr4::tree::TerminalNode *TEMPORARY_();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Create_view_stmtContext* create_view_stmt();

  class  Create_virtual_table_stmtContext : public antlr4::ParserRuleContext {
  public:
    Create_virtual_table_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CREATE_();
    antlr4::tree::TerminalNode *VIRTUAL_();
    antlr4::tree::TerminalNode *TABLE_();
    Table_nameContext *table_name();
    antlr4::tree::TerminalNode *USING_();
    Module_nameContext *module_name();
    antlr4::tree::TerminalNode *IF_();
    antlr4::tree::TerminalNode *NOT_();
    antlr4::tree::TerminalNode *EXISTS_();
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *OPEN_PAR();
    std::vector<Module_argumentContext *> module_argument();
    Module_argumentContext* module_argument(size_t i);
    antlr4::tree::TerminalNode *CLOSE_PAR();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Create_virtual_table_stmtContext* create_virtual_table_stmt();

  class  With_clauseContext : public antlr4::ParserRuleContext {
  public:
    With_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *WITH_();
    std::vector<Common_table_expressionContext *> common_table_expression();
    Common_table_expressionContext* common_table_expression(size_t i);
    antlr4::tree::TerminalNode *RECURSIVE_();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  With_clauseContext* with_clause();

  class  Common_table_expressionContext : public antlr4::ParserRuleContext {
  public:
    Common_table_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Cte_table_nameContext *cte_table_name();
    antlr4::tree::TerminalNode *AS_();
    antlr4::tree::TerminalNode *OPEN_PAR();
    Select_stmtContext *select_stmt();
    antlr4::tree::TerminalNode *CLOSE_PAR();
    antlr4::tree::TerminalNode *MATERIALIZED_();
    antlr4::tree::TerminalNode *NOT_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Common_table_expressionContext* common_table_expression();

  class  Cte_table_nameContext : public antlr4::ParserRuleContext {
  public:
    Cte_table_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Table_nameContext *table_name();
    antlr4::tree::TerminalNode *OPEN_PAR();
    std::vector<Column_nameContext *> column_name();
    Column_nameContext* column_name(size_t i);
    antlr4::tree::TerminalNode *CLOSE_PAR();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Cte_table_nameContext* cte_table_name();

  class  Delete_stmtContext : public antlr4::ParserRuleContext {
  public:
    Delete_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DELETE_();
    antlr4::tree::TerminalNode *FROM_();
    Qualified_table_nameContext *qualified_table_name();
    With_clauseContext *with_clause();
    antlr4::tree::TerminalNode *WHERE_();
    ExprContext *expr();
    Returning_clauseContext *returning_clause();
    Order_clauseContext *order_clause();
    Limit_clauseContext *limit_clause();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Delete_stmtContext* delete_stmt();

  class  Detach_stmtContext : public antlr4::ParserRuleContext {
  public:
    Detach_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DETACH_();
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *DATABASE_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Detach_stmtContext* detach_stmt();

  class  Drop_stmtContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *object = nullptr;
    Drop_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DROP_();
    Any_nameContext *any_name();
    antlr4::tree::TerminalNode *INDEX_();
    antlr4::tree::TerminalNode *TABLE_();
    antlr4::tree::TerminalNode *TRIGGER_();
    antlr4::tree::TerminalNode *VIEW_();
    antlr4::tree::TerminalNode *IF_();
    antlr4::tree::TerminalNode *EXISTS_();
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *DOT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Drop_stmtContext* drop_stmt();

  class  ExprContext : public antlr4::ParserRuleContext {
  public:
    ExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Expr_orContext *expr_or();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ExprContext* expr();

  class  Expr_orContext : public antlr4::ParserRuleContext {
  public:
    Expr_orContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Expr_andContext *> expr_and();
    Expr_andContext* expr_and(size_t i);
    std::vector<antlr4::tree::TerminalNode *> OR_();
    antlr4::tree::TerminalNode* OR_(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Expr_orContext* expr_or();

  class  Expr_andContext : public antlr4::ParserRuleContext {
  public:
    Expr_andContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Expr_notContext *> expr_not();
    Expr_notContext* expr_not(size_t i);
    std::vector<antlr4::tree::TerminalNode *> AND_();
    antlr4::tree::TerminalNode* AND_(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Expr_andContext* expr_and();

  class  Expr_notContext : public antlr4::ParserRuleContext {
  public:
    Expr_notContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Expr_binaryContext *expr_binary();
    std::vector<antlr4::tree::TerminalNode *> NOT_();
    antlr4::tree::TerminalNode* NOT_(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Expr_notContext* expr_not();

  class  Expr_binaryContext : public antlr4::ParserRuleContext {
  public:
    Expr_binaryContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Expr_comparisonContext *> expr_comparison();
    Expr_comparisonContext* expr_comparison(size_t i);
    std::vector<antlr4::tree::TerminalNode *> IS_();
    antlr4::tree::TerminalNode* IS_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> BETWEEN_();
    antlr4::tree::TerminalNode* BETWEEN_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> AND_();
    antlr4::tree::TerminalNode* AND_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> IN_();
    antlr4::tree::TerminalNode* IN_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> ISNULL_();
    antlr4::tree::TerminalNode* ISNULL_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> NOTNULL_();
    antlr4::tree::TerminalNode* NOTNULL_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> NOT_();
    antlr4::tree::TerminalNode* NOT_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> NULL_();
    antlr4::tree::TerminalNode* NULL_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> ASSIGN();
    antlr4::tree::TerminalNode* ASSIGN(size_t i);
    std::vector<antlr4::tree::TerminalNode *> EQ();
    antlr4::tree::TerminalNode* EQ(size_t i);
    std::vector<antlr4::tree::TerminalNode *> NOT_EQ1();
    antlr4::tree::TerminalNode* NOT_EQ1(size_t i);
    std::vector<antlr4::tree::TerminalNode *> NOT_EQ2();
    antlr4::tree::TerminalNode* NOT_EQ2(size_t i);
    std::vector<antlr4::tree::TerminalNode *> OPEN_PAR();
    antlr4::tree::TerminalNode* OPEN_PAR(size_t i);
    std::vector<antlr4::tree::TerminalNode *> CLOSE_PAR();
    antlr4::tree::TerminalNode* CLOSE_PAR(size_t i);
    std::vector<Table_nameContext *> table_name();
    Table_nameContext* table_name(size_t i);
    std::vector<Table_function_nameContext *> table_function_name();
    Table_function_nameContext* table_function_name(size_t i);
    std::vector<antlr4::tree::TerminalNode *> LIKE_();
    antlr4::tree::TerminalNode* LIKE_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> DISTINCT_();
    antlr4::tree::TerminalNode* DISTINCT_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> FROM_();
    antlr4::tree::TerminalNode* FROM_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> GLOB_();
    antlr4::tree::TerminalNode* GLOB_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> REGEXP_();
    antlr4::tree::TerminalNode* REGEXP_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> MATCH_();
    antlr4::tree::TerminalNode* MATCH_(size_t i);
    std::vector<Select_stmtContext *> select_stmt();
    Select_stmtContext* select_stmt(size_t i);
    std::vector<Schema_nameContext *> schema_name();
    Schema_nameContext* schema_name(size_t i);
    std::vector<antlr4::tree::TerminalNode *> DOT();
    antlr4::tree::TerminalNode* DOT(size_t i);
    std::vector<antlr4::tree::TerminalNode *> ESCAPE_();
    antlr4::tree::TerminalNode* ESCAPE_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Expr_binaryContext* expr_binary();

  class  Expr_comparisonContext : public antlr4::ParserRuleContext {
  public:
    Expr_comparisonContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Expr_bitwiseContext *> expr_bitwise();
    Expr_bitwiseContext* expr_bitwise(size_t i);
    std::vector<antlr4::tree::TerminalNode *> LT();
    antlr4::tree::TerminalNode* LT(size_t i);
    std::vector<antlr4::tree::TerminalNode *> LT_EQ();
    antlr4::tree::TerminalNode* LT_EQ(size_t i);
    std::vector<antlr4::tree::TerminalNode *> GT();
    antlr4::tree::TerminalNode* GT(size_t i);
    std::vector<antlr4::tree::TerminalNode *> GT_EQ();
    antlr4::tree::TerminalNode* GT_EQ(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Expr_comparisonContext* expr_comparison();

  class  Expr_bitwiseContext : public antlr4::ParserRuleContext {
  public:
    Expr_bitwiseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Expr_additionContext *> expr_addition();
    Expr_additionContext* expr_addition(size_t i);
    std::vector<antlr4::tree::TerminalNode *> LT2();
    antlr4::tree::TerminalNode* LT2(size_t i);
    std::vector<antlr4::tree::TerminalNode *> GT2();
    antlr4::tree::TerminalNode* GT2(size_t i);
    std::vector<antlr4::tree::TerminalNode *> AMP();
    antlr4::tree::TerminalNode* AMP(size_t i);
    std::vector<antlr4::tree::TerminalNode *> PIPE();
    antlr4::tree::TerminalNode* PIPE(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Expr_bitwiseContext* expr_bitwise();

  class  Expr_additionContext : public antlr4::ParserRuleContext {
  public:
    Expr_additionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Expr_multiplicationContext *> expr_multiplication();
    Expr_multiplicationContext* expr_multiplication(size_t i);
    std::vector<antlr4::tree::TerminalNode *> PLUS();
    antlr4::tree::TerminalNode* PLUS(size_t i);
    std::vector<antlr4::tree::TerminalNode *> MINUS();
    antlr4::tree::TerminalNode* MINUS(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Expr_additionContext* expr_addition();

  class  Expr_multiplicationContext : public antlr4::ParserRuleContext {
  public:
    Expr_multiplicationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Expr_stringContext *> expr_string();
    Expr_stringContext* expr_string(size_t i);
    std::vector<antlr4::tree::TerminalNode *> STAR();
    antlr4::tree::TerminalNode* STAR(size_t i);
    std::vector<antlr4::tree::TerminalNode *> DIV();
    antlr4::tree::TerminalNode* DIV(size_t i);
    std::vector<antlr4::tree::TerminalNode *> MOD();
    antlr4::tree::TerminalNode* MOD(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Expr_multiplicationContext* expr_multiplication();

  class  Expr_stringContext : public antlr4::ParserRuleContext {
  public:
    Expr_stringContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Expr_collateContext *> expr_collate();
    Expr_collateContext* expr_collate(size_t i);
    std::vector<antlr4::tree::TerminalNode *> PIPE2();
    antlr4::tree::TerminalNode* PIPE2(size_t i);
    std::vector<antlr4::tree::TerminalNode *> JPTR();
    antlr4::tree::TerminalNode* JPTR(size_t i);
    std::vector<antlr4::tree::TerminalNode *> JPTR2();
    antlr4::tree::TerminalNode* JPTR2(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Expr_stringContext* expr_string();

  class  Expr_collateContext : public antlr4::ParserRuleContext {
  public:
    Expr_collateContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Expr_unaryContext *expr_unary();
    std::vector<antlr4::tree::TerminalNode *> COLLATE_();
    antlr4::tree::TerminalNode* COLLATE_(size_t i);
    std::vector<Collation_nameContext *> collation_name();
    Collation_nameContext* collation_name(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Expr_collateContext* expr_collate();

  class  Expr_unaryContext : public antlr4::ParserRuleContext {
  public:
    Expr_unaryContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Expr_baseContext *expr_base();
    std::vector<antlr4::tree::TerminalNode *> MINUS();
    antlr4::tree::TerminalNode* MINUS(size_t i);
    std::vector<antlr4::tree::TerminalNode *> PLUS();
    antlr4::tree::TerminalNode* PLUS(size_t i);
    std::vector<antlr4::tree::TerminalNode *> TILDE();
    antlr4::tree::TerminalNode* TILDE(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Expr_unaryContext* expr_unary();

  class  Expr_baseContext : public antlr4::ParserRuleContext {
  public:
    Expr_baseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Literal_valueContext *literal_value();
    antlr4::tree::TerminalNode *BIND_PARAMETER();
    Table_nameContext *table_name();
    std::vector<antlr4::tree::TerminalNode *> DOT();
    antlr4::tree::TerminalNode* DOT(size_t i);
    Column_nameContext *column_name();
    Schema_nameContext *schema_name();
    Column_name_excluding_stringContext *column_name_excluding_string();
    antlr4::tree::TerminalNode *OPEN_PAR();
    Select_stmtContext *select_stmt();
    antlr4::tree::TerminalNode *CLOSE_PAR();
    antlr4::tree::TerminalNode *EXISTS_();
    antlr4::tree::TerminalNode *NOT_();
    Raise_functionContext *raise_function();
    Expr_recursiveContext *expr_recursive();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Expr_baseContext* expr_base();

  class  Expr_recursiveContext : public antlr4::ParserRuleContext {
  public:
    Expr_recursiveContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Function_nameContext *function_name();
    antlr4::tree::TerminalNode *OPEN_PAR();
    antlr4::tree::TerminalNode *CLOSE_PAR();
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);
    antlr4::tree::TerminalNode *STAR();
    Percentile_clauseContext *percentile_clause();
    Filter_clauseContext *filter_clause();
    Over_clauseContext *over_clause();
    antlr4::tree::TerminalNode *DISTINCT_();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    Order_clauseContext *order_clause();
    antlr4::tree::TerminalNode *CAST_();
    antlr4::tree::TerminalNode *AS_();
    Type_nameContext *type_name();
    antlr4::tree::TerminalNode *CASE_();
    antlr4::tree::TerminalNode *END_();
    std::vector<antlr4::tree::TerminalNode *> WHEN_();
    antlr4::tree::TerminalNode* WHEN_(size_t i);
    std::vector<antlr4::tree::TerminalNode *> THEN_();
    antlr4::tree::TerminalNode* THEN_(size_t i);
    antlr4::tree::TerminalNode *ELSE_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Expr_recursiveContext* expr_recursive();

  class  Raise_functionContext : public antlr4::ParserRuleContext {
  public:
    Raise_functionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *RAISE_();
    antlr4::tree::TerminalNode *OPEN_PAR();
    antlr4::tree::TerminalNode *CLOSE_PAR();
    antlr4::tree::TerminalNode *IGNORE_();
    antlr4::tree::TerminalNode *COMMA();
    Error_messageContext *error_message();
    antlr4::tree::TerminalNode *ROLLBACK_();
    antlr4::tree::TerminalNode *ABORT_();
    antlr4::tree::TerminalNode *FAIL_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Raise_functionContext* raise_function();

  class  Literal_valueContext : public antlr4::ParserRuleContext {
  public:
    Literal_valueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NUMERIC_LITERAL();
    antlr4::tree::TerminalNode *STRING_LITERAL();
    antlr4::tree::TerminalNode *BLOB_LITERAL();
    antlr4::tree::TerminalNode *NULL_();
    antlr4::tree::TerminalNode *TRUE_();
    antlr4::tree::TerminalNode *FALSE_();
    antlr4::tree::TerminalNode *CURRENT_TIME_();
    antlr4::tree::TerminalNode *CURRENT_DATE_();
    antlr4::tree::TerminalNode *CURRENT_TIMESTAMP_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Literal_valueContext* literal_value();

  class  Percentile_clauseContext : public antlr4::ParserRuleContext {
  public:
    Percentile_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *WITHIN_();
    antlr4::tree::TerminalNode *GROUP_();
    antlr4::tree::TerminalNode *OPEN_PAR();
    antlr4::tree::TerminalNode *ORDER_();
    antlr4::tree::TerminalNode *BY_();
    ExprContext *expr();
    antlr4::tree::TerminalNode *CLOSE_PAR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Percentile_clauseContext* percentile_clause();

  class  Value_rowContext : public antlr4::ParserRuleContext {
  public:
    Value_rowContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OPEN_PAR();
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);
    antlr4::tree::TerminalNode *CLOSE_PAR();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Value_rowContext* value_row();

  class  Values_clauseContext : public antlr4::ParserRuleContext {
  public:
    Values_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *VALUES_();
    std::vector<Value_rowContext *> value_row();
    Value_rowContext* value_row(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Values_clauseContext* values_clause();

  class  Insert_stmtContext : public antlr4::ParserRuleContext {
  public:
    Insert_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *INTO_();
    Table_nameContext *table_name();
    antlr4::tree::TerminalNode *INSERT_();
    antlr4::tree::TerminalNode *REPLACE_();
    antlr4::tree::TerminalNode *OR_();
    Select_stmtContext *select_stmt();
    antlr4::tree::TerminalNode *DEFAULT_();
    antlr4::tree::TerminalNode *VALUES_();
    With_clauseContext *with_clause();
    antlr4::tree::TerminalNode *ROLLBACK_();
    antlr4::tree::TerminalNode *ABORT_();
    antlr4::tree::TerminalNode *FAIL_();
    antlr4::tree::TerminalNode *IGNORE_();
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *AS_();
    Table_aliasContext *table_alias();
    antlr4::tree::TerminalNode *OPEN_PAR();
    std::vector<Column_nameContext *> column_name();
    Column_nameContext* column_name(size_t i);
    antlr4::tree::TerminalNode *CLOSE_PAR();
    Returning_clauseContext *returning_clause();
    std::vector<Upsert_clauseContext *> upsert_clause();
    Upsert_clauseContext* upsert_clause(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Insert_stmtContext* insert_stmt();

  class  Returning_clauseContext : public antlr4::ParserRuleContext {
  public:
    Returning_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *RETURNING_();
    std::vector<antlr4::tree::TerminalNode *> STAR();
    antlr4::tree::TerminalNode* STAR(size_t i);
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    std::vector<Column_aliasContext *> column_alias();
    Column_aliasContext* column_alias(size_t i);
    std::vector<antlr4::tree::TerminalNode *> AS_();
    antlr4::tree::TerminalNode* AS_(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Returning_clauseContext* returning_clause();

  class  Upsert_clauseContext : public antlr4::ParserRuleContext {
  public:
    Upsert_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ON_();
    antlr4::tree::TerminalNode *CONFLICT_();
    antlr4::tree::TerminalNode *DO_();
    antlr4::tree::TerminalNode *NOTHING_();
    antlr4::tree::TerminalNode *UPDATE_();
    antlr4::tree::TerminalNode *SET_();
    std::vector<antlr4::tree::TerminalNode *> ASSIGN();
    antlr4::tree::TerminalNode* ASSIGN(size_t i);
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);
    antlr4::tree::TerminalNode *OPEN_PAR();
    std::vector<Indexed_columnContext *> indexed_column();
    Indexed_columnContext* indexed_column(size_t i);
    antlr4::tree::TerminalNode *CLOSE_PAR();
    std::vector<Column_nameContext *> column_name();
    Column_nameContext* column_name(size_t i);
    std::vector<Column_name_listContext *> column_name_list();
    Column_name_listContext* column_name_list(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    std::vector<antlr4::tree::TerminalNode *> WHERE_();
    antlr4::tree::TerminalNode* WHERE_(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Upsert_clauseContext* upsert_clause();

  class  Pragma_stmtContext : public antlr4::ParserRuleContext {
  public:
    Pragma_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *PRAGMA_();
    Pragma_nameContext *pragma_name();
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *ASSIGN();
    Pragma_valueContext *pragma_value();
    antlr4::tree::TerminalNode *OPEN_PAR();
    antlr4::tree::TerminalNode *CLOSE_PAR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Pragma_stmtContext* pragma_stmt();

  class  Pragma_valueContext : public antlr4::ParserRuleContext {
  public:
    Pragma_valueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Signed_numberContext *signed_number();
    NameContext *name();
    antlr4::tree::TerminalNode *STRING_LITERAL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Pragma_valueContext* pragma_value();

  class  Reindex_stmtContext : public antlr4::ParserRuleContext {
  public:
    Reindex_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *REINDEX_();
    Collation_nameContext *collation_name();
    Table_nameContext *table_name();
    Index_nameContext *index_name();
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *DOT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Reindex_stmtContext* reindex_stmt();

  class  Select_stmtContext : public antlr4::ParserRuleContext {
  public:
    Select_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Select_coreContext *> select_core();
    Select_coreContext* select_core(size_t i);
    With_clauseContext *with_clause();
    std::vector<Compound_operatorContext *> compound_operator();
    Compound_operatorContext* compound_operator(size_t i);
    Order_clauseContext *order_clause();
    Limit_clauseContext *limit_clause();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Select_stmtContext* select_stmt();

  class  Join_clauseContext : public antlr4::ParserRuleContext {
  public:
    Join_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Table_or_subqueryContext *> table_or_subquery();
    Table_or_subqueryContext* table_or_subquery(size_t i);
    std::vector<Join_operatorContext *> join_operator();
    Join_operatorContext* join_operator(size_t i);
    std::vector<Join_constraintContext *> join_constraint();
    Join_constraintContext* join_constraint(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Join_clauseContext* join_clause();

  class  Select_coreContext : public antlr4::ParserRuleContext {
  public:
    SQLiteParser::ExprContext *where_expr = nullptr;
    SQLiteParser::ExprContext *exprContext = nullptr;
    std::vector<ExprContext *> group_by_expr;
    SQLiteParser::ExprContext *having_expr = nullptr;
    Select_coreContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SELECT_();
    std::vector<Result_columnContext *> result_column();
    Result_columnContext* result_column(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    antlr4::tree::TerminalNode *FROM_();
    Join_clauseContext *join_clause();
    antlr4::tree::TerminalNode *WHERE_();
    antlr4::tree::TerminalNode *GROUP_();
    antlr4::tree::TerminalNode *BY_();
    antlr4::tree::TerminalNode *WINDOW_();
    std::vector<Window_nameContext *> window_name();
    Window_nameContext* window_name(size_t i);
    std::vector<antlr4::tree::TerminalNode *> AS_();
    antlr4::tree::TerminalNode* AS_(size_t i);
    std::vector<Window_defnContext *> window_defn();
    Window_defnContext* window_defn(size_t i);
    antlr4::tree::TerminalNode *DISTINCT_();
    antlr4::tree::TerminalNode *ALL_();
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);
    antlr4::tree::TerminalNode *HAVING_();
    Values_clauseContext *values_clause();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Select_coreContext* select_core();

  class  Table_or_subqueryContext : public antlr4::ParserRuleContext {
  public:
    Table_or_subqueryContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Table_nameContext *table_name();
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *AS_();
    Table_aliasContext *table_alias();
    Table_alias_excluding_joinsContext *table_alias_excluding_joins();
    antlr4::tree::TerminalNode *INDEXED_();
    antlr4::tree::TerminalNode *BY_();
    Index_nameContext *index_name();
    antlr4::tree::TerminalNode *NOT_();
    Table_function_nameContext *table_function_name();
    antlr4::tree::TerminalNode *OPEN_PAR();
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);
    antlr4::tree::TerminalNode *CLOSE_PAR();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    Join_clauseContext *join_clause();
    Select_stmtContext *select_stmt();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Table_or_subqueryContext* table_or_subquery();

  class  Result_columnContext : public antlr4::ParserRuleContext {
  public:
    Result_columnContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STAR();
    Table_nameContext *table_name();
    antlr4::tree::TerminalNode *DOT();
    ExprContext *expr();
    Column_aliasContext *column_alias();
    antlr4::tree::TerminalNode *AS_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Result_columnContext* result_column();

  class  Join_operatorContext : public antlr4::ParserRuleContext {
  public:
    Join_operatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMA();
    antlr4::tree::TerminalNode *JOIN_();
    antlr4::tree::TerminalNode *NATURAL_();
    antlr4::tree::TerminalNode *INNER_();
    antlr4::tree::TerminalNode *CROSS_();
    antlr4::tree::TerminalNode *LEFT_();
    antlr4::tree::TerminalNode *RIGHT_();
    antlr4::tree::TerminalNode *FULL_();
    antlr4::tree::TerminalNode *OUTER_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Join_operatorContext* join_operator();

  class  Join_constraintContext : public antlr4::ParserRuleContext {
  public:
    Join_constraintContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ON_();
    ExprContext *expr();
    antlr4::tree::TerminalNode *USING_();
    antlr4::tree::TerminalNode *OPEN_PAR();
    std::vector<Column_nameContext *> column_name();
    Column_nameContext* column_name(size_t i);
    antlr4::tree::TerminalNode *CLOSE_PAR();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Join_constraintContext* join_constraint();

  class  Compound_operatorContext : public antlr4::ParserRuleContext {
  public:
    Compound_operatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *UNION_();
    antlr4::tree::TerminalNode *ALL_();
    antlr4::tree::TerminalNode *INTERSECT_();
    antlr4::tree::TerminalNode *EXCEPT_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Compound_operatorContext* compound_operator();

  class  Update_stmtContext : public antlr4::ParserRuleContext {
  public:
    Update_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *UPDATE_();
    Qualified_table_nameContext *qualified_table_name();
    antlr4::tree::TerminalNode *SET_();
    std::vector<antlr4::tree::TerminalNode *> ASSIGN();
    antlr4::tree::TerminalNode* ASSIGN(size_t i);
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);
    std::vector<Column_nameContext *> column_name();
    Column_nameContext* column_name(size_t i);
    std::vector<Column_name_listContext *> column_name_list();
    Column_name_listContext* column_name_list(size_t i);
    With_clauseContext *with_clause();
    antlr4::tree::TerminalNode *OR_();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    antlr4::tree::TerminalNode *FROM_();
    Join_clauseContext *join_clause();
    antlr4::tree::TerminalNode *WHERE_();
    Returning_clauseContext *returning_clause();
    Order_clauseContext *order_clause();
    Limit_clauseContext *limit_clause();
    antlr4::tree::TerminalNode *ROLLBACK_();
    antlr4::tree::TerminalNode *ABORT_();
    antlr4::tree::TerminalNode *REPLACE_();
    antlr4::tree::TerminalNode *FAIL_();
    antlr4::tree::TerminalNode *IGNORE_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Update_stmtContext* update_stmt();

  class  Column_name_listContext : public antlr4::ParserRuleContext {
  public:
    Column_name_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OPEN_PAR();
    std::vector<Column_nameContext *> column_name();
    Column_nameContext* column_name(size_t i);
    antlr4::tree::TerminalNode *CLOSE_PAR();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Column_name_listContext* column_name_list();

  class  Qualified_table_nameContext : public antlr4::ParserRuleContext {
  public:
    Qualified_table_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Table_nameContext *table_name();
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *DOT();
    antlr4::tree::TerminalNode *AS_();
    AliasContext *alias();
    antlr4::tree::TerminalNode *INDEXED_();
    antlr4::tree::TerminalNode *BY_();
    Index_nameContext *index_name();
    antlr4::tree::TerminalNode *NOT_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Qualified_table_nameContext* qualified_table_name();

  class  Vacuum_stmtContext : public antlr4::ParserRuleContext {
  public:
    Vacuum_stmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *VACUUM_();
    Schema_nameContext *schema_name();
    antlr4::tree::TerminalNode *INTO_();
    FilenameContext *filename();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Vacuum_stmtContext* vacuum_stmt();

  class  Filter_clauseContext : public antlr4::ParserRuleContext {
  public:
    Filter_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *FILTER_();
    antlr4::tree::TerminalNode *OPEN_PAR();
    antlr4::tree::TerminalNode *WHERE_();
    ExprContext *expr();
    antlr4::tree::TerminalNode *CLOSE_PAR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Filter_clauseContext* filter_clause();

  class  Window_defnContext : public antlr4::ParserRuleContext {
  public:
    Window_defnContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OPEN_PAR();
    antlr4::tree::TerminalNode *CLOSE_PAR();
    Base_window_nameContext *base_window_name();
    antlr4::tree::TerminalNode *PARTITION_();
    antlr4::tree::TerminalNode *BY_();
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);
    Order_clauseContext *order_clause();
    Frame_specContext *frame_spec();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Window_defnContext* window_defn();

  class  Over_clauseContext : public antlr4::ParserRuleContext {
  public:
    Over_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OVER_();
    Window_nameContext *window_name();
    antlr4::tree::TerminalNode *OPEN_PAR();
    antlr4::tree::TerminalNode *CLOSE_PAR();
    Base_window_nameContext *base_window_name();
    antlr4::tree::TerminalNode *PARTITION_();
    antlr4::tree::TerminalNode *BY_();
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);
    Order_clauseContext *order_clause();
    Frame_specContext *frame_spec();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Over_clauseContext* over_clause();

  class  Frame_specContext : public antlr4::ParserRuleContext {
  public:
    Frame_specContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Frame_clauseContext *frame_clause();
    antlr4::tree::TerminalNode *EXCLUDE_();
    antlr4::tree::TerminalNode *NO_();
    antlr4::tree::TerminalNode *OTHERS_();
    antlr4::tree::TerminalNode *CURRENT_();
    antlr4::tree::TerminalNode *ROW_();
    antlr4::tree::TerminalNode *GROUP_();
    antlr4::tree::TerminalNode *TIES_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Frame_specContext* frame_spec();

  class  Frame_clauseContext : public antlr4::ParserRuleContext {
  public:
    Frame_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *RANGE_();
    antlr4::tree::TerminalNode *ROWS_();
    antlr4::tree::TerminalNode *GROUPS_();
    Frame_singleContext *frame_single();
    antlr4::tree::TerminalNode *BETWEEN_();
    Frame_leftContext *frame_left();
    antlr4::tree::TerminalNode *AND_();
    Frame_rightContext *frame_right();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Frame_clauseContext* frame_clause();

  class  Order_clauseContext : public antlr4::ParserRuleContext {
  public:
    Order_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ORDER_();
    antlr4::tree::TerminalNode *BY_();
    std::vector<Ordering_termContext *> ordering_term();
    Ordering_termContext* ordering_term(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Order_clauseContext* order_clause();

  class  Limit_clauseContext : public antlr4::ParserRuleContext {
  public:
    Limit_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LIMIT_();
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);
    antlr4::tree::TerminalNode *OFFSET_();
    antlr4::tree::TerminalNode *COMMA();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Limit_clauseContext* limit_clause();

  class  Ordering_termContext : public antlr4::ParserRuleContext {
  public:
    Ordering_termContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExprContext *expr();
    antlr4::tree::TerminalNode *COLLATE_();
    Collation_nameContext *collation_name();
    Asc_descContext *asc_desc();
    antlr4::tree::TerminalNode *NULLS_();
    antlr4::tree::TerminalNode *FIRST_();
    antlr4::tree::TerminalNode *LAST_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Ordering_termContext* ordering_term();

  class  Asc_descContext : public antlr4::ParserRuleContext {
  public:
    Asc_descContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ASC_();
    antlr4::tree::TerminalNode *DESC_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Asc_descContext* asc_desc();

  class  Frame_leftContext : public antlr4::ParserRuleContext {
  public:
    Frame_leftContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExprContext *expr();
    antlr4::tree::TerminalNode *PRECEDING_();
    antlr4::tree::TerminalNode *FOLLOWING_();
    antlr4::tree::TerminalNode *CURRENT_();
    antlr4::tree::TerminalNode *ROW_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Frame_leftContext* frame_left();

  class  Frame_rightContext : public antlr4::ParserRuleContext {
  public:
    Frame_rightContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExprContext *expr();
    antlr4::tree::TerminalNode *PRECEDING_();
    antlr4::tree::TerminalNode *FOLLOWING_();
    antlr4::tree::TerminalNode *CURRENT_();
    antlr4::tree::TerminalNode *ROW_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Frame_rightContext* frame_right();

  class  Frame_singleContext : public antlr4::ParserRuleContext {
  public:
    Frame_singleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExprContext *expr();
    antlr4::tree::TerminalNode *PRECEDING_();
    antlr4::tree::TerminalNode *CURRENT_();
    antlr4::tree::TerminalNode *ROW_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Frame_singleContext* frame_single();

  class  Error_messageContext : public antlr4::ParserRuleContext {
  public:
    Error_messageContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExprContext *expr();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Error_messageContext* error_message();

  class  FilenameContext : public antlr4::ParserRuleContext {
  public:
    FilenameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExprContext *expr();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  FilenameContext* filename();

  class  Module_argumentContext : public antlr4::ParserRuleContext {
  public:
    Module_argumentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Module_argument_outerContext *> module_argument_outer();
    Module_argument_outerContext* module_argument_outer(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Module_argumentContext* module_argument();

  class  Module_argument_outerContext : public antlr4::ParserRuleContext {
  public:
    Module_argument_outerContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OPEN_PAR();
    antlr4::tree::TerminalNode *CLOSE_PAR();
    antlr4::tree::TerminalNode *UNEXPECTED_CHAR();
    antlr4::tree::TerminalNode *COMMA();
    std::vector<Module_argument_innerContext *> module_argument_inner();
    Module_argument_innerContext* module_argument_inner(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Module_argument_outerContext* module_argument_outer();

  class  Module_argument_innerContext : public antlr4::ParserRuleContext {
  public:
    Module_argument_innerContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OPEN_PAR();
    antlr4::tree::TerminalNode *CLOSE_PAR();
    antlr4::tree::TerminalNode *UNEXPECTED_CHAR();
    std::vector<Module_argument_innerContext *> module_argument_inner();
    Module_argument_innerContext* module_argument_inner(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Module_argument_innerContext* module_argument_inner();

  class  Fallback_excluding_conflictsContext : public antlr4::ParserRuleContext {
  public:
    Fallback_excluding_conflictsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ABORT_();
    antlr4::tree::TerminalNode *ACTION_();
    antlr4::tree::TerminalNode *AFTER_();
    antlr4::tree::TerminalNode *ALWAYS_();
    antlr4::tree::TerminalNode *ANALYZE_();
    antlr4::tree::TerminalNode *ASC_();
    antlr4::tree::TerminalNode *ATTACH_();
    antlr4::tree::TerminalNode *BEFORE_();
    antlr4::tree::TerminalNode *BEGIN_();
    antlr4::tree::TerminalNode *BY_();
    antlr4::tree::TerminalNode *CASCADE_();
    antlr4::tree::TerminalNode *CAST_();
    antlr4::tree::TerminalNode *COLUMN_();
    antlr4::tree::TerminalNode *CONFLICT_();
    antlr4::tree::TerminalNode *CURRENT_();
    antlr4::tree::TerminalNode *CURRENT_DATE_();
    antlr4::tree::TerminalNode *CURRENT_TIME_();
    antlr4::tree::TerminalNode *CURRENT_TIMESTAMP_();
    antlr4::tree::TerminalNode *DATABASE_();
    antlr4::tree::TerminalNode *DEFERRED_();
    antlr4::tree::TerminalNode *DESC_();
    antlr4::tree::TerminalNode *DETACH_();
    antlr4::tree::TerminalNode *DO_();
    antlr4::tree::TerminalNode *EACH_();
    antlr4::tree::TerminalNode *END_();
    antlr4::tree::TerminalNode *EXCEPT_();
    antlr4::tree::TerminalNode *EXCLUDE_();
    antlr4::tree::TerminalNode *EXCLUSIVE_();
    antlr4::tree::TerminalNode *EXPLAIN_();
    antlr4::tree::TerminalNode *FAIL_();
    antlr4::tree::TerminalNode *FALSE_();
    antlr4::tree::TerminalNode *FIRST_();
    antlr4::tree::TerminalNode *FOLLOWING_();
    antlr4::tree::TerminalNode *FOR_();
    antlr4::tree::TerminalNode *GENERATED_();
    antlr4::tree::TerminalNode *GLOB_();
    antlr4::tree::TerminalNode *GROUPS_();
    antlr4::tree::TerminalNode *IF_();
    antlr4::tree::TerminalNode *IGNORE_();
    antlr4::tree::TerminalNode *IMMEDIATE_();
    antlr4::tree::TerminalNode *INITIALLY_();
    antlr4::tree::TerminalNode *INSTEAD_();
    antlr4::tree::TerminalNode *INTERSECT_();
    antlr4::tree::TerminalNode *KEY_();
    antlr4::tree::TerminalNode *LAST_();
    antlr4::tree::TerminalNode *LIKE_();
    antlr4::tree::TerminalNode *MATCH_();
    antlr4::tree::TerminalNode *MATERIALIZED_();
    antlr4::tree::TerminalNode *NO_();
    antlr4::tree::TerminalNode *NULLS_();
    antlr4::tree::TerminalNode *OF_();
    antlr4::tree::TerminalNode *OFFSET_();
    antlr4::tree::TerminalNode *OTHERS_();
    antlr4::tree::TerminalNode *PARTITION_();
    antlr4::tree::TerminalNode *PLAN_();
    antlr4::tree::TerminalNode *PRAGMA_();
    antlr4::tree::TerminalNode *PRECEDING_();
    antlr4::tree::TerminalNode *QUERY_();
    antlr4::tree::TerminalNode *RANGE_();
    antlr4::tree::TerminalNode *RECURSIVE_();
    antlr4::tree::TerminalNode *REGEXP_();
    antlr4::tree::TerminalNode *REINDEX_();
    antlr4::tree::TerminalNode *RELEASE_();
    antlr4::tree::TerminalNode *RENAME_();
    antlr4::tree::TerminalNode *REPLACE_();
    antlr4::tree::TerminalNode *RESTRICT_();
    antlr4::tree::TerminalNode *ROLLBACK_();
    antlr4::tree::TerminalNode *ROW_();
    antlr4::tree::TerminalNode *ROWID_();
    antlr4::tree::TerminalNode *ROWS_();
    antlr4::tree::TerminalNode *SAVEPOINT_();
    antlr4::tree::TerminalNode *STORED_();
    antlr4::tree::TerminalNode *STRICT_();
    antlr4::tree::TerminalNode *TEMP_();
    antlr4::tree::TerminalNode *TEMPORARY_();
    antlr4::tree::TerminalNode *TIES_();
    antlr4::tree::TerminalNode *TRIGGER_();
    antlr4::tree::TerminalNode *TRUE_();
    antlr4::tree::TerminalNode *UNBOUNDED_();
    antlr4::tree::TerminalNode *UNION_();
    antlr4::tree::TerminalNode *VACUUM_();
    antlr4::tree::TerminalNode *VIEW_();
    antlr4::tree::TerminalNode *VIRTUAL_();
    antlr4::tree::TerminalNode *WITH_();
    antlr4::tree::TerminalNode *WITHIN_();
    antlr4::tree::TerminalNode *WITHOUT_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Fallback_excluding_conflictsContext* fallback_excluding_conflicts();

  class  Join_keywordContext : public antlr4::ParserRuleContext {
  public:
    Join_keywordContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CROSS_();
    antlr4::tree::TerminalNode *FULL_();
    antlr4::tree::TerminalNode *INDEXED_();
    antlr4::tree::TerminalNode *INNER_();
    antlr4::tree::TerminalNode *LEFT_();
    antlr4::tree::TerminalNode *NATURAL_();
    antlr4::tree::TerminalNode *OUTER_();
    antlr4::tree::TerminalNode *RIGHT_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Join_keywordContext* join_keyword();

  class  FallbackContext : public antlr4::ParserRuleContext {
  public:
    FallbackContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Fallback_excluding_conflictsContext *fallback_excluding_conflicts();
    Join_keywordContext *join_keyword();
    antlr4::tree::TerminalNode *RAISE_();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  FallbackContext* fallback();

  class  NameContext : public antlr4::ParserRuleContext {
  public:
    NameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  NameContext* name();

  class  Function_nameContext : public antlr4::ParserRuleContext {
  public:
    Function_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_name_excluding_raiseContext *any_name_excluding_raise();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Function_nameContext* function_name();

  class  Schema_nameContext : public antlr4::ParserRuleContext {
  public:
    Schema_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Schema_nameContext* schema_name();

  class  Table_nameContext : public antlr4::ParserRuleContext {
  public:
    Table_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Table_nameContext* table_name();

  class  Table_or_index_nameContext : public antlr4::ParserRuleContext {
  public:
    Table_or_index_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Table_or_index_nameContext* table_or_index_name();

  class  Column_nameContext : public antlr4::ParserRuleContext {
  public:
    Column_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Column_nameContext* column_name();

  class  Column_name_excluding_stringContext : public antlr4::ParserRuleContext {
  public:
    Column_name_excluding_stringContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_name_excluding_stringContext *any_name_excluding_string();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Column_name_excluding_stringContext* column_name_excluding_string();

  class  Column_aliasContext : public antlr4::ParserRuleContext {
  public:
    Column_aliasContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Column_aliasContext* column_alias();

  class  Collation_nameContext : public antlr4::ParserRuleContext {
  public:
    Collation_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Collation_nameContext* collation_name();

  class  Foreign_tableContext : public antlr4::ParserRuleContext {
  public:
    Foreign_tableContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Foreign_tableContext* foreign_table();

  class  Index_nameContext : public antlr4::ParserRuleContext {
  public:
    Index_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Index_nameContext* index_name();

  class  Trigger_nameContext : public antlr4::ParserRuleContext {
  public:
    Trigger_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Trigger_nameContext* trigger_name();

  class  View_nameContext : public antlr4::ParserRuleContext {
  public:
    View_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  View_nameContext* view_name();

  class  Module_nameContext : public antlr4::ParserRuleContext {
  public:
    Module_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Module_nameContext* module_name();

  class  Pragma_nameContext : public antlr4::ParserRuleContext {
  public:
    Pragma_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Pragma_nameContext* pragma_name();

  class  Savepoint_nameContext : public antlr4::ParserRuleContext {
  public:
    Savepoint_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Savepoint_nameContext* savepoint_name();

  class  Table_aliasContext : public antlr4::ParserRuleContext {
  public:
    Table_aliasContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Table_aliasContext* table_alias();

  class  Table_alias_excluding_joinsContext : public antlr4::ParserRuleContext {
  public:
    Table_alias_excluding_joinsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_name_excluding_joinsContext *any_name_excluding_joins();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Table_alias_excluding_joinsContext* table_alias_excluding_joins();

  class  Window_nameContext : public antlr4::ParserRuleContext {
  public:
    Window_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Window_nameContext* window_name();

  class  AliasContext : public antlr4::ParserRuleContext {
  public:
    AliasContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  AliasContext* alias();

  class  Base_window_nameContext : public antlr4::ParserRuleContext {
  public:
    Base_window_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Base_window_nameContext* base_window_name();

  class  Table_function_nameContext : public antlr4::ParserRuleContext {
  public:
    Table_function_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Any_nameContext *any_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Table_function_nameContext* table_function_name();

  class  Any_name_excluding_raiseContext : public antlr4::ParserRuleContext {
  public:
    Any_name_excluding_raiseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();
    Fallback_excluding_conflictsContext *fallback_excluding_conflicts();
    Join_keywordContext *join_keyword();
    antlr4::tree::TerminalNode *STRING_LITERAL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Any_name_excluding_raiseContext* any_name_excluding_raise();

  class  Any_name_excluding_joinsContext : public antlr4::ParserRuleContext {
  public:
    Any_name_excluding_joinsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();
    Fallback_excluding_conflictsContext *fallback_excluding_conflicts();
    antlr4::tree::TerminalNode *RAISE_();
    antlr4::tree::TerminalNode *STRING_LITERAL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Any_name_excluding_joinsContext* any_name_excluding_joins();

  class  Any_name_excluding_stringContext : public antlr4::ParserRuleContext {
  public:
    Any_name_excluding_stringContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();
    FallbackContext *fallback();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Any_name_excluding_stringContext* any_name_excluding_string();

  class  Any_nameContext : public antlr4::ParserRuleContext {
  public:
    Any_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();
    FallbackContext *fallback();
    antlr4::tree::TerminalNode *STRING_LITERAL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Any_nameContext* any_name();


  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

