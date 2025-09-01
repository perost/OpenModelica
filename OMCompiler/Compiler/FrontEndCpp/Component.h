#ifndef COMPONENT_H
#define COMPONENT_H

#include <memory>

#include "Absyn/AbsynFwd.h"
#include "Attributes.h"
#include "Prefixes.h"
#include "Type.h"

namespace OpenModelica
{
  class InstNode;

  class Component
  {
    public:
      Component(Absyn::Component *definition);
      virtual ~Component();

      virtual std::unique_ptr<Component> clone() const = 0;

      static std::unique_ptr<Component> fromMM(MetaModelica::Record value);
      virtual MetaModelica::Value toNF() const noexcept = 0;

      virtual const InstNode* classInstance() const noexcept { return nullptr; }
      virtual InstNode* classInstance() noexcept {return nullptr; }

      virtual const Type* type() const noexcept { return nullptr; }

      virtual ConnectorType connectorType() const noexcept { return ConnectorType::None; }
      bool isExpandableConnector() const noexcept;

    protected:
      Component(const Component&) = default;
      Component(Component&&) noexcept = default;
      Component& operator=(const Component&) = default;
      Component& operator=(Component&&) noexcept = default;

    protected:
      Absyn::Component *_definition;
  };

  class ComponentDef : public Component
  {
    public:
      ComponentDef(MetaModelica::Record value);
      ComponentDef(Absyn::Component *component);

      std::unique_ptr<Component> clone() const override;

      MetaModelica::Value toNF() const noexcept override;

    private:
      // Modifier _modifier;
  };

  class NormalComponent : public Component
  {
    public:
      enum class State
      {
        PartiallyInstantiated, // Component instance has been created.
        FullyInstantiated,     // All component expressions have been instantiated.
        Typed,                 // The component's type has been determined.
        TypeChecked            // The component's binding has been typed and type checked.
      };

    public:
      NormalComponent(MetaModelica::Record value);
      NormalComponent(Absyn::Component *definition, std::unique_ptr<InstNode> classInst, Type ty,
                      Absyn::Comment *comment, State state);

      std::unique_ptr<Component> clone() const override;

      MetaModelica::Value toNF() const noexcept override;

      const InstNode* classInstance() const noexcept override;
      InstNode* classInstance() noexcept override;

      const Type* type() const noexcept override;

      ConnectorType connectorType() const noexcept override;

    private:
      std::unique_ptr<InstNode> _classInst;
      Type _ty;
      // Binding _binding;
      // Binding _condition;
      Attributes _attributes;
      Absyn::Comment *_comment;
      State _state = State::PartiallyInstantiated;
  };

  class MMComponent : public Component
  {
    public:
      MMComponent(MetaModelica::Record value);

      std::unique_ptr<Component> clone() const override;
      MetaModelica::Value toNF() const noexcept override;

    private:
      MetaModelica::Record _value;
  };
}

#endif /* COMPONENT_H */
