#include <string>
#include <unordered_map>
#include <yaml-cpp/yaml.h>

#include "shared.h"
#include "pager/pager.h"
#include "btree/btree.h"

#pragma once

namespace YAML {
template<typename T>
struct convert<PagerKey<T>> {
  static Node encode(const PagerKey<T>& key) {
    return Node(key.value);
  }
  static bool decode(const Node& node, PagerKey<T>& key) {
    if (!node.IsScalar()) return false;
    key = PagerKey<T>(node.as<T>());
    return true;
  }
};
} // namespace YAML

enum class BTreeBuilderPageType {
  INTERNAL,
  LEAF
};

static const std::unordered_map<std::string, BTreeBuilderPageType> str_to_page_type = {
  {"internal", BTreeBuilderPageType::INTERNAL},
  {"leaf",     BTreeBuilderPageType::LEAF}
};

static const std::unordered_map<BTreeBuilderPageType, std::string> page_type_to_str = {
  {BTreeBuilderPageType::INTERNAL, "internal"},
  {BTreeBuilderPageType::LEAF,     "leaf"}
};

class BTreeBuilder {
  private:
    static bool validate_tree_config(YAML::Node tree_config, BTreeConfig config);
    static bool is_valid_tree_node(
      YAML::Node node,
      std::map<std::string, YAML::Node>& config_nodes_map,
      bool is_root,
      DefaultPagerKey min_value,
      DefaultPagerKey max_value,
      BTreeConfig config
    );
    static bool is_valid_tree_leaf(
      YAML::Node leaf,
      std::map<std::string, YAML::Node>& config_nodes_map,
      DefaultPagerKey min_value,
      DefaultPagerKey max_value,
      BTreeConfig config
    );

    static PageNumber construct_tree_node(
      YAML::Node node,
      std::map<std::string, YAML::Node>& config_nodes_map,
      Pager* pager,
      std::vector<PageNumber>& ordered_leaf_pages
    );
    static PageNumber construct_tree_leaf(
      YAML::Node node,
      std::map<std::string, YAML::Node>& config_nodes_map,
      Pager* pager,
      std::vector<PageNumber>& ordered_leaf_pages
    );

  public:
    static PageNumber build_tree(std::string path, Pager* pager, BTreeConfig config = {});
};
