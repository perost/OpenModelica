#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#include "Absyn/AbsynFwd.h"
#include "MetaModelica.h"
#include "Prefixes.h"

namespace OpenModelica
{
  struct Attributes
  {
    explicit Attributes(MetaModelica::Record value);
    Attributes(const Absyn::ElementAttributes &attrs, const Absyn::ElementPrefixes &prefs);

    ConnectorType connectorType;
    Parallelism parallelism;
    Variability variability;
    Direction direction;
    InnerOuter innerOuter;
    bool isFinal;
    bool isRedeclare;
    //Replaceable replaceable;
    bool isResizable;
  };
}

#endif /* ATTRIBUTES_H */
