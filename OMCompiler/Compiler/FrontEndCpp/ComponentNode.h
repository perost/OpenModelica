#ifndef COMPONENTNODE_H
#define COMPONENTNODE_H

#include <memory>
#include <optional>

#include "Absyn/AbsynFwd.h"
#include "InstNode.h"
#include "Prefixes.h"

namespace OpenModelica
{
  class Component;

  class ComponentNode : public InstNode
  {
    public:
      ComponentNode(MetaModelica::Record value);
      ComponentNode(Absyn::Component *definition, InstNode *parent);
      ComponentNode(Absyn::Component *definition, InstNode *parent, std::unique_ptr<InstNodeType> nodeType);
      ComponentNode(std::string name, Absyn::Component *definition, Visibility visibility,
                    std::unique_ptr<Component> _component, InstNode *parent, std::unique_ptr<InstNodeType> nodeType);
      ~ComponentNode();

      std::unique_ptr<InstNode> clone() const override;

      const std::string& name() const noexcept override { return _name; }
      Path scopePath() const override;
      const Absyn::Element* definition() const noexcept override;

      const InstNodeType* nodeType() const noexcept override;
      void setNodeType(std::unique_ptr<InstNodeType> nodeType) noexcept override;

      void setParent(InstNode *parent) noexcept override;

      const Class* getClass() const noexcept override;
      Class* getClass() noexcept override;

      const Type* type() const noexcept override;

      bool isExpandableConnector() const noexcept override;

      MetaModelica::Value toMetaModelica() const override;

    private:
      void initialize() override;

    private:
      std::string _name;
      Absyn::Component *_definition;
      Visibility _visibility;
      std::shared_ptr<Component> _component;
      InstNode *_parent;
      std::unique_ptr<InstNodeType> _nodeType;

      mutable std::optional<MetaModelica::Record> _mmCache;

      friend class MMSharedCache<Component>;
      static MMSharedCache<Component> _cache;
  };
}

#endif /* COMPONENTNODE_H */
