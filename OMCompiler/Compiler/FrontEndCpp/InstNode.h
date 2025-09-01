#ifndef INSTNODE_H
#define INSTNODE_H

#include <memory>

#include "Absyn/AbsynFwd.h"
#include "Util/MMCache.h"
#include "MetaModelica.h"
#include "InstNodeType.h"
#include "Path.h"

namespace OpenModelica
{
  class Class;
  class Type;

  std::unique_ptr<InstNode> node_from_mm(MetaModelica::Record value);

  class InstNode
  {
    public:
      static constexpr int CLASS_NODE = 0;
      static constexpr int COMPONENT_NODE = 1;
      static constexpr int INNER_OUTER_NODE = 2;
      static constexpr int REF_NODE = 3;
      static constexpr int NAME_NODE = 4;
      static constexpr int IMPLICIT_SCOPE = 5;
      static constexpr int ITERATOR_NODE = 6;
      static constexpr int VAR_NODE = 7;
      static constexpr int EMPTY_NODE = 8;

      static MetaModelica::Value emptyMMNode();

    public:
      virtual ~InstNode() = default;

      virtual std::unique_ptr<InstNode> clone() const = 0;

      bool isComponent() const noexcept { return false; }
      bool isClass() const noexcept { return false; }
      bool isRef() const noexcept { return false; }
      bool isRedeclare() const noexcept { return false; }
      bool isRootClass() const noexcept;

      virtual const std::string& name() const noexcept = 0;
      virtual Path scopePath() const;
      virtual const Absyn::Element* definition() const noexcept { return nullptr; }
      int refIndex() const noexcept { return 0; }

      virtual const InstNodeType* nodeType() const noexcept { return nullptr; }
      virtual void setNodeType(std::unique_ptr<InstNodeType>) noexcept {};

      virtual void setParent(InstNode*) noexcept {}

      virtual const Class* getClass() const noexcept { return nullptr; }
      virtual Class* getClass() noexcept { return nullptr; }

      virtual const Type* type() const noexcept { return nullptr; }
      virtual bool isExpandableConnector() const noexcept { return false; }

      virtual void partialInst() {}
      virtual void expand() {}
      virtual void instantiate() {}

      static std::unique_ptr<InstNode> getOwnedPtr(MetaModelica::Record value);
      static std::vector<std::unique_ptr<InstNode>> getOwnedPtrs(MetaModelica::Value value);
      static InstNode* getReference(MetaModelica::Record value);
      static std::vector<InstNode*> getReferences(MetaModelica::Value value);
      virtual MetaModelica::Value toMetaModelica() const = 0;

    protected:
      InstNode() = default;
      InstNode(const InstNode&) = default;
      InstNode(InstNode&&) noexcept = default;
      InstNode& operator=(const InstNode&) = default;
      InstNode& operator=(InstNode&&) noexcept = default;

      virtual void initialize() {}

    private:
      static std::unique_ptr<InstNode> fromMM(MetaModelica::Record value);

    private:
      friend class MMCache<InstNode>;
      static MMCache<InstNode> _cache;
  };


}

#endif /* INSTNODE_H */
