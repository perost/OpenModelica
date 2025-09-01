#include <ostream>

#include "Util.h"
#include "Subscript.h"

using namespace OpenModelica;

constexpr int RAW_SUBSCRIPT_INDEX = 0;
constexpr int UNTYPED_INDEX = 1;
constexpr int INDEX_INDEX = 2;
constexpr int SLICE_INDEX = 3;
constexpr int EXPANDED_SLICE_INDEX = 4;
constexpr int WHOLE_INDEX = 5;
constexpr int SPLIT_PROXY_INDEX = 6;
constexpr int SPLIT_INDEX_INDEX = 7;

Subscript::Subscript(MetaModelica::Record value)
{
  if (value.index() == INDEX_INDEX) {
    auto index_exp = value[0].toRecord();
    _index = index_exp[0].toInt();
  }
}

Subscript::~Subscript() = default;

std::string Subscript::str() const
{
  return std::to_string(_index);
}

std::size_t Subscript::hash() const noexcept
{
  return _index;
}

bool OpenModelica::operator== (const Subscript &subscript1, const Subscript &subscript2)
{
  return subscript1.index() == subscript2.index();
}

std::ostream& OpenModelica::operator<< (std::ostream &os, const Subscript &subscript)
{
  os << subscript.index();
  return os;
}

std::ostream& OpenModelica::operator<< (std::ostream &os, const std::vector<Subscript> &subscripts)
{
  if (!subscripts.empty()) {
    os << '[' << Util::printList(subscripts) << ']';
  }

  return os;
}
