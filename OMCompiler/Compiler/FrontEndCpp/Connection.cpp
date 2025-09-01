#include <ostream>

#include "Connection.h"

#include <iostream>
using namespace OpenModelica;

Connection::Connection(MetaModelica::Record value)
  : _lhs{value["lhs"]}, _rhs{value["rhs"]}
{
  // TODO: An expandable connector may only connect to another expandable connector.
  // TODO: Both sides can't be undeclared, one must be a declared component.
}

const Connector& Connection::lhsConnector() const noexcept
{
  return _lhs;
}

Connector& Connection::lhsConnector() noexcept
{
  return _lhs;
}

const Connector& Connection::rhsConnector() const noexcept
{
  return _rhs;
}

Connector& Connection::rhsConnector() noexcept
{
  return _rhs;
}

bool Connection::isExpandable() const noexcept
{
  return _lhs.isExpandable() && _rhs.isExpandable();
}

bool Connection::isUndeclared() const noexcept
{
  return _lhs.isUndeclared() || _rhs.isUndeclared();
}

std::ostream& OpenModelica::operator<< (std::ostream &os, const Connection &connection)
{
  os << "connect(" << connection.lhsConnector() << ", " << connection.rhsConnector() << ")";
  return os;
}
