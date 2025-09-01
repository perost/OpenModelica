#include <sstream>
#include <algorithm>
#include <utility>
#include <stdexcept>
#include <cassert>

#include "Util.h"
#include "InstNode.h"
#include "ComplexType.h"
#include "Type.h"

using namespace OpenModelica;

constexpr int INTEGER = 0;
constexpr int REAL = 1;
constexpr int STRING = 2;
constexpr int BOOLEAN = 3;
constexpr int CLOCK = 4;
constexpr int ENUMERATION = 5;
constexpr int ARRAY = 7;
constexpr int TUPLE = 8;
constexpr int NORETCALL = 9;
constexpr int UNKNOWN = 10;
constexpr int COMPLEX = 11;
constexpr int FUNCTION = 12;
constexpr int METABOXED = 13;
constexpr int POLYMORPHIC = 14;
constexpr int ANY = 15;
constexpr int CONDITIONAL_ARRAY = 16;
constexpr int UNTYPED = 17;

Type::Kind kind_from_mm(MetaModelica::Record value)
{
  switch (value.index()) {
    case INTEGER:           return Type::Kind::Integer;
    case REAL:              return Type::Kind::Real;
    case STRING:            return Type::Kind::String;
    case BOOLEAN:           return Type::Kind::Boolean;
    case CLOCK:             return Type::Kind::Clock;
    case ENUMERATION:       return Type::Kind::Enumeration;
    case ARRAY:             return kind_from_mm(value[0]);
    case TUPLE:             return Type::Kind::Tuple;
    case NORETCALL:         return Type::Kind::NoRetCall;
    case UNKNOWN:           return Type::Kind::Unknown;
    case COMPLEX:           return Type::Kind::Complex;
    case FUNCTION:          return Type::Kind::Function;
    case METABOXED:         return Type::Kind::MetaBoxed;
    case POLYMORPHIC:       return Type::Kind::Polymorphic;
    case ANY:               return Type::Kind::Any;
    case CONDITIONAL_ARRAY: return Type::Kind::ConditionalArray;
    case UNTYPED:           return Type::Kind::Untyped;
  }

  throw std::runtime_error("Unknown record index in kind_from_mm");
}

std::vector<Dimension> dims_from_mm(MetaModelica::Record value)
{
  return value.index() == ARRAY ? value[1].mapVector<Dimension>() : std::vector<Dimension>{};
}

std::unique_ptr<TypeData> type_data_from_mm(MetaModelica::Record value)
{
  switch (value.index()) {
    case ENUMERATION:       return std::make_unique<EnumerationTypeData>(value);
    case ARRAY:             return type_data_from_mm(value[0]);
    case TUPLE:             return std::make_unique<TupleTypeData>(value);
    case COMPLEX:           return std::make_unique<ComplexTypeData>(value);
    case POLYMORPHIC:       return std::make_unique<PolymorphicTypeData>(value);
    case CONDITIONAL_ARRAY: return std::make_unique<ConditionalArrayData>(value);
    default: return nullptr;
  }
}

Type::Type(Kind kind)
  : _kind{kind}
{

}

Type::Type(MetaModelica::Record value)
  : _kind{kind_from_mm(value)}, _dims{dims_from_mm(value)}, _data{type_data_from_mm(value)}
{

}

Type::Type(const Type &other)
  : _kind{other._kind}, _dims{other._dims},
    _data{other._data ? other._data->clone() : nullptr}
{

}

Type::Type(Type &&other) = default;

Type& Type::operator= (Type other)
{
  swap(*this, other);
  return *this;
}

Type& Type::operator= (Type &&other) = default;

void OpenModelica::swap(Type &first, Type &second) noexcept
{
  using std::swap;
  swap(first._kind, second._kind);
  swap(first._dims, second._dims);
  swap(first._data, second._data);
}

bool Type::isInteger() const
{
  return _kind == Integer;
}

bool Type::isReal() const
{
  return _kind == Real;
}

bool Type::isBoolean() const
{
  return _kind == Boolean;
}

bool Type::isString() const
{
  return _kind == String;
}

bool Type::isClock() const
{
  return _kind == Clock;
}

bool Type::isEnumeration() const
{
  return _kind == Enumeration;
}

bool Type::isScalar() const
{
  return _dims.empty() || (_data && _data->isScalar());
}

bool Type::isBasic() const
{
  return _kind <= Clock || (_data && _data->isBasic());
}

bool Type::isBasicNumeric() const
{
  return _kind <= Real;
}

bool Type::isNumeric() const
{
  return isBasicNumeric() || (_data && _data->isNumeric());
}

bool Type::isScalarBuiltin() const
{
  return (isBasic() && isScalar()) || (_data && _data->isScalarBuiltin());
}

bool Type::isDiscrete() const
{
  return _kind <= Enumeration || (_data && _data->isScalarBuiltin());
}

bool Type::isArray() const
{
  return !_dims.empty() || (_data && !_data->isScalar());
}

bool Type::isConditionalArray() const
{
  return dynamic_cast<ConditionalArrayData*>(_data.get());
}

bool Type::isVector() const
{
  return _dims.size() == 1;
}

bool Type::isMatrix() const
{
  return _dims.size() == 2;
}

//bool Type::isSquareMatrix() const
//{
//  return _dims.size() == 2 && _dims[0] == _dims[1];
//}

//bool Type::isEmptyArray() const
//{
//  return std::any_of(std::begin(_dims), std::end(_dims),
//    [] (const Dimension &dim) { return dim.isZero(); });
//}

//bool Type::isSingleElementArray() const
//{
//  return _dims.size() == 1 && _dims[0].isKnown() && _dims[0].size() == 1;
//}

bool Type::isComplex() const
{
  return _kind == Complex;
}

bool Type::isConnector() const
{
  return _data && _data->isConnector();
}

bool Type::isExpandableConnector() const
{
  return _data && _data->isExpandableConnector();
}

bool Type::isExternalObject() const
{
  return _data && _data->isExternalObject();
}

bool Type::isRecord() const
{
  return _data && _data->isRecord();
}

bool Type::isTuple() const
{
  return _kind == Tuple;
}

bool Type::isUnknown() const
{
  return _kind == Unknown;
}

bool Type::isKnown() const
{
  return !isUnknown();
}

bool Type::isPolymorphic() const
{
  return _data && _data->isPolymorphic();
}

bool Type::isPolymorphicNamed(std::string_view name) const
{
  return _data && _data->isPolymorphicNamed(name);
}

Type Type::elementType()
{
  if (!_data) return _kind;

  throw std::runtime_error("TODO: implement Type::elementType");
}

std::string Type::str() const
{
  std::ostringstream ss;
  ss << *this;
  return ss.str();
}

std::ostream& OpenModelica::operator<< (std::ostream &os, const Type &ty)
{
  switch (ty._kind) {
    case Type::Integer: os << "Integer"; break;
    case Type::Real: os << "Real"; break;
    case Type::String: os << "String"; break;
    case Type::Boolean: os << "Boolean"; break;
    case Type::Clock: os << "Clock"; break;
    case Type::NoRetCall: os << "()"; break;
    case Type::Unknown: os << "unknown()"; break;
    case Type::Any: os << "$ANY$"; break;
    default: if (ty._data) os << ty._data->str();
  }

  if (!ty._dims.empty()) {
    os << '[' << Util::printList(ty._dims) << ']';
  }

  return os;
}

EnumerationTypeData::EnumerationTypeData(Path typePath, std::vector<std::string> literals)
  : _typePath{std::move(typePath)}, _literals{std::move(literals)}
{

}

EnumerationTypeData::EnumerationTypeData(MetaModelica::Record value)
  : _typePath{value[0]}, _literals{value[1].toVector<std::string>()}
{

}

std::unique_ptr<TypeData> EnumerationTypeData::clone() const
{
  return std::make_unique<EnumerationTypeData>(_typePath, _literals);
}

std::string EnumerationTypeData::str() const
{
  std::ostringstream ss;
  ss << "enumeration";

  if (_literals.empty()) {
    ss << "(:)";
  } else {
    ss << ' ' << _typePath << '(' << Util::printList(_literals) << ')';
  }

  return ss.str();
}

TupleTypeData::TupleTypeData(std::vector<Type> types, std::vector<std::string> names)
  : _types{std::move(types)}, _names{std::move(names)}
{

}

TupleTypeData::TupleTypeData(MetaModelica::Record value)
  : _types{value[0].mapVector<Type>()}, _names{value[1].toVector<std::string>()}
{

}

std::unique_ptr<TypeData> TupleTypeData::clone() const
{
  return std::make_unique<TupleTypeData>(_types, _names);
}

std::string TupleTypeData::str() const
{
  std::ostringstream ss;
  ss << '(' << Util::printList(_types) << ')';
  return ss.str();
}

ComplexTypeData::ComplexTypeData(InstNode *cls, std::unique_ptr<ComplexType> complexTy)
  : _cls{cls}, _complexTy{std::move(complexTy)}
{
  assert(cls);
}

ComplexTypeData::ComplexTypeData(MetaModelica::Record value)
  : _cls{InstNode::getReference(value[0])}, _complexTy{ComplexType::fromNF(value[1])}
{

}

std::unique_ptr<TypeData> ComplexTypeData::clone() const
{
  return std::make_unique<ComplexTypeData>(_cls, _complexTy ? _complexTy->clone() : nullptr);
}

bool ComplexTypeData::isConnector() const
{
  return dynamic_cast<const ConnectorType*>(_complexTy.get());
}

bool ComplexTypeData::isExpandableConnector() const
{
  return dynamic_cast<const ExpandableConnectorType*>(_complexTy.get());
}

bool ComplexTypeData::isExternalObject() const
{
  return dynamic_cast<const ExternalObjectType*>(_complexTy.get());
}

bool ComplexTypeData::isRecord() const
{
  return dynamic_cast<const RecordType*>(_complexTy.get());
}

std::string ComplexTypeData::str() const
{
  return _cls->name();
}

bool FunctionTypeData::isBasic() const
{
  // TODO: isBasic(Function.returnType(ty.fn))
  return false;
}

bool FunctionTypeData::isScalarBuiltin() const
{
  // TODO: isScalarBuiltin(Function.returnType(ty.fn))
  return false;
}

PolymorphicTypeData::PolymorphicTypeData(std::string name)
  : _name{std::move(name)}
{

}

PolymorphicTypeData::PolymorphicTypeData(MetaModelica::Record value)
  : _name{value[0].toString()}
{

}

std::unique_ptr<TypeData> PolymorphicTypeData::clone() const
{
  return std::make_unique<PolymorphicTypeData>(_name);
}

std::string PolymorphicTypeData::str() const
{
  if (_name.size() > 2 && _name[0] == '_' && _name[1] == '_') {
    return _name.substr(2);
  } else {
    return '<' + _name + '>';
  }
}

ConditionalArrayData::ConditionalArrayData(Type trueType, Type falseType, std::optional<bool> matchedBranch)
  : _trueType{std::move(trueType)}, _falseType{std::move(falseType)}, _matchedBranch{std::move(matchedBranch)}
{

}

ConditionalArrayData::ConditionalArrayData(MetaModelica::Record value)
  : _trueType{value[0]}, _falseType{value[1]}
{
  auto branch = value[2].toInt();
  if (branch > 1) {
    _matchedBranch = branch == 1;
  }
}

std::unique_ptr<TypeData> ConditionalArrayData::clone() const
{
  return std::make_unique<ConditionalArrayData>(_trueType, _falseType, _matchedBranch);
}

std::string ConditionalArrayData::str() const
{
  std::ostringstream ss;
  ss << _trueType << '|' << _falseType;
  return ss.str();
}
