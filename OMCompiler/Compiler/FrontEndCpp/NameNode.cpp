#include "NameNode.h"

using namespace OpenModelica;

constexpr int NAME_INDEX = 0;

extern record_description NFInstNode_InstNode_NAME__NODE__desc;

NameNode::NameNode(MetaModelica::Record value)
  : _name{value[NAME_INDEX].toString()}
{

}

NameNode::NameNode(std::string name)
  : _name{std::move(name)}
{

}

NameNode::~NameNode() = default;

std::unique_ptr<InstNode> NameNode::clone() const
{
  return std::make_unique<NameNode>(*this);
}

const std::string& NameNode::name() const noexcept
{
  return _name;
}

MetaModelica::Value NameNode::toMetaModelica() const
{

}

