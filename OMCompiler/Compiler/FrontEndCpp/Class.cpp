#include "Absyn/Class.h"
#include "Absyn/ClassDef.h"
#include "InstNode.h"
#include "Class.h"

using namespace OpenModelica;

extern record_description NFClass_PARTIAL__CLASS__desc;

constexpr int NOT_INSTANTIATED = 0;
constexpr int PARTIAL_CLASS = 1;
constexpr int PARTIAL_BUILTIN = 2;
constexpr int EXPANDED_CLASS = 3;
constexpr int EXPANDED_DERIVED = 4;
constexpr int INSTANCED_CLASS = 5;
constexpr int INSTANCED_BUILTIN = 6;
constexpr int TYPED_DERIVED = 7;
constexpr int DAE_TYPE = 8;

constexpr int PARTIAL_CLASS_ELEMENTS = 0;
constexpr int PARTIAL_CLASS_MODIFIER = 1;
constexpr int PARTIAL_CLASS_CC_MOD = 2;
constexpr int PARTIAL_CLASS_PREFIXES = 3;

constexpr int PARTIAL_BUILTIN_TY = 0;
constexpr int PARTIAL_BUILTIN_ELEMENTS = 1;
constexpr int PARTIAL_BUILTIN_MODIFIER = 2;
constexpr int PARTIAL_BUILTIN_PREFIXES = 3;
constexpr int PARTIAL_BUILTIN_RESTRICTION = 4;

constexpr int EXPANDED_CLASS_ELEMENTS = 0;
constexpr int EXPANDED_CLASS_MODIFIER = 1;
constexpr int EXPANDED_CLASS_CC_MOD = 2;
constexpr int EXPANDED_CLASS_PREFIXES = 3;
constexpr int EXPANDED_CLASS_RESTRICTION = 4;

constexpr int EXPANDED_DERIVED_BASE_CLASS = 0;
constexpr int EXPANDED_DERIVED_MODIFIER = 1;
constexpr int EXPANDED_DERIVED_CC_MOD = 2;
constexpr int EXPANDED_DERIVED_DIMS = 3;
constexpr int EXPANDED_DERIVED_PREFIXES = 4;
constexpr int EXPANDED_DERIVED_ATTRIBUTES = 5;
constexpr int EXPANDED_DERIVED_RESTRICTION = 6;

constexpr int INSTANCED_CLASS_TY = 0;
constexpr int INSTANCED_CLASS_ELEMENTS = 1;
constexpr int INSTANCED_CLASS_SECTIONS = 2;
constexpr int INSTANCED_CLASS_PREFIXES = 3;
constexpr int INSTANCED_CLASS_RESTRICTION = 4;

constexpr int INSTANCED_BUILTIN_TY = 0;
constexpr int INSTANCED_BUILTIN_ELEMENTS = 1;
constexpr int INSTANCED_BUILTIN_RESTRICTION = 2;

constexpr int TYPED_DERIVED_TY = 0;
constexpr int TYPED_DERIVED_BASE_CLASS = 1;
constexpr int TYPED_DERIVED_RESTRICTION = 2;

constexpr int DAE_TYPE_TY = 0;

extern record_description NFModifier_Modifier_NOMOD__desc;
extern record_description NFClass_Prefixes_PREFIXES__desc;

Class::Prefixes::Prefixes(MetaModelica::Record value)
  : encapsulated{value[0]},
    partial{value[1]},
    final{value[2]},
    innerOuter{value[3]},
    replaceable{value[4]}
{

}

Class::Class(MetaModelica::Record value)
  : _mm_value{value}
{

}

std::unique_ptr<Class> Class::fromMM(MetaModelica::Record value)
{
  switch (value.index()) {
    case PARTIAL_CLASS:     return std::make_unique<PartialClass>(value);
    case PARTIAL_BUILTIN:   return std::make_unique<PartialBuiltin>(value);
    case EXPANDED_CLASS:    return std::make_unique<ExpandedClass>(value);
    case EXPANDED_DERIVED:  return std::make_unique<ExpandedDerived>(value);
    case INSTANCED_CLASS:   return std::make_unique<InstancedClass>(value);
    case INSTANCED_BUILTIN: return std::make_unique<InstancedBuiltin>(value);
    case TYPED_DERIVED:     return std::make_unique<TypedDerived>(value);
    case DAE_TYPE:          return std::make_unique<DAEType>(value);
  }

  return nullptr;
}

std::unique_ptr<Class> Class::fromAbsyn(const Absyn::Class &cls, InstNode *scope)
{
  struct Visitor : public Absyn::ClassDefVisitor
  {
    Visitor(InstNode *scope) : scope{scope} {}

    void visit(const Absyn::ClassParts &def)
    {
      cls = std::make_unique<PartialClass>(def, false, scope);
    }

    //void visit(Absyn::ClassExtends &def) {
    //  classDef = std::make_unique<ClassTree>(def.composition(), true, scope);
    //}

    //void visit(Absyn::Enumeration &def) {
    //  classDef = std::make_unique<ClassTree>(def);
    //}

    InstNode *scope;
    std::unique_ptr<Class> cls;
  };

  Visitor visitor{scope};
  cls.definition().apply(visitor);
  return std::move(visitor.cls);
}

Class::~Class() = default;

bool Class::add(std::unique_ptr<InstNode> node)
{
  auto cls_tree = classTree();
  return cls_tree && cls_tree->add(std::move(node));
}

PartialClass::PartialClass(MetaModelica::Record value)
  : Class(value),
   _elements{value[PARTIAL_CLASS_ELEMENTS]},
   _prefixes{value[PARTIAL_CLASS_PREFIXES]}
{

}

PartialClass::PartialClass(const Absyn::ClassParts &definition, bool isClassExtends, InstNode *scope)
  : _elements{definition, isClassExtends, scope}
{

}

std::unique_ptr<Class> PartialClass::clone() const
{
  return std::make_unique<PartialClass>(*this);
}

MetaModelica::Value PartialClass::toMetaModelica() const
{
  static const MetaModelica::Record emptyMod(2, NFModifier_Modifier_NOMOD__desc, {});

  return MetaModelica::Record(PARTIAL_CLASS, NFClass_PARTIAL__CLASS__desc, {
    _elements.toNF(),
    emptyMod,
    emptyMod,
    MetaModelica::Record(0, NFClass_Prefixes_PREFIXES__desc, {
      _prefixes.encapsulated.toSCode(),
      _prefixes.partial.toSCode(),
      _prefixes.final.toSCode(),
      _prefixes.innerOuter.toAbsyn(),
      _prefixes.replaceable.toSCode()
    })
  });
}

PartialBuiltin::PartialBuiltin(MetaModelica::Record value)
  : Class(value),
    _ty{value[PARTIAL_BUILTIN_TY]},
    _elements{value[PARTIAL_BUILTIN_ELEMENTS]},
    _prefixes{value[PARTIAL_BUILTIN_PREFIXES]},
    _restriction{value[PARTIAL_BUILTIN_RESTRICTION]}
{

}

std::unique_ptr<Class> PartialBuiltin::clone() const
{
  return std::make_unique<PartialBuiltin>(*this);
}

MetaModelica::Value PartialBuiltin::toMetaModelica() const
{

}

ExpandedClass::ExpandedClass(MetaModelica::Record value)
  : Class(value),
    _elements{value[EXPANDED_CLASS_ELEMENTS]},
    _prefixes{value[EXPANDED_CLASS_PREFIXES]},
    _restriction{value[EXPANDED_CLASS_RESTRICTION]}
{

}

std::unique_ptr<Class> ExpandedClass::clone() const
{
  return std::make_unique<ExpandedClass>(*this);
}

MetaModelica::Value ExpandedClass::toMetaModelica() const
{

}

ExpandedDerived::ExpandedDerived(std::unique_ptr<InstNode> baseClass, std::vector<Dimension> dims,
  Prefixes prefixes, Restriction restriction)
  : _baseClass{std::move(baseClass)},
    _dims{std::move(dims)},
    _prefixes{prefixes},
    _restriction{restriction}
{

}

ExpandedDerived::ExpandedDerived(MetaModelica::Record value)
  : Class(value),
    _baseClass{InstNode::getOwnedPtr(value[EXPANDED_DERIVED_BASE_CLASS])},
    _dims{value[EXPANDED_DERIVED_DIMS].mapVector<Dimension>()},
    _prefixes{value[EXPANDED_DERIVED_PREFIXES]},
    _restriction{value[EXPANDED_DERIVED_PREFIXES]}
{

}

std::unique_ptr<Class> ExpandedDerived::clone() const
{
  return std::make_unique<ExpandedDerived>(_baseClass->clone(), _dims, _prefixes, _restriction);
}

const ClassTree* ExpandedDerived::classTree() const noexcept
{
  return _baseClass->getClass()->classTree();
}

ClassTree* ExpandedDerived::classTree() noexcept
{
  return _baseClass->getClass()->classTree();
}

MetaModelica::Value ExpandedDerived::toMetaModelica() const
{

}

InstancedClass::InstancedClass(MetaModelica::Record value)
  : Class(value),
    _ty{value[INSTANCED_CLASS_TY]},
    _elements{value[INSTANCED_CLASS_ELEMENTS]},
    _prefixes{value[INSTANCED_CLASS_PREFIXES]},
    _restriction{value[INSTANCED_CLASS_RESTRICTION]}
{

}

std::unique_ptr<Class> InstancedClass::clone() const
{
  return std::make_unique<InstancedClass>(*this);
}

MetaModelica::Value InstancedClass::toMetaModelica() const
{

}

InstancedBuiltin::InstancedBuiltin(MetaModelica::Record value)
  : Class(value),
    _ty{value[INSTANCED_BUILTIN_TY]},
    _elements{value[INSTANCED_BUILTIN_ELEMENTS]},
    _restriction{value[INSTANCED_BUILTIN_RESTRICTION]}
{

}

std::unique_ptr<Class> InstancedBuiltin::clone() const
{
  return std::make_unique<InstancedBuiltin>(*this);
}

MetaModelica::Value InstancedBuiltin::toMetaModelica() const
{

}

TypedDerived::TypedDerived(Type ty, std::unique_ptr<InstNode> baseClass, Restriction restriction)
  : _ty{ty}, _baseClass{std::move(baseClass)}, _restriction{restriction}
{

}

TypedDerived::TypedDerived(MetaModelica::Record value)
  : Class(value),
    _ty{value[TYPED_DERIVED_TY]},
    _baseClass{InstNode::getOwnedPtr(value[TYPED_DERIVED_BASE_CLASS])},
    _restriction{value[TYPED_DERIVED_RESTRICTION]}
{

}

std::unique_ptr<Class> TypedDerived::clone() const
{
  return std::make_unique<TypedDerived>(_ty, _baseClass->clone(), _restriction);
}

const ClassTree* TypedDerived::classTree() const noexcept
{
  return _baseClass->getClass()->classTree();
}

ClassTree* TypedDerived::classTree() noexcept
{
  return _baseClass->getClass()->classTree();
}

MetaModelica::Value TypedDerived::toMetaModelica() const
{

}

DAEType::DAEType(MetaModelica::Record value)
  : Class(value),
    _ty{value[DAE_TYPE_TY]}
{

}

std::unique_ptr<Class> DAEType::clone() const
{
  return std::make_unique<DAEType>(*this);
}

MetaModelica::Value DAEType::toMetaModelica() const
{

}

