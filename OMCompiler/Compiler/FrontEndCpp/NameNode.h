#ifndef NAMENODE_H
#define NAMENODE_H

#include "InstNode.h"

namespace OpenModelica
{
  class NameNode : public InstNode
  {
    public:
      NameNode(MetaModelica::Record value);
      NameNode(std::string name);
      ~NameNode();

      std::unique_ptr<InstNode> clone() const override;

      const std::string& name() const noexcept override;

      MetaModelica::Value toMetaModelica() const override;

    private:
      std::string _name;
  };
}

#endif /* NAMENODE_H */
