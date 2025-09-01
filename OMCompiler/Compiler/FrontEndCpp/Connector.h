#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <iosfwd>

#include "MetaModelica.h"

#include "ComponentRef.h"
#include "Prefixes.h"

namespace OpenModelica
{
  class Connector
  {
    public:
      enum class Face
      {
        Inside,
        Outside
      };

    public:
      explicit Connector(MetaModelica::Record value);

      const ComponentRef& name() const noexcept { return _name; }
      ComponentRef& name() noexcept { return _name; }
      Face face() const noexcept { return _face; }

      bool isUndeclared() const noexcept;
      bool isExpandable() const noexcept;

    private:
      ComponentRef _name;
      // Type _ty;
      Face _face;
      ConnectorType _cty;
      MetaModelica::Value _source;
  };

  bool operator== (const Connector &connector1, const Connector &connector2) noexcept;

  std::ostream& operator<< (std::ostream &os, const Connector &connector);
  std::ostream& operator<< (std::ostream &os, Connector::Face face);
}

template<>
struct std::hash<OpenModelica::Connector>
{
  std::size_t operator() (const OpenModelica::Connector &connector) const noexcept
  {
    return std::hash<OpenModelica::ComponentRef>{}(connector.name());
  }
};


#endif /* CONNECTOR_H */
