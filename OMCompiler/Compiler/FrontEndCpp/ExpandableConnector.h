#ifndef EXPANDABLECONNECTOR_H
#define EXPANDABLECONNECTOR_H

#include <string>
#include <unordered_map>
#include <iosfwd>

#include "MetaModelica.h"

namespace OpenModelica
{
  class ConnectorNode
  {
    public:
      static std::unique_ptr<ConnectorNode> create(ConnectorNode *parent, InstNode *node,
        const Type &ty, bool inExpandable, bool present);

      ConnectorNode(ConnectorNode *parent) : _parent{parent} {}
      virtual ~ConnectorNode() {};

      ConnectorNode* find(Connector *connector);
      virtual bool augment(const Connector &connector) { return false; }

      ConnectorNode* parent() { return _parent; }
      virtual std::string name() const = 0;
      virtual void print(std::ostream &os, int indent = 0) const = 0;

      virtual bool isExpandable() { return false; }

    protected:
      void addNode(InstNode &node);

    protected:
      ConnectorNode *_parent;
      std::unordered_map<std::string, std::unique_ptr<ConnectorNode>> _children;
  };

  class ExpandableConnectorNode : public ConnectorNode
  {
    public:
      explicit ExpandableConnectorNode(ConnectorNode *parent, InstNode *node, bool present);

      bool augment(const Connector &connector) override;
      void equalize(const ExpandableConnectorNode &other);

      bool isExpandable() override { return true; }

      std::string name() const override { return _node->name(); }
      void print(std::ostream &os, int indent = 0) const override;

    private:
      InstNode *_node;
      bool _present;
  };

  class ExpandableConnectorVariableNode : public ConnectorNode
  {
    public:
      ExpandableConnectorVariableNode(ConnectorNode *parent, InstNode *node, bool present);
      bool isPresent() const noexcept { return _present; }

      std::string name() const override { return _node ? _node->name() : "<no node>"; }
      void print(std::ostream &os, int indent = 0) const override;

    private:
      InstNode *_node;
      bool _present;
  };

  class NonConnectorNode : public ConnectorNode
  {
    public:
      NonConnectorNode(ConnectorNode *parent, InstNode *node);

      std::string name() const override { return _node ? _node->name() : "<no node>"; }
      void print(std::ostream &os, int indent = 0) const override;

    private:
      InstNode *_node;
  };

  std::ostream& operator<< (std::ostream &os, const ConnectorNode &node);
}

#ifdef __cplusplus
  extern "C" {
#endif

extern void* ExpandableConnectors_elaborate_ext(void* flatModel, void* connections, void** outFlatModel);

#ifdef __cplusplus
  }
#endif

#endif /* EXPANDABLECONNECTOR_H */
