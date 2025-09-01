#ifndef COMPLEXTYPE_H
#define COMPLEXTYPE_H

#include <memory>
#include <vector>

#include "MetaModelica.h"

namespace OpenModelica
{
  class InstNode;

  class ComplexType
  {
    public:
      virtual ~ComplexType() = default;

      virtual std::unique_ptr<ComplexType> clone() = 0;

      static std::unique_ptr<ComplexType> fromNF(MetaModelica::Record value);
  };

  class ExtendsType : public ComplexType
  {
    public:
      ExtendsType(InstNode *baseClass);
      ExtendsType(MetaModelica::Record value);

      std::unique_ptr<ComplexType> clone() override;

    private:
      InstNode *_baseClass;
  };

  class ConnectorType : public ComplexType
  {
    public:
      ConnectorType(std::vector<InstNode*> potentials, std::vector<InstNode*> flows, std::vector<InstNode*> streams);
      ConnectorType(MetaModelica::Record value);

      std::unique_ptr<ComplexType> clone() override;

    private:
      std::vector<InstNode*> _potentials;
      std::vector<InstNode*> _flows;
      std::vector<InstNode*> _streams;
  };

  class ExpandableConnectorType : public ComplexType
  {
    public:
      ExpandableConnectorType(std::vector<InstNode*> potentiallyPresents, std::vector<InstNode*> expandableConnectors);
      ExpandableConnectorType(MetaModelica::Record value);

      std::unique_ptr<ComplexType> clone() override;

    private:
      std::vector<InstNode*> _potentiallyPresents;
      std::vector<InstNode*> _expandableConnectors;
  };

  class RecordType : public ComplexType
  {
    public:
      RecordType(InstNode *constructor);
      RecordType(MetaModelica::Record value);

      std::unique_ptr<ComplexType> clone() override;

    private:
      InstNode *_constructor;
      //std::vector<Record.Field> _fields;
      //std::unordered_map<std::string, int> _indexMap;
  };

  class ExternalObjectType : public ComplexType
  {
    public:
      ExternalObjectType(InstNode *constructor, InstNode *destructor);
      ExternalObjectType(MetaModelica::Record value);

      std::unique_ptr<ComplexType> clone() override;

    private:
      InstNode *_constructor;
      InstNode *_destructor;
  };
}

#endif /* COMPLEXTYPE_H */
