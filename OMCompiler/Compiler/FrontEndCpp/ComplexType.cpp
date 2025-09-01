#include "InstNode.h"
#include "ComplexType.h"

using namespace OpenModelica;

constexpr int EXTENDS_TYPE = 1;
constexpr int CONNECTOR = 2;
constexpr int EXPANDABLE_CONNECTOR = 3;
constexpr int RECORD = 4;
constexpr int EXTERNAL_OBJECT = 5;

std::unique_ptr<ComplexType> ComplexType::fromNF(MetaModelica::Record value)
{
  switch (value.index()) {
    case EXTENDS_TYPE:         return std::make_unique<ExtendsType>(value);
    case CONNECTOR:            return std::make_unique<ConnectorType>(value);
    case EXPANDABLE_CONNECTOR: return std::make_unique<ExpandableConnectorType>(value);
    case RECORD:               return std::make_unique<RecordType>(value);
    case EXTERNAL_OBJECT:      return std::make_unique<ExternalObjectType>(value);
  }

  return nullptr;
}

ExtendsType::ExtendsType(InstNode *baseClass)
  : _baseClass{baseClass}
{

}

ExtendsType::ExtendsType(MetaModelica::Record value)
  : _baseClass{InstNode::getReference(value[0])}
{

}

std::unique_ptr<ComplexType> ExtendsType::clone()
{
  return std::make_unique<ExtendsType>(_baseClass);
}

ConnectorType::ConnectorType(std::vector<InstNode*> potentials, std::vector<InstNode*> flows, std::vector<InstNode*> streams)
  : _potentials{std::move(potentials)}, _flows{std::move(flows)}, _streams{std::move(streams)}
{

}

ConnectorType::ConnectorType(MetaModelica::Record value)
  : _potentials{InstNode::getReferences(value[0])},
    _flows{InstNode::getReferences(value[1])},
    _streams{InstNode::getReferences(value[2])}
{

}

std::unique_ptr<ComplexType> ConnectorType::clone()
{
  return std::make_unique<ConnectorType>(_potentials, _flows, _streams);
}

ExpandableConnectorType::ExpandableConnectorType(std::vector<InstNode*> potentiallyPresents,
                                                 std::vector<InstNode*> expandableConnectors)
  : _potentiallyPresents{potentiallyPresents}, _expandableConnectors{expandableConnectors}
{

}

ExpandableConnectorType::ExpandableConnectorType(MetaModelica::Record value)
  : _potentiallyPresents{InstNode::getReferences(value[0])},
    _expandableConnectors{InstNode::getReferences(value[1])}
{

}

std::unique_ptr<ComplexType> ExpandableConnectorType::clone()
{
  return std::make_unique<ExpandableConnectorType>(_potentiallyPresents, _expandableConnectors);
}

RecordType::RecordType(InstNode *constructor)
  : _constructor{constructor}
{

}

RecordType::RecordType(MetaModelica::Record value)
  : _constructor{InstNode::getReference(value[0])}
{

}

std::unique_ptr<ComplexType> RecordType::clone()
{
  return std::make_unique<RecordType>(_constructor);
}

ExternalObjectType::ExternalObjectType(InstNode *constructor, InstNode *destructor)
  : _constructor{constructor}, _destructor{destructor}
{

}

ExternalObjectType::ExternalObjectType(MetaModelica::Record value)
  : _constructor{InstNode::getReference(value[0])},
    _destructor{InstNode::getReference(value[1])}
{

}

std::unique_ptr<ComplexType> ExternalObjectType::clone()
{
  return std::make_unique<ExternalObjectType>(_constructor, _destructor);
}

