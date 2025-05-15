#include "parser.h"
#include "utils.h"
#include <cctype>
#include <memory>

Parser::Parser(const std::string& input) : input_(input), position_(0) {}

std::unique_ptr<ParsedStatement> Parser::parse() {
  std::string curr_token = readToken(); 
  auto stmt = std::make_unique<ParsedStatement>();

  while (curr_token.length() > 0) {
    DEBUG_PRINT(std::format("curr_token: {}", curr_token));
    if (Utils::compareNocase(curr_token, "create")) {
      curr_token = readToken();
      if (!Utils::compareNocase(curr_token, "table"))
        return nullptr;
      parseCreateTable(stmt);   
    }
    curr_token = readToken();
  }
  return stmt;
}

/* BEGIN: CREATE TABLE */
void Parser::parseCreateTable(std::unique_ptr<ParsedStatement>& stmt) {
  std::string curr_token = readToken();
  if (Utils::compareNocase(curr_token, "if")) {
    curr_token = readToken();
    if (!Utils::compareNocase(curr_token, "not")) {
      stmt->is_valid_stmt = false;
      return;
    }
    curr_token = readToken();
    if (!Utils::compareNocase(curr_token, "exists")) {
      stmt->is_valid_stmt = false;
      return;
    }
    stmt->if_not_exists = true;
    
    curr_token = readToken();
    DEBUG_PRINT(std::format("After IF NOT EXISTS token: {}", curr_token));
  }
  stmt->table_name = curr_token;
  DEBUG_PRINT(std::format("table name: {}", curr_token));

  // Table defn
  curr_token = readToken();
  if (!Utils::compareNocase(curr_token, "(")) {
    stmt->is_valid_stmt = false;
    return;
  }

  // Parse column defs
  std::vector<std::string> col_tokens;
  std::unique_ptr<ParsedColumnDef> col_def;
  curr_token = readToken();
  while (!Utils::compareNocase(curr_token, ")") && curr_token.length() > 0) {
    if (Utils::compareNocase(curr_token, ",")) {
      col_def = parseColDef(col_tokens);
      if (col_def == nullptr) {
        DEBUG_PRINT(std::format("ERR: parseColDef returned nullptr at L{}", std::to_string(__LINE__)));
        return;
      }
      stmt->column_defs.push_back(*col_def);
      col_tokens.clear();
    } else {
      col_tokens.push_back(curr_token);
    }
    curr_token = readToken();
    DEBUG_PRINT(std::format("token: {}", curr_token));
  }
  col_def = parseColDef(col_tokens);
  if (col_def == nullptr) {
    DEBUG_PRINT(std::format("ERR: parseColDef returned nullptr at L{}", std::to_string(__LINE__)));
    return;
  }
  stmt->is_valid_stmt = true;
  stmt->column_defs.push_back(*col_def);
  col_tokens.clear();
}

std::unique_ptr<ParsedColumnDef> Parser::parseColDef(std::vector<std::string> col_tokens) {
  #ifdef DEBUG
    for (auto s: col_tokens) {
      DEBUG_PRINT(std::format("begin parser token: {}", s));
    }
  #endif

  std::string reserved_keywords[] = {"primary", "key", "not", "null", "default"};
  if (col_tokens.empty())
    return nullptr;

  // Parse column name
  std::unique_ptr<ParsedColumnDef> col_def = std::make_unique<ParsedColumnDef>();
  col_def->col_name = col_tokens.front();
  col_tokens.erase(col_tokens.begin());

  if (col_tokens.empty())
    return nullptr;

  // Parse column type
  std::string raw_col_type = col_tokens.front();
  col_tokens.erase(col_tokens.begin());
  if (Utils::compareNocase(raw_col_type, "int")) {
    col_def->col_type = INTEGER;
  } else if (Utils::compareNocase(raw_col_type, "text")) {
    col_def->col_type = TEXT;
  } else if (Utils::compareNocase(raw_col_type, "bool")) {
    col_def->col_type = BOOL;
  } else {
    return nullptr;
  }

  // Parse column constraint
  std::string token;
  while (!col_tokens.empty()) {
    token = col_tokens.front();
    col_tokens.erase(col_tokens.begin());
    
    if (Utils::compareNocase(token, "primary")) {
      if (col_tokens.empty())
        return nullptr;
      token = col_tokens.front();
      col_tokens.erase(col_tokens.begin());
      if (!Utils::compareNocase(token, "key"))
        return nullptr;

      std::unique_ptr<DB_Constraint> col_constraint = std::make_unique<DB_Constraint>();
      col_constraint->type = PRIMARY_KEY;
      col_def->col_constraints.push_back(col_constraint);
    } else if (Utils::compareNocase(token, "unique")) {
      std::unique_ptr<DB_Constraint> col_constraint = std::make_unique<DB_Constraint>();
      col_constraint->type = UNIQUE;
      col_def->col_constraints.push_back(col_constraint);
    } else if (Utils::compareNocase(token, "not")) {
      if (col_tokens.empty())
        return nullptr;
      token = col_tokens.front();
      col_tokens.erase(col_tokens.begin());
      if (!Utils::compareNocase(token, "null"))
        return nullptr;

      std::unique_ptr<DB_Constraint> col_constraint = std::make_unique<DB_Constraint>();
      col_constraint->type = NOT_NULL;
      col_def->col_constraints.push_back(col_constraint);
    } else if (Utils::compareNocase(token, "check")) {
      
    }
  }
  Utils::printColDef(col_def);
  return col_def;
}
/* END: CREATE TABLE */


/* BEGIN: PARSER UTILS */
std::unique_ptr<DB_Expression> Parser::parseExpr(std::vector<std::string> tokens) {
  std::string token;
  std::unique_ptr<DB_Expression> expr = std::make_unique<DB_Expression>();
  while (!tokens.empty()) {
    token = tokens.front();
    tokens.erase(tokens.begin());
    if (Utils::compareNocase(token, "length")) {
      token = tokens.front();
      tokens.erase(tokens.begin());
      if (!Utils::compareNocase(token, "("))
        return nullptr;
      
      std::unique_ptr<DB_Expression> subexpr = parseExpr(tokens);
      if (subexpr == nullptr)
        return nullptr;
      expr->sym1 = std::move(subexpr);
      
      token = tokens.front();
      tokens.erase(tokens.begin());
      if (!Utils::compareNocase(token, ")"))
        return nullptr;
    
      token = tokens.front();
      tokens.erase(tokens.begin());
      if (Utils::compareNocase(token, "=")) {
        expr->op = EQ;
      } else if (Utils::compareNocase(token, "<")) {
        expr->op = LT;
      } else if (Utils::compareNocase(token, "<=")) {
        expr->op = LTE;
      } else if (Utils::compareNocase(token, ">")) {
        expr->op = GT;
      } else if (Utils::compareNocase(token, ">=")) {
        expr->op = GTE;
      } else if (Utils::compareNocase(token, "<>")) {
        expr->op = NEQ;
      } else {
        return nullptr;
      }

      subexpr = parseExpr(tokens);
      if (subexpr == nullptr)
        return nullptr;
      expr->sym2 = std::move(subexpr);
      return expr;
    } else {
      expr->op = LITERAL;
      expr->literal = token;

      std::string next_token = tokens.front();
      return expr;
    }
  }
}

std::string Parser::readToken() {
  while (position_ < input_.size() && std::isspace(input_[position_])) {
    position_++;
  }

  if (position_ >= input_.size()) {
    return "";
  }

  char ch = input_[position_];
  if (ch == '(' || ch == ')' || ch == ',') {
    position_++;
    return std::string(1, ch);
  }

  size_t start = position_;
  while (position_ < input_.size() && std::isalnum(input_[position_])) {
    position_++;
  }

  return input_.substr(start, position_ - start);
}
/* END: PARSER UTILS */