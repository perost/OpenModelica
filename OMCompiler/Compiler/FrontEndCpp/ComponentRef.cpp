#include <sstream>
#include <ostream>
#include <algorithm>

#include "Util.h"
#include "InstNode.h"
#include "ComponentRef.h"

using namespace OpenModelica;

constexpr int CREF = 0;
constexpr int EMPTY = 1;
constexpr int WILD = 2;

constexpr int NODE_INDEX = 0;
constexpr int SUBSCRIPTS_INDEX = 1;
constexpr int TY_INDEX = 2;
constexpr int ORIGIN_INDEX = 3;
constexpr int REST_CREF_INDEX = 4;

extern record_description NFComponentRef_CREF__desc;
extern record_description NFComponentRef_EMPTY__desc;
extern record_description NFComponentRef_WILD__desc;

ComponentRef::Part::Part(MetaModelica::Record value)
  : node{InstNode::getReference(value[NODE_INDEX])},
    subscripts{value[SUBSCRIPTS_INDEX].mapVector<Subscript>()},
    ty{value[TY_INDEX]},
    origin{value[ORIGIN_INDEX].toEnum<Origin>()}
{

}

const std::string& ComponentRef::Part::name() const
{
  return node ? node->name() : ComponentRef::Part::wildcard;
}

std::string ComponentRef::Part::str() const
{
  if (node) {
    std::ostringstream ss;
    ss << node->name() << subscripts;
    return ss.str();
  } else {
    return wildcard;
  }
}

ComponentRef::ComponentRef(std::vector<ComponentRef::Part> parts)
  : _parts{std::move(parts)}
{

}

ComponentRef::ComponentRef(MetaModelica::Record value)
{
  MetaModelica::Record v = value;

  while (v.index() == CREF) {
    _parts.emplace_back(v);
    v = v[REST_CREF_INDEX];
  }

  if (v.index() == WILD) {
    _parts.emplace_back(nullptr, std::vector<Subscript>(), Type::Unknown, Origin::Absyn);
  }

  std::reverse(_parts.begin(), _parts.end());
}

ComponentRef::~ComponentRef() = default;

MetaModelica::Value ComponentRef::toNF() const noexcept
{

}

std::string ComponentRef::str() const
{
  std::ostringstream ss;
  ss << *this;
  return ss.str();
}

std::size_t ComponentRef::hash() const noexcept
{
  std::size_t hash = 0;

  for (const auto &part: _parts) {
    if (part.node) {
      Util::hashCombine(hash, part.node->name());
      for (const auto &sub: part.subscripts) {
        Util::hashCombine(hash, sub);
      }
    } else {
      hash = '_';
    }
  }

  return hash;
}

void OpenModelica::swap(ComponentRef::Part &part1, ComponentRef::Part &part2) noexcept
{
  using std::swap;
  swap(part1.node, part2.node);
  swap(part1.subscripts, part2.subscripts);
  swap(part1.ty, part2.ty);
  swap(part1.origin, part2.origin);
}

bool OpenModelica::operator== (const ComponentRef::Part &part1, const ComponentRef::Part &part2) noexcept
{
  return (part1.node == part2.node ||
          (part1.node && part2.node && part1.node->name() == part2.node->name())) &&
         std::equal(part1.subscripts.begin(), part1.subscripts.end(),
                    part2.subscripts.begin(), part2.subscripts.end());
}

bool OpenModelica::operator== (const ComponentRef &cref1, const ComponentRef &cref2) noexcept
{
  return std::equal(cref1.begin(), cref1.end(), cref2.begin(), cref2.end());
}

std::ostream& OpenModelica::operator<< (std::ostream &os, const ComponentRef::Part &part)
{
  if (!part.node) {
    os << "_";
  } else {
    os << part.node->name() << part.subscripts;
  }

  return os;
}

std::ostream& OpenModelica::operator<< (std::ostream &os, const ComponentRef &cref)
{
  os << Util::printList(cref.begin(), cref.end(), ".");
  return os;
}
