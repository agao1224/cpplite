
// Generated from grammar/SQLiteLexer.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  SQLiteLexer : public antlr4::Lexer {
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

  explicit SQLiteLexer(antlr4::CharStream *input);

  ~SQLiteLexer() override;


  std::string getGrammarFileName() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const std::vector<std::string>& getChannelNames() const override;

  const std::vector<std::string>& getModeNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;

  const antlr4::atn::ATN& getATN() const override;

  // By default the static state used to implement the lexer is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:

  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

};

