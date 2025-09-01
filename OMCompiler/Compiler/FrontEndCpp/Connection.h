#ifndef CONNECTION_H
#define CONNECTION_H

#include <iosfwd>

#include "MetaModelica.h"

#include "Connector.h"

namespace OpenModelica
{
  class Connection
  {
    public:
      explicit Connection(MetaModelica::Record value);

      const Connector& lhsConnector() const noexcept;
      Connector& lhsConnector() noexcept;
      const Connector& rhsConnector() const noexcept;
      Connector& rhsConnector() noexcept;

      bool isExpandable() const noexcept;
      bool isUndeclared() const noexcept;

    private:
      Connector _lhs;
      Connector _rhs;
  };

  std::ostream& operator<< (std::ostream &os, const Connection &connection);
}

#endif /* CONNECTION_H */
