#include <string>
#include <cassert>
#include <vector>
#include <yaml-cpp/yaml.h>

#include "btree_builder.h"
#include "shared.h"

bool BTreeBuilder::validate_tree_config(YAML::Node tree_config, BTreeConfig config) {
  YAML::Node root = tree_config["root"];
  if (!root) return false;

  std::map<std::string, YAML::Node> config_nodes_map;
  for (const auto& entry : tree_config) {
    std::string key = entry.first.as<std::string>();
    YAML::Node node = entry.second;
    config_nodes_map[key] = node;
  }

  return BTreeBuilder::is_valid_tree_node(
    root,
    config_nodes_map,
    true,
    DefaultPagerKey::min_value,
    DefaultPagerKey::max_value,
    config
  );
}

bool BTreeBuilder::is_valid_tree_node(
  YAML::Node node,
  std::map<std::string, YAML::Node>& config_nodes_map,
  bool is_root,
  DefaultPagerKey min_value,
  DefaultPagerKey max_value,
  BTreeConfig config
) {
  YAML::Node cells = node["cells"];
  YAML::Node children = node["children"];

  if (!cells.IsSequence() || !children.IsSequence()) return false;

  if (children.size() != cells.size() + 1) return false;

  BTreeBuilderPageType children_page_type;
  std::vector<YAML::Node> child_nodes;

  // NOTE(andrew): B+Tree invariant, ceil(m/2) - 1 <= cells.size() <= m-1.
  // But root has no minimum number of nodes
  if (!is_root && (cells.size() < config.node_min_cells || config.node_max_cells < cells.size()))
    return false;

  if (is_root && config.node_max_cells < cells.size())
    return false;

  for (size_t i = 0; i < cells.size(); i++) {
    DefaultPagerKey key = cells[i].as<DefaultPagerKey>();
    if (key < min_value || max_value <= key) return false;
  }

  for (size_t i = 0; i < children.size(); i++) {
    std::string child_key = children[i].as<std::string>();
    if (config_nodes_map.find(child_key) == config_nodes_map.end()) return false;

    YAML::Node child_node = config_nodes_map[child_key];
    if(!child_node["page_type"]) return false;
    child_nodes.push_back(child_node);

    std::string raw_child_page_type = child_node["page_type"].as<std::string>();
    if (str_to_page_type.find(raw_child_page_type) == str_to_page_type.end()) return false;

    BTreeBuilderPageType child_page_type = str_to_page_type.at(raw_child_page_type);

    if (i == 0)
      children_page_type = child_page_type;
    else if (children_page_type != child_page_type)
      return false;
  }

  bool is_children_valid = true;
  for (size_t i = 0; i < children.size(); i++) {
    std::string child_key = children[i].as<std::string>();
    if (config_nodes_map.find(child_key) == config_nodes_map.end()) return false;

    DefaultPagerKey child_min_value = min_value;
    DefaultPagerKey child_max_value = max_value;
    if (i == children.size() - 1)
      child_min_value = cells[i-1].as<DefaultPagerKey>();
    else
      child_max_value = cells[i].as<DefaultPagerKey>(); 

    YAML::Node child_node = config_nodes_map[child_key];

    if (children_page_type == BTreeBuilderPageType::INTERNAL) {
      is_children_valid &= is_valid_tree_node(
        child_node,
        config_nodes_map,
        false,
        child_min_value,
        child_max_value,
        config
      );
    } else {
      assert(children_page_type == BTreeBuilderPageType::LEAF);
      is_children_valid &= is_valid_tree_leaf(
        child_node,
        config_nodes_map,
        child_min_value,
        child_max_value,
        config
      );
    }
  }
  return is_children_valid;
}

bool BTreeBuilder::is_valid_tree_leaf(
  YAML::Node leaf,
  std::map<std::string, YAML::Node>& config_nodes_map,
  DefaultPagerKey min_value,
  DefaultPagerKey max_value,
  BTreeConfig config
) {
  YAML::Node cells = leaf["cells"];

  if (!cells.IsSequence()) return false;

  // NOTE(andrew): B+Tree invariant, ceil(m/2) - 1 <= cells.size() <= m-1
  if (cells.size() < config.leaf_min_cells || config.leaf_max_cells < cells.size())
    return false;

  for (size_t i = 0; i < cells.size(); i++) {
    YAML::Node raw_key = cells[i]["key"];
    DefaultPagerKey key = raw_key.as<DefaultPagerKey>();
    YAML::Node payload = cells[i]["payload"];
    if (key < min_value || max_value <= key) return false;
    if (!raw_key || !payload) return false;
  }
  return true;
}
