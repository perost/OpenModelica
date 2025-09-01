#ifndef COMPONENTREF_H
#define COMPONENTREF_H

#include <iosfwd>

#include "MetaModelica.h"

#include "Subscript.h"
#include "Type.h"

namespace OpenModelica
{
  class InstNode;

  class ComponentRef
  {
    public:
      enum class Origin
      {
        Absyn,   // From an Absyn cref.
        Scope,   // From prefixing the cref with its scope.
        Iterator // From an iterator.
      };

      struct Part
      {
        Part(MetaModelica::Record value);
        Part(InstNode *node, std::vector<Subscript> subscripts, Type ty, Origin origin)
          : node{node}, subscripts{std::move(subscripts)}, ty{std::move(ty)}, origin{origin} {}

        const std::string& name() const;
        std::string str() const;

        InstNode *node;
        std::vector<Subscript> subscripts;
        Type ty;
        Origin origin;

        static inline const std::string wildcard = "_";
      };

      using PartList = std::vector<Part>;
      using PartIterator = PartList::iterator;
      using PartConstIterator = PartList::const_iterator;

    public:
      ComponentRef(std::vector<Part> parts);
      explicit ComponentRef(MetaModelica::Record value);
      ~ComponentRef();

      MetaModelica::Value toNF() const noexcept;

      const Part& front() const { return _parts.front(); }
      const Part& back() const { return _parts.back(); }

      PartIterator begin() noexcept { return _parts.begin(); }
      PartIterator end() noexcept { return _parts.end(); }
      PartConstIterator begin() const noexcept { return _parts.begin(); }
      PartConstIterator end() const noexcept  { return _parts.end(); }
      PartConstIterator cbegin() noexcept { return _parts.cbegin(); }
      PartConstIterator cend() const noexcept { return _parts.cend(); }

      std::string str() const;
      std::size_t hash() const noexcept;

    private:
      std::vector<Part> _parts;
  };

  void swap(ComponentRef::Part &first, ComponentRef::Part &second) noexcept;

  bool operator== (const ComponentRef::Part &part1, const ComponentRef::Part &part2) noexcept;
  bool operator== (const ComponentRef &cref1, const ComponentRef &cref2) noexcept;

  std::ostream& operator<< (std::ostream &os, const ComponentRef::Part &part);
  std::ostream& operator<< (std::ostream &os, const ComponentRef &cref);
}

template<>
struct std::hash<OpenModelica::ComponentRef>
{
  std::size_t operator() (const OpenModelica::ComponentRef &cref) const noexcept
  {
    return cref.hash();
  }
};

#endif /* COMPONENTREF_H */
