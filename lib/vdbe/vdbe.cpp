#include <vector>

#include "../query.h"
#include "vdbe.h"

std::unique_ptr<std::vector<VDBEOpType>> vdbe_prepare(ParseQuery_t *parse_query) {
  std::unique_ptr<std::vector<VDBEOpType>> ops = std::make_unique<std::vector<VDBEOpType>>();
  switch (parse_query->query_type) {
    case CREATE_TABLE:
      (*ops).push_back(VDBE_OP_CREATE_TABLE);
      break;
    case SYSTEM_QUIT:
      (*ops).push_back(VDBE_OP_QUIT);
      break;
    default:
      (*ops).push_back(VDBE_OP_ERROR);
      break;
  }
  return ops;
}

void vdbe_execute(
  std::unique_ptr<std::vector<VDBEOpType>> ops,
  ParseQuery_t *parse_query
) {
  if (ops == nullptr)
    return;

  for (auto op: *ops) {
    // TODO(andrew): first build out btree (fake for now, just do linearly or something)
    // interface first. then link it to vdbe_execute() here      
  }
}