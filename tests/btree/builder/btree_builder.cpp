#include <string>
#include <cassert>
#include <vector>
#include <iostream>
#include <yaml-cpp/yaml.h>

#include "pager/base_page.h"
#include "shared.h"
#include "btree_builder.h"

PageNumber BTreeBuilder::build_tree(
  std::string path, Pager* pager, BTreeConfig config
) {
  YAML::Node yaml_config = YAML::LoadFile(path);

  bool is_btree_config_valid = BTreeBuilder::validate_tree_config(yaml_config, config);
  if (!is_btree_config_valid) return NULL_PAGE;

  std::vector<PageNumber> ordered_leaf_pages;
  std::map<std::string, YAML::Node> config_nodes_map;
  for (const auto& entry : yaml_config) {
    config_nodes_map[entry.first.as<std::string>()] = entry.second;
  }

  PageNumber root_pgno = BTreeBuilder::construct_tree_node(
    yaml_config["root"],
    config_nodes_map,
    pager,
    ordered_leaf_pages
  );

  for (size_t i = 0; i < ordered_leaf_pages.size()-1; i++) {
    PageNumber curr_page = ordered_leaf_pages[i];
    PageNumber next_page = ordered_leaf_pages[i+1];
  
    LeafPageManager curr_lpm(curr_page, pager->db_file_ptr_, pager);
    curr_lpm.set_next_page(next_page);

    LeafPageManager next_lpm(next_page, pager->db_file_ptr_, pager);
    next_lpm.set_prev_page(curr_page); 
  }
  return root_pgno;
}

PageNumber BTreeBuilder::construct_tree_node(
  YAML::Node node,
  std::map<std::string, YAML::Node>& config_nodes_map,
  Pager* pager,
  std::vector<PageNumber>& ordered_leaf_pages
) {
  assert(pager != NULL && pager->db_file_ptr_ != nullptr);

  YAML::Node cells = node["cells"];
  YAML::Node children = node["children"];

  assert(cells.size() + 1 == children.size());

  PageNumber node_pgno = pager->create_page(PAGER_NODE_PAGE);
  NodePageManager npm(node_pgno, pager->db_file_ptr_);

  for (size_t i = 0; i < children.size()-1; i++) {
    std::string child_node_name = children[i].as<std::string>();
    YAML::Node child_node = config_nodes_map.find(child_node_name)->second;

    DefaultPagerKey key = cells[i].as<DefaultPagerKey>();
    std::string raw_child_page_type = child_node["page_type"].as<std::string>();
    BTreeBuilderPageType child_page_type = str_to_page_type.at(raw_child_page_type);

    npm.insert_node_cell(key);

    PageNumber left_child_pgno;
    if (child_page_type == BTreeBuilderPageType::INTERNAL)
      left_child_pgno = BTreeBuilder::construct_tree_node(child_node, config_nodes_map, pager, ordered_leaf_pages);
    else
      left_child_pgno = BTreeBuilder::construct_tree_leaf(child_node, config_nodes_map, pager, ordered_leaf_pages);

    npm.set_cell_left_child(key, left_child_pgno);
  }

  std::string last_child_name = children[children.size()-1].as<std::string>();
  YAML::Node last_child_node = config_nodes_map.find(last_child_name)->second;

  std::string raw_child_page_type = last_child_node["page_type"].as<std::string>();
  BTreeBuilderPageType child_page_type = str_to_page_type.at(raw_child_page_type);

  PageNumber right_child_pgno;
  if (child_page_type == BTreeBuilderPageType::INTERNAL) {
    right_child_pgno = BTreeBuilder::construct_tree_node(
      last_child_node,
      config_nodes_map,
      pager,
      ordered_leaf_pages
    );
  } else {
    right_child_pgno = BTreeBuilder::construct_tree_leaf(
      last_child_node,
      config_nodes_map,
      pager,
      ordered_leaf_pages
    );
  }

  npm.set_node_right_child(right_child_pgno);
  return node_pgno;
}

PageNumber BTreeBuilder::construct_tree_leaf(
  YAML::Node leaf,
  std::map<std::string, YAML::Node>& config_nodes_map,
  Pager* pager,
  std::vector<PageNumber>& ordered_leaf_pages
) {
  assert(pager != NULL && pager->db_file_ptr_ != nullptr);

  YAML::Node cells = leaf["cells"];

  PageNumber leaf_pgno = pager->create_page(PAGER_LEAF_PAGE);
  LeafPageManager lpm(leaf_pgno, pager->db_file_ptr_, pager);

  for (size_t i = 0; i < cells.size(); i++) {
    YAML::Node child_node = cells[i];

    DefaultPagerKey leaf_key = child_node["key"].as<DefaultPagerKey>();
    std::string raw_payload = child_node["payload"].as<std::string>();
    std::vector<std::byte> payload(
      reinterpret_cast<const std::byte*>(raw_payload.data()),
      reinterpret_cast<const std::byte*>(raw_payload.data() + raw_payload.size())
    );

    lpm.insert_cell(leaf_key, payload);
    // TODO(andrew): we must also link together each leaf node's prev/next
    // page pointers. This needs to be done in 2 passes. Here, we want to
    // insert (min leaf key, curr pgno) into a heap or vector (+sort)
    // so that in second pass we can connect leaves together
  }
  ordered_leaf_pages.push_back(leaf_pgno);
  return leaf_pgno;
}
