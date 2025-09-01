#include "Attributes.h"

using namespace OpenModelica;

constexpr int ATTRIBUTES_CONNECTOR_TYPE = 0;
constexpr int ATTRIBUTES_PARALLELISM = 1;
constexpr int ATTRIBUTES_VARIABILITY = 2;
constexpr int ATTRIBUTES_DIRECTION = 3;
constexpr int ATTRIBUTES_INNER_OUTER = 4;
constexpr int ATTRIBUTES_IS_FINAL = 5;
constexpr int ATTRIBUTES_IS_REDECLARE = 6;
constexpr int ATTRIBUTES_REPLACEABLE = 7;
constexpr int ATTRIBUTES_IS_RESIZABLE = 8;

Attributes::Attributes(MetaModelica::Record value)
  : connectorType{value[ATTRIBUTES_CONNECTOR_TYPE]},
    parallelism{value[ATTRIBUTES_PARALLELISM]},
    variability{value[ATTRIBUTES_VARIABILITY]},
    direction{value[ATTRIBUTES_DIRECTION]},
    innerOuter{value[ATTRIBUTES_INNER_OUTER]},
    isFinal{value[ATTRIBUTES_IS_FINAL].toBool()},
    isRedeclare{value[ATTRIBUTES_IS_REDECLARE].toBool()},
    isResizable{value[ATTRIBUTES_IS_RESIZABLE].toBool()}
{

}

Attributes::Attributes(const Absyn::ElementAttributes &attrs, const Absyn::ElementPrefixes &prefs)
{
  // TODO: Implement
}
