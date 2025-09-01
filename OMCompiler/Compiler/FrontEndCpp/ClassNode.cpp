#include "Absyn/Class.h"
#include "Class.h"
#include "ClassNode.h"

using namespace OpenModelica;

extern record_description NFInstNode_InstNode_CLASS__NODE__desc;

extern record_description NFClass_NOT__INSTANTIATED__desc;

constexpr int NAME_INDEX = 0;
constexpr int DEFINITION_INDEX = 1;
constexpr int VISIBILITY_INDEX = 2;
constexpr int CLS_INDEX = 3;
constexpr int CACHES_INDEX = 4;
constexpr int PARENT_SCOPE_INDEX = 5;
constexpr int NODE_TYPE_INDEX = 6;

extern record_description NFInstNode_CachedData_NO__CACHE__desc;

MMSharedCache<Class> ClassNode::_cache;

ClassNode::ClassNode(MetaModelica::Record value)
  : _name{value[NAME_INDEX].toString()},
    _definition{nullptr},
    _visibility{value[VISIBILITY_INDEX]},
    // _cls is initialized by initialize()
    _parentScope{InstNode::getReference(value[PARENT_SCOPE_INDEX])},
    // _nodeType is initialized by initialize()
    _mmCache{value}
{

}

ClassNode::ClassNode(Absyn::Class *cls, InstNode *parent)
  : ClassNode(cls, parent, std::make_unique<NormalClassType>())
{

}

ClassNode::ClassNode(Absyn::Class *cls, InstNode *parent, std::unique_ptr<InstNodeType> nodeType)
  : _name{cls ? cls->name() : ""},
    _definition{cls},
    _visibility{cls ? cls->prefixes().visibility() : Visibility::Public},
    _parentScope{parent},
    _nodeType{std::move(nodeType)}
{

}

ClassNode::ClassNode(std::string name, Absyn::Class *definition, Visibility visibility,
  std::unique_ptr<Class> cls, InstNode *parentScope, std::unique_ptr<InstNodeType> nodeType)
  : _name{name},
    _definition{definition},
    _visibility{visibility},
    _cls{std::move(cls)},
    _parentScope{parentScope},
    _nodeType{std::move(nodeType)}
{

}

ClassNode::~ClassNode() = default;

std::unique_ptr<InstNode> ClassNode::clone() const
{
  return std::make_unique<ClassNode>(_name, _definition, _visibility, _cls ? _cls->clone() : nullptr,
    _parentScope, _nodeType ? _nodeType->clone() : nullptr);
}

const Absyn::Element* ClassNode::definition() const noexcept
{
  return _definition;
}

const InstNodeType* ClassNode::nodeType() const noexcept
{
  return _nodeType.get();
}

void ClassNode::setNodeType(std::unique_ptr<InstNodeType> nodeType) noexcept
{
  _nodeType = std::move(nodeType);
  _mmCache.reset();
}

void ClassNode::setParent(InstNode *parent) noexcept
{
  _parentScope = parent;
}

const Class* ClassNode::getClass() const noexcept
{
  return _cls.get();
}

Class* ClassNode::getClass() noexcept
{
  return _cls.get();
}

void ClassNode::partialInst()
{
  if (_cls) return;
  _cls = Class::fromAbsyn(*_definition, this);
  _mmCache.reset();
}

void ClassNode::expand()
{

}

void ClassNode::instantiate()
{

}

MetaModelica::Value ClassNode::toMetaModelica() const
{
  static const MetaModelica::Value noClass = MetaModelica::Record(0, NFClass_NOT__INSTANTIATED__desc);
  static const MetaModelica::Value noCache = MetaModelica::Record(0, NFInstNode_CachedData_NO__CACHE__desc);

  // Nodes need to be cached to deal with the cyclical dependencies between nodes.
  if (!_mmCache) {
    auto cls_ptr = MetaModelica::Pointer();

    // Create a MetaModelica record for the node.
    auto rec = MetaModelica::Record(InstNode::CLASS_NODE, NFInstNode_InstNode_CLASS__NODE__desc, {
      MetaModelica::Value(_name),
      _definition->toSCode(),
      _visibility.toSCode(),
      cls_ptr,
      MetaModelica::Array{3, noCache},
      InstNode::emptyMMNode(),
      MetaModelica::Value(static_cast<int64_t>(0))
    });

    // Set the cache.
    _mmCache = rec;

    // Set any of the record members that might depend on this node now that we have a cached value.
    cls_ptr.update(_cls ? _cls->toMetaModelica() : noClass);
    if (_parentScope) rec.set(PARENT_SCOPE_INDEX, _parentScope->toMetaModelica());
    rec.set(NODE_TYPE_INDEX, _nodeType->toMetaModelica());
  }

  return *_mmCache;
}

void ClassNode::initialize()
{
  if (_mmCache) {
    auto value = *_mmCache;
    _cls = _cache.getOwnedPtr(value[CLS_INDEX].toPointer().access());
    _nodeType = InstNodeType::fromNF(value[NODE_TYPE_INDEX]);
  }
}
