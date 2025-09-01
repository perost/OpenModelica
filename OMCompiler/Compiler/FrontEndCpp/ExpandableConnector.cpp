#include <cassert>
#include <sstream>
#include <iostream>

#include "Util/DisjointSets.h"
#include "Util.h"

#include "InstNode.h"
#include "Class.h"
#include "Connection.h"
#include "ExpandableConnector.h"

using namespace OpenModelica;

void* ExpandableConnectors_elaborate_ext(void* flatModel, void* connections, void** outFlatModel)
{
  auto mm_connections = MetaModelica::Record{connections};
  auto mm_connections_lst = mm_connections["connections"].toList();
  auto conns = mm_connections_lst.mapVector<Connection>();

  //for (const auto &c: conns) {
  //  std::cout << c << std::endl;
  //}

  DisjointSets<Connector> csets;
  NonConnectorNode ctree(nullptr, nullptr);

  for (auto &c: conns) {
    if (c.isExpandable()) {
      ctree.find(&c.lhsConnector());
      ctree.find(&c.rhsConnector());
      csets.merge(c.lhsConnector(), c.rhsConnector());
    }
  }

  std::cout << "augment" << std::endl;
  for (auto &c: conns) {
    if (c.isUndeclared()) {
      bool reversed = c.lhsConnector().isUndeclared();
      auto &undecl_conn = reversed ? c.lhsConnector() : c.rhsConnector();
      auto &other_conn = reversed ? c.rhsConnector() : c.lhsConnector();

      auto undecl_node = ctree.find(&undecl_conn);

      if (undecl_node && undecl_node->parent()) {
        undecl_node->parent()->augment(other_conn);
      }
      std::cout << *undecl_node->parent() << std::endl;
    }
  }

  std::cout << "equalize" << std::endl;
  for (auto &c: conns) {
    if (c.isExpandable()) {
      std::cout << c << std::endl;
      auto c1 = dynamic_cast<ExpandableConnectorNode*>(ctree.find(&c.lhsConnector()));
      auto c2 = dynamic_cast<ExpandableConnectorNode*>(ctree.find(&c.rhsConnector()));
      assert(c1);
      assert(c2);

      std::cout << *c1 << std::endl;
      std::cout << *c2 << std::endl;

      c1->equalize(*c2);
      c2->equalize(*c1);
    }
  }

  //auto csets_array = csets.extractSets();
  //for (const auto &set: csets_array) {
  //  std::cout << Util::printList(set) << "\n";
  //}

  *outFlatModel = flatModel;
  return connections;
}

std::unique_ptr<ConnectorNode> ConnectorNode::create(ConnectorNode *parent, InstNode *node,
  const Type &ty, bool inExpandable, bool present)
{
  if (ty.isExpandableConnector()) {
    return std::make_unique<ExpandableConnectorNode>(parent, node, present);
  } else if (inExpandable) {
    return std::make_unique<ExpandableConnectorVariableNode>(parent, node, present);
  } else {
    return std::make_unique<NonConnectorNode>(parent, node);
  }
}

ConnectorNode* ConnectorNode::find(Connector *connector)
{
  ConnectorNode *node = this;
  bool in_expandable = false;

  for (auto& part: connector->name()) {
    auto name = in_expandable ? part.name() : part.node->name();
    auto it = node->_children.find(part.name());

    if (it != node->_children.end()) {
      node = it->second.get();
    } else {
      in_expandable = in_expandable || node->isExpandable();
      auto [i, b] = node->_children.try_emplace(part.name(),
        ConnectorNode::create(node, part.node, part.ty, in_expandable, true));
      node = i->second.get();
    }
  }

  return node;
}

ExpandableConnectorNode::ExpandableConnectorNode(ConnectorNode *parent, InstNode *node, bool present)
  : ConnectorNode(parent), _node{node}, _present{present}
{
  auto cls_tree = node->getClass()->classTree();

  if (cls_tree) {
    for (auto &c: cls_tree->components()) {
      _children.try_emplace(c->name(), ConnectorNode::create(this, c.get(), *c->type(), true, false));
    }
  }
}

bool ExpandableConnectorNode::augment(const Connector &connector)
{
  const auto conn_name = connector.name();
  const auto conn_node = conn_name.back().node;

  _node->getClass()->add(conn_node->clone());
  return true;
}

void ExpandableConnectorNode::equalize(const ExpandableConnectorNode &other)
{
  for (auto &it: other._children) {
    if (_children.find(it.first) == _children.end()) {
      std::cout << "Add " << it.first << " to " << _node->scopePath() << std::endl;
    }
  }
}

void ExpandableConnectorNode::print(std::ostream &os, int indent) const
{
  os << std::string(indent, ' ') << "expandable connector " << _node->scopePath() << " {\n";
  for (auto &it: _children) {
    it.second->print(os, indent + 2);
    os << '\n';
  }
  os << std::string(indent, ' ') << '}';
}

ExpandableConnectorVariableNode::ExpandableConnectorVariableNode(ConnectorNode *parent, InstNode *node, bool present)
  : ConnectorNode(parent), _node{node}, _present(present)
{

}

void ExpandableConnectorVariableNode::print(std::ostream &os, int indent) const
{
  os << std::string(indent, ' ') << "expandable connector var " << _node->scopePath();
}

NonConnectorNode::NonConnectorNode(ConnectorNode *parent, InstNode *node)
  : ConnectorNode(parent), _node{node}
{

}

void NonConnectorNode::print(std::ostream &os, int indent) const
{
  os << std::string(indent, ' ') << "non connector " << _node->scopePath();
}

std::ostream& OpenModelica::operator<< (std::ostream &os, const ConnectorNode &node)
{
  node.print(os);
  return os;
}
