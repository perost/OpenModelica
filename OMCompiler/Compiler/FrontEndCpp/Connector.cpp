#include <ostream>

#include "Connector.h"

using namespace OpenModelica;

extern record_description NFConnector_CONNECTOR__desc;

constexpr int NAME_INDEX = 0;
constexpr int TYPE_INDEX = 1;
constexpr int FACE_INDEX = 2;
constexpr int CTY_INDEX = 3;
constexpr int SOURCE_INDEX = 4;

Connector::Connector(MetaModelica::Record value)
  : _name{value[NAME_INDEX]},
    _face{value[FACE_INDEX].toInt() == 1 ? Face::Inside : Face::Outside},
    _cty{value[CTY_INDEX]},
    _source{value[SOURCE_INDEX]}
{

}

bool Connector::isUndeclared() const noexcept
{
  return _cty.isUndeclared();
}

bool Connector::isExpandable() const noexcept
{
  return _cty.isExpandable();
}

bool OpenModelica::operator== (const Connector &connector1, const Connector &connector2) noexcept
{
  // TODO: This should also compare the face of the connectors, but some
  //       alternative that doesn't check face is needed in some places.
  return connector1.name() == connector2.name();
}

namespace OpenModelica
{
  std::ostream& operator<< (std::ostream &os, const Connector &connector)
  {
    os << connector.face() << " " << connector.name();
    return os;
  }

  std::ostream& operator<< (std::ostream &os, Connector::Face face)
  {
    os << (face == Connector::Face::Inside ? "inside" : "outside");
    return os;
  }
}
