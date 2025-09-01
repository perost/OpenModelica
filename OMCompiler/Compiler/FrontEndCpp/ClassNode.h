#ifndef CLASSNODE_H
#define CLASSNODE_H

#include "InstNode.h"

#include <optional>

namespace OpenModelica
{
  class ClassNode : public InstNode
  {
    public:
      ClassNode(MetaModelica::Record value);
      ClassNode(Absyn::Class *cls, InstNode *parent);
      ClassNode(Absyn::Class *cls, InstNode *parent, std::unique_ptr<InstNodeType> nodeType);
      ClassNode(std::string name, Absyn::Class *definition, Visibility visibility,
                std::unique_ptr<Class> cls, InstNode *parentScope, std::unique_ptr<InstNodeType> nodeType);
      ~ClassNode();

      std::unique_ptr<InstNode> clone() const override;

      //const Class* getClass() const noexcept override;

      const std::string& name() const noexcept override { return _name; }
      const Absyn::Element* definition() const noexcept override;

      const InstNodeType* nodeType() const noexcept override;
      void setNodeType(std::unique_ptr<InstNodeType> nodeType) noexcept override;

      void setParent(InstNode *parent) noexcept override;

      const Class* getClass() const noexcept override;
      Class* getClass() noexcept override;

      void partialInst() override;
      void expand() override;
      void instantiate() override;

      MetaModelica::Value toMetaModelica() const override;

    protected:
      void initialize() override;

    private:
      std::string _name;
      Absyn::Class *_definition;
      Visibility _visibility;
      // TODO: Change to unique_ptr once conversion from MetaModelica is no longer needed.
      std::shared_ptr<Class> _cls;
      // array<CachedData> caches;
      InstNode *_parentScope [[maybe_unused]];
      std::unique_ptr<InstNodeType> _nodeType;

      mutable std::optional<MetaModelica::Record> _mmCache;

      friend class MMSharedCache<Class>;
      static MMSharedCache<Class> _cache;
  };
}

#endif /* CLASSNODE_H */
