#include <ostream>
#include <stdexcept>

#include "Dimension.h"

using namespace OpenModelica;

constexpr int RAW_DIM = 0;
constexpr int UNTYPED = 1;
constexpr int INTEGER = 2;
constexpr int BOOLEAN = 3;
constexpr int ENUM = 4;
constexpr int EXP = 5;
constexpr int RESIZABLE = 6;
constexpr int UNKNOWN = 7;

std::unique_ptr<int> dimension_from_mm(MetaModelica::Record value)
{
  switch (value.index()) {
    case INTEGER: return std::make_unique<int>(value[0].toInt());
    case BOOLEAN: return std::make_unique<int>(2);
  }

  throw std::runtime_error("Unhandled record index in Dimension::Dimension");
}

Dimension::Dimension() = default;

Dimension::Dimension(MetaModelica::Record value)
  : _dim{dimension_from_mm(value)}
{

}

Dimension::Dimension(int size)
  : _dim{std::make_unique<int>(size)}
{

}

Dimension::Dimension(const Dimension &other)
  : _dim{other._dim ? std::make_unique<int>(*other._dim) : nullptr}
{

}

Dimension::Dimension(Dimension &&other) = default;

Dimension::~Dimension() = default;

Dimension& Dimension::operator= (Dimension other)
{
  swap(*this, other);
  return *this;
}

Dimension& Dimension::operator= (Dimension &&other) = default;

std::optional<int> Dimension::size() const noexcept
{
  return _dim ? std::make_optional(*_dim) : std::nullopt;
}

void OpenModelica::swap(Dimension &first, Dimension &second) noexcept
{
  using std::swap;
  swap(first._dim, second._dim);
}

std::ostream& OpenModelica::operator<< (std::ostream &os, const Dimension &dim)
{
  auto size = dim.size();
  os << (size ? *size : ':');
  return os;
}
