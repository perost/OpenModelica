#ifndef SUBSCRIPT_H
#define SUBSCRIPT_H

#include <iosfwd>
#include <vector>

#include "MetaModelica.h"

namespace OpenModelica
{
  class Subscript
  {
    public:
      explicit Subscript(MetaModelica::Record value);
      ~Subscript();

      int index() const { return _index; }

      std::string str() const;
      std::size_t hash() const noexcept;

    private:
      int _index = 0;
  };

  bool operator== (const Subscript &subscript1, const Subscript &subscript2);

  std::ostream& operator<< (std::ostream &os, const Subscript &subscript);
  std::ostream& operator<< (std::ostream &os, const std::vector<Subscript> &subscripts);
}

template<>
struct std::hash<OpenModelica::Subscript>
{
  std::size_t operator() (const OpenModelica::Subscript &sub) const noexcept
  {
    return sub.hash();
  }
};

#endif /* SUBSCRIPT_H */
