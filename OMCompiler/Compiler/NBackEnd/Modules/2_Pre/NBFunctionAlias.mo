/*
* This file is part of OpenModelica.
*
* Copyright (c) 1998-2021, Open Source Modelica Consortium (OSMC),
* c/o Linköpings universitet, Department of Computer and Information Science,
* SE-58183 Linköping, Sweden.
*
* All rights reserved.
*
* THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR
* THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.2.
* ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
* RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3,
* ACCORDING TO RECIPIENTS CHOICE.
*
* The OpenModelica software and the Open Source Modelica
* Consortium (OSMC) Public License (OSMC-PL) are obtained
* from OSMC, either from the above address,
* from the URLs: http://www.ida.liu.se/projects/OpenModelica or
* http://www.openmodelica.org, and in the OpenModelica distribution.
* GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
*
* This program is distributed WITHOUT ANY WARRANTY; without
* even the implied warranty of  MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
* IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
*
* See the full OSMC Public License conditions for more details.
*
*/
encapsulated package NBFunctionAlias
"file:        NBFunctionAlias.mo
 package:     NBFunctionAlias
 description: This file contains the functions for the function alias encapsulation module.
"
public
  import Module = NBModule;
protected
  // OF imports
  import Absyn;
  import AbsynUtil;
  import DAE;

  // NF imports
  import BackendExtension = NFBackendExtension;
  import Call = NFCall;
  import ComponentRef = NFComponentRef;
  import Dimension = NFDimension;
  import Expression = NFExpression;
  import NFFunction.Function;
  import Subscript = NFSubscript;
  import Type = NFType;
  import Variable = NFVariable;

  // Backend imports
  import BackendDAE = NBackendDAE;
  import BEquation = NBEquation;
  import Inline = NBInline;
  import NBEquation.{Equation, EquationPointers, EqData, EquationAttributes, EquationKind, Iterator};
  import BVariable = NBVariable;
  import NBVariable.{VariablePointers, VarData};

  // Util imports
  import UnorderedMap;
public
  function main
    "Wrapper function for any function alias introduction function. This will be
     called during simulation and gets the corresponding subfunction from
     Config."
    extends Module.wrapper;
  protected
    Module.aliasInterface func;
  algorithm
    (func) := getModule();

    bdae := match bdae
      local
        VarData varData         "Data containing variable pointers";
        EqData eqData           "Data containing equation pointers";

      case BackendDAE.MAIN(varData = varData, eqData = eqData)
        algorithm
          (varData, eqData) := func(varData, eqData);
          bdae.varData := varData;
          bdae.eqData := eqData;
      then bdae;

      case BackendDAE.HESSIAN(varData = varData, eqData = eqData)
        algorithm
          (varData, eqData) := func(varData, eqData);
          bdae.varData := varData;
          bdae.eqData := eqData;
      then bdae;

      else algorithm
        Error.addMessage(Error.INTERNAL_ERROR,{getInstanceName() + " failed!"});
      then fail();
    end match;
  end main;

  function getModule
    "Returns the module function that was chosen by the user."
    output Module.functionAliasInterface func;
  protected
    String flag = "default";
  algorithm
    func := match flag
      case "default" then functionAliasDefault;
      /* ... New function alias modules have to be added here */
      else fail();
    end match;
  end getModule;

protected
  uniontype Call_Id
    "key for UnorderedMap.
    used to uniquely identify a function call"
    record CALL_ID
      Expression call;
      Iterator iter;
    end CALL_ID;

    function toString
      input Call_Id id;
      output String str;
    algorithm
      str := if not Iterator.isEmpty(id.iter) then " [" + Iterator.toString(id.iter) + "]" else "";
      str := Expression.toString(id.call) + str;
    end toString;

    function hash
      "just hashes the id based on its string representation"
      input Call_Id id;
      output Integer hash;
    algorithm
      hash := stringHashDjb2(toString(id));
    end hash;

    function isEqual
      input Call_Id id1;
      input Call_Id id2;
      output Boolean b;
    algorithm
      b := Expression.isEqual(id1.call, id2.call) and Iterator.isEqual(id1.iter, id2.iter);
    end isEqual;
  end Call_Id;

  uniontype Call_Aux
    "value for UnorderedMap.
    represents the auxilliary variable that will be created and has
    the equation kind for auxilliary equation."
    record CALL_AUX
      ComponentRef name;
      EquationKind kind;
      Boolean parsed;
    end CALL_AUX;

    function toString
      input Call_Aux aux;
      output String str = ComponentRef.toString(aux.name);
    end toString;
  end Call_Aux;

  function functionAliasTplString
    input tuple<String, String> tpl;
    input Integer max_length;
    output String str;
  algorithm
    str := Util.tuple21(tpl) + " " + StringUtil.repeat(".", max_length - stringLength(Util.tuple21(tpl))) + " " + Util.tuple22(tpl);
  end functionAliasTplString;

  function functionAliasDefault
    extends Module.functionAliasInterface;
  protected
    UnorderedMap<Call_Id, Call_Aux> map = UnorderedMap.new<Call_Aux>(Call_Id.hash, Call_Id.isEqual);
    Pointer<Integer> index = Pointer.create(1);
    list<Pointer<Variable>> new_vars_disc = {}, new_vars_cont = {}, new_vars_init = {};
    list<Pointer<Equation>> new_eqns_disc = {}, new_eqns_cont = {}, new_eqns_init = {};
    list<tuple<Call_Id, Call_Aux>> debug_lst_sim, debug_lst_ini;
    list<tuple<String, String>> debug_str;
    Integer debug_max_length;
  algorithm
    _ := match eqData
      local
        Call_Id id;
        Call_Aux aux;
        Boolean disc;
        Pointer<Equation> new_eqn;
        Pointer<Variable> new_var;

      case EqData.EQ_DATA_SIM() algorithm
        // first collect all new functions from simulation equations
        eqData.simulation := EquationPointers.map(eqData.simulation, function introduceFunctionAliasEquation(map = map, index = index, init = false));

        // create new simulation variables and corresponding equations for the function alias
        for tpl in listReverse(UnorderedMap.toList(map)) loop
          (id, aux)       := tpl;
          new_var         := BVariable.getVarPointer(aux.name);
          new_eqn         := Equation.makeAssignment(aux.name, id.call, eqData.uniqueIndex, NBVariable.AUXILIARY_STR, id.iter, EquationAttributes.default(aux.kind, false));
          if BVariable.isContinuous(new_var) then
            new_vars_cont := new_var :: new_vars_cont;
            new_eqns_cont := new_eqn :: new_eqns_cont;
          else
            new_vars_disc := new_var :: new_vars_disc;
            new_eqns_disc := new_eqn :: new_eqns_disc;
          end if;
          aux.parsed      := true;
          UnorderedMap.add(id, aux, map);
        end for;

        if Flags.isSet(Flags.DUMP_CSE) then
          debug_lst_sim := UnorderedMap.toList(map);
        end if;

        // afterwards collect all functions from initial equations
        eqData.initials := EquationPointers.map(eqData.initials, function introduceFunctionAliasEquation(map = map, index = index, init = true));

        // create new initialization variables and corresponding equations for the function alias
        for tpl in listReverse(UnorderedMap.toList(map)) loop
          // only create new var and eqn if there is not already one in the simulation system
          if not aux.parsed then
            (id, aux)       := tpl;
            // unfix parameters in initial system because we also add an equation
            new_var         := BVariable.setFixed(BVariable.getVarPointer(aux.name), false);
            new_eqn         := Equation.makeAssignment(aux.name, id.call, eqData.uniqueIndex, NBVariable.AUXILIARY_STR, id.iter, EquationAttributes.default(aux.kind, false));
            new_vars_init := new_var :: new_vars_init;
            new_eqns_init := new_eqn :: new_eqns_init;
          end if;
        end for;
      then ();
      else ();
    end match;
    // add the new variables and equations
    varData := VarData.addTypedList(varData, new_vars_cont, VarData.VarType.ALGEBRAIC);
    varData := VarData.addTypedList(varData, new_vars_disc, VarData.VarType.DISCRETE);
    varData := VarData.addTypedList(varData, new_vars_init, VarData.VarType.PARAMETER);
    eqData  := EqData.addTypedList(eqData, new_eqns_cont, EqData.EqType.CONTINUOUS, false);
    eqData  := EqData.addTypedList(eqData, new_eqns_disc, EqData.EqType.DISCRETE, false);
    eqData  := EqData.addTypedList(eqData, new_eqns_init, EqData.EqType.INITIAL, false);

    // dump if flag is set
    if Flags.isSet(Flags.DUMP_CSE) then
      // remove sim vars from final map to see whats exclusively initial
      for tpl in debug_lst_sim loop
        UnorderedMap.remove(Util.tuple21(tpl), map);
      end for;
      debug_lst_ini := UnorderedMap.toList(map);
      print(StringUtil.headline_3("Simulation Function Alias"));
      debug_str := list((Call_Id.toString(Util.tuple21(tpl)), Call_Aux.toString(Util.tuple22(tpl))) for tpl in debug_lst_sim);
      debug_max_length := max(stringLength(Util.tuple21(tpl)) for tpl in debug_str) + 3;
      print(List.toString(debug_str, function functionAliasTplString(max_length = debug_max_length), "", "  ", "\n  ", "\n\n"));
      print(StringUtil.headline_3("Initial Function Alias"));
      debug_str := list((Call_Id.toString(Util.tuple21(tpl)), Call_Aux.toString(Util.tuple22(tpl))) for tpl in debug_lst_ini);
      debug_max_length := max(stringLength(Util.tuple21(tpl)) for tpl in debug_str) + 3;
      print(List.toString(debug_str, function functionAliasTplString(max_length = debug_max_length), "", "  ", "\n  ", "\n\n"));
    end if;
  end functionAliasDefault;

  function introduceFunctionAliasEquation
    "creates auxilliary variables for all not inlineable function calls in the equation"
    input output Equation eq;
    input UnorderedMap<Call_Id, Call_Aux> map;
    input Pointer<Integer> index;
    input Boolean init;
  protected
    Iterator iter;
  algorithm
    iter := match eq
      case Equation.FOR_EQUATION() then eq.iter;
      else Iterator.EMPTY();
    end match;
    eq := Equation.map(eq, function introduceFunctionAlias(map = map, index = index, iter = iter, init = init));
  end introduceFunctionAliasEquation;

  function introduceFunctionAlias
    "checks if an expression is a function call and replaces it with auxilliary if not inlinable
    map with Equation.map() or Expression.map()
    ToDo: also exclude special functions der(), pre(), ..."
    input output Expression exp;
    input UnorderedMap<Call_Id, Call_Aux> map;
    input Pointer<Integer> index;
    input Iterator iter;
    input Boolean init;
  algorithm
    exp := match exp
      local
        list<ComponentRef> names;
        list<Expression> ranges;
        Iterator new_iter;
        Call_Id id;
        ComponentRef name;
        Type ty;
        Call_Aux aux;
        Option<Call_Aux> aux_opt;
        Expression new_exp;

      case Expression.CALL() guard(checkCallReplacement(exp.call)) algorithm
        // strip nested iterator for the iterators that actually occure in the function call
        if not Iterator.isEmpty(iter) then
          (names, ranges) := Iterator.getFrames(iter);
          (names, ranges) := filterFrames(exp, names, ranges);
          new_iter := Iterator.fromFrames(List.zip(names, ranges));
        else
          new_iter := iter;
        end if;
        // check if call id already exists in the map
        id                  := CALL_ID(exp, new_iter);
        aux_opt             := UnorderedMap.get(id, map);
        if isSome(aux_opt) then
          aux := Util.getOption(aux_opt);
        else
          // for initial systems create parameters, otherwise use type to determine variable kind
          ty := Expression.typeOf(exp);
          if not Iterator.isEmpty(new_iter) then
            ty := Type.liftArrayRightList(ty, list(Dimension.fromInteger(i) for i in Iterator.sizes(new_iter)));
            (_, name) := BVariable.makeAuxVar(NBVariable.FUNCTION_STR, Pointer.access(index), ty, init);
            // add iterators to subscripts of auxilliary variable
            name      := ComponentRef.mergeSubscripts(Iterator.normalizedSubscripts(new_iter), name, true, true);
          else
            (_, name) := BVariable.makeAuxVar(NBVariable.FUNCTION_STR, Pointer.access(index), ty, init);
          end if;
          aux := CALL_AUX(name, if Type.isDiscrete(ty) then EquationKind.DISCRETE else EquationKind.CONTINUOUS, false);
          UnorderedMap.add(id, aux, map);
          Pointer.update(index, Pointer.access(index) + 1);
        end if;
        new_exp := Expression.fromCref(aux.name);
      then new_exp;

      // do nothing if not function call or inlineable
      else exp;
    end match;
  end introduceFunctionAlias;

  function checkCallReplacement
    "returns true if the call should be replaced"
    input Call call;
    output Boolean b;
  protected
    Function fn = Call.typedFunction(call);
  algorithm
    b := not Inline.functionInlineable(fn)
     and not Function.isSpecialBuiltin(fn)
     and not inlineException(fn);
  end checkCallReplacement;

  function inlineException
    input Function fn;
    output Boolean b;
  protected
    Absyn.Path path;
  algorithm
    if Function.isDefaultRecordConstructor(fn) or Function.isNonDefaultRecordConstructor(fn) then
      b := true;
      return;
    end if;
    if not Function.isBuiltin(fn) then
      b := false;
    else
      path := Function.nameConsiderBuiltin(fn);
      if not AbsynUtil.pathIsIdent(path) then
        b := false;
      else
        b := match AbsynUtil.pathFirstIdent(path)
          case "integer" then true;
          else false;
        end match;
      end if;
    end if;
  end inlineException;

  function filterFrames
    "filters the list of frames for all iterators that occure in exp"
    input Expression exp;
    input output list<ComponentRef> names;
    input output list<Expression> ranges;
  protected
    UnorderedMap<ComponentRef, Expression> frame_map = UnorderedMap.fromLists<Expression>(names, ranges, ComponentRef.hash, ComponentRef.isEqual);
    Pointer<list<ComponentRef>> names_acc = Pointer.create({});
    Pointer<list<Expression>> ranges_acc = Pointer.create({});
    function collectFrames
      input output Expression exp;
      input UnorderedMap<ComponentRef, Expression> frame_map;
      input Pointer<list<ComponentRef>> names_acc;
      input Pointer<list<Expression>> ranges_acc;
    algorithm
      _ := match exp
        local
          Option<Expression> range;
        case Expression.CREF() algorithm
          range := UnorderedMap.get(exp.cref, frame_map);
          if isSome(range) then
            Pointer.update(names_acc, exp.cref  :: Pointer.access(names_acc));
            Pointer.update(ranges_acc, Util.getOption(range) :: Pointer.access(ranges_acc));
          end if;
        then ();
        else ();
      end match;
    end collectFrames;
  algorithm
    _ := Expression.map(exp, function collectFrames(frame_map = frame_map, names_acc = names_acc, ranges_acc = ranges_acc));
  names := Pointer.access(names_acc);
  ranges := Pointer.access(ranges_acc);
  end filterFrames;

  annotation(__OpenModelica_Interface="backend");
end NBFunctionAlias;