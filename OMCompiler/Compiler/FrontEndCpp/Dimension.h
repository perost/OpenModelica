#ifndef DIMENSION_H
#define DIMENSION_H

#include <memory>
#include <optional>
#include <iosfwd>

#include "MetaModelica.h"

namespace OpenModelica
{
  class Dimension
  {
    public:
      Dimension();
      explicit Dimension(MetaModelica::Record value);
      explicit Dimension(int size);
      Dimension(const Dimension &other);
      Dimension(Dimension &&other);
      ~Dimension();

      Dimension& operator= (Dimension other);
      Dimension& operator= (Dimension &&other);
      friend void swap(Dimension &first, Dimension &second) noexcept;

      std::optional<int> size() const noexcept;

    private:
      std::unique_ptr<int> _dim;
  };

  void swap(Dimension &first, Dimension &second) noexcept;

  std::ostream& operator<< (std::ostream &os, const Dimension &dim);
}

#endif /* DIMENSION_H */
