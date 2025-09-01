#ifndef TYPE_H
#define TYPE_H

#include <string>
#include <string_view>
#include <vector>
#include <optional>
#include <memory>
#include <iosfwd>

#include "MetaModelica.h"

#include "Dimension.h"
#include "Path.h"

namespace OpenModelica
{
  class InstNode;
  class ComplexType;

  class TypeData
  {
    public:
      virtual ~TypeData() = default;

      virtual std::unique_ptr<TypeData> clone() const = 0;

      virtual bool isScalar() const { return true; }
      virtual bool isScalarBuiltin() const { return false; }
      virtual bool isBasic() const { return false; }
      virtual bool isNumeric() const { return false; }
      virtual bool isConnector() const { return false; }
      virtual bool isExpandableConnector() const { return false; }
      virtual bool isExternalObject() const { return false; }
      virtual bool isRecord() const { return false; }
      virtual bool isPolymorphic() const { return false; }
      virtual bool isPolymorphicNamed([[maybe_unused]] std::string_view name) const { return false; }

      virtual std::string str() const = 0;
  };

  class Type
  {
    public:
      enum Kind
      {
        // Builtin types
        Integer,
        Real,
        String,
        Boolean,
        Clock,
        // Special types without extra data
        NoRetCall,
        Unknown,
        Any,
        // Special types with extra data
        Enumeration,
        Tuple,
        Complex,
        Function,
        MetaBoxed,
        Polymorphic,
        ConditionalArray,
        Untyped
      };

    public:
      Type(Kind kind);
      Type(MetaModelica::Record value);
      Type(const Type &other);
      Type(Type &&other) noexcept;
      virtual ~Type() = default;

      Type& operator= (Type other);
      Type& operator= (Type &&other) noexcept;
      friend void swap(Type &first, Type &second) noexcept;

      bool isInteger() const;
      bool isReal() const;
      bool isBoolean() const;
      bool isString() const;
      bool isClock() const;
      bool isEnumeration() const;
      bool isScalar() const;
      bool isBasic() const;
      bool isBasicNumeric() const;
      bool isNumeric() const;
      bool isScalarBuiltin() const;
      bool isDiscrete() const;
      bool isArray() const;
      bool isConditionalArray() const;
      bool isVector() const;
      bool isMatrix() const;
      bool isSquareMatrix() const;
      bool isEmptyArray() const;
      bool isSingleElementArray() const;
      bool isComplex() const;
      bool isConnector() const;
      bool isExpandableConnector() const;
      bool isExternalObject() const;
      bool isRecord() const;
      bool isTuple() const;
      bool isUnknown() const;
      bool isKnown() const;
      bool isPolymorphic() const;
      bool isPolymorphicNamed(std::string_view name) const;

      Type elementType();
      //tcb::span<const Dimension> arrayDims() const;

      std::unique_ptr<Type> unliftArray(size_t n = 1) const;

      std::string typenameString() const;
      std::string str() const;

      friend std::ostream& operator<< (std::ostream &os, const Type &ty);

    private:
      Kind _kind;
      std::vector<Dimension> _dims;
      std::unique_ptr<TypeData> _data;
  };

  void swap(Type &first, Type &second) noexcept;

  std::ostream& operator<< (std::ostream &os, const Type &ty);

  class EnumerationTypeData : public TypeData
  {
    public:
      EnumerationTypeData(Path typePath, std::vector<std::string> literals);
      explicit EnumerationTypeData(MetaModelica::Record value);
      std::unique_ptr<TypeData> clone() const override;

      bool isBasic() const override { return true; }

      std::string str() const override;

    private:
      Path _typePath;
      std::vector<std::string> _literals;
  };

  class TupleTypeData : public TypeData
  {
    public:
      TupleTypeData(std::vector<Type> types, std::vector<std::string> names);
      TupleTypeData(MetaModelica::Record value);
      std::unique_ptr<TypeData> clone() const override;

      std::string str() const override;

    private:
      std::vector<Type> _types;
      std::vector<std::string> _names;
  };

  class ComplexTypeData : public TypeData
  {
    public:
      ComplexTypeData(InstNode *cls, std::unique_ptr<ComplexType> complexTy);
      ComplexTypeData(MetaModelica::Record value);
      std::unique_ptr<TypeData> clone() const override;

      bool isConnector() const override;
      bool isExpandableConnector() const override;
      bool isExternalObject() const override;
      bool isRecord() const override;

      std::string str() const override;

    private:
      InstNode* _cls;
      std::unique_ptr<ComplexType> _complexTy;
  };

  class FunctionTypeData : public TypeData
  {
    public:
      enum Kind
      {
        FunctionalParameter, // Function parameter of function type.
        FunctionReference,   // Function name used to reference a function.
        FunctionalVariable   // A variable that contains a function reference.
      };

      std::unique_ptr<TypeData> clone() const override;

      bool isBasic() const override;
      bool isScalarBuiltin() const override;

      std::string str() const override;

    private:
      //Function* _fn;
      Kind _kind;
  };

  class PolymorphicTypeData : public TypeData
  {
    public:
      PolymorphicTypeData(std::string name);
      PolymorphicTypeData(MetaModelica::Record value);
      std::unique_ptr<TypeData> clone() const override;

      bool isPolymorphic() const override { return true; }
      bool isPolymorphicNamed(std::string_view name) const override { return _name == name; }

      std::string str() const override;

    private:
      std::string _name;
  };

  class ConditionalArrayData : public TypeData
  {
    public:
      ConditionalArrayData(Type trueType, Type falseType, std::optional<bool> matchedBranch);
      ConditionalArrayData(MetaModelica::Record value);
      std::unique_ptr<TypeData> clone() const override;

      bool isScalar() const override { return false; }
      bool isNumeric() const override { return _trueType.isNumeric(); }

      std::string str() const override;

    private:
      Type _trueType;
      Type _falseType;
      std::optional<bool> _matchedBranch;
  };
}

#endif /* TYPE_H */
