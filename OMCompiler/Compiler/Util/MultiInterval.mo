encapsulated package MultiInterval
  import Interval;

protected
  import Array;
  import MetaModelica.Dangerous.listReverseInPlace;

public
  type MultiInterval = array<Interval>;

  function create
    input list<Interval> int;
    output MultiInterval outInt;
  algorithm
    outInt := listArray(int);
  end create;

  function crop
    input MultiInterval int;
    output MultiInterval outInt;
  algorithm
    outInt := Array.mapNoCopy(int, Interval.crop);
  end crop;

  function createUnit
    input Integer dimensions;
    output MultiInterval outInt;
  algorithm
    outInt := arrayCreate(dimensions, Interval.createUnit());
  end createUnit;

  function createFull
    input Integer dimensions;
    output MultiInterval outInt;
  algorithm
    outInt := arrayCreate(dimensions, Interval.createFull());
  end createFull;

  function isEmpty
    input MultiInterval int;
    output Boolean empty;
  algorithm
    empty := arrayEmpty(int);
  end isEmpty;

  function length
    input MultiInterval int;
    output Integer len;
  algorithm
    len := arrayLength(int);
  end length;

  function nthInterval
    input MultiInterval ints;
    input Integer index;
    output Interval outInt;
  algorithm
    outInt := arrayGet(ints, index);
  end nthInterval;

  function crossProduct
    input MultiInterval int1;
    input MultiInterval int2;
    output MultiInterval outInt;
  algorithm
    outInt := Array.join(int1, int2);
  end crossProduct;

  function replace
    "Destructively replaces the interval at the given index with the given interval."
    input MultiInterval int1;
    input Interval int2;
    input Integer index;
    output MultiInterval outInt;
  algorithm
    outInt := arrayUpdate(int1, index, int2);
  end replace;

  function intersect
    "Returns the intersection of int1 and int2 as a new multiinterval."
    input MultiInterval int1;
    input MultiInterval int2;
    output MultiInterval outInt;
  protected
    Interval int;
    list<Interval> ints = {};
  algorithm
    if length(int1) == length(int2) then
      for i in arrayLength(int1):-1:1 loop
        int := Interval.intersect(int1[i], int2[i]);

        if Interval.isEmpty(int) then
          // If any dimensions has an empty intersection, the whole intersection is empty.
          ints := {};
          break;
        else
          ints := int :: ints;
        end if;
      end for;
    end if;

    outInt := create(ints);
  end intersect;

  function remove
    "Returns a list of multiintervals corresponding to the removal of int2 from int1."
    input MultiInterval int1;
    input MultiInterval int2;
    output list<MultiInterval> outInt = {};
  protected
    Integer N, nmints;
    MultiInterval int2a, auxint;
    list<Interval> ints;
  algorithm
    N := length(int1);
    int2a := intersect(int1, int2); // Only the intersection is removed.

    if isEmpty(int2a) then
      outInt := {arrayCopy(int1)};
    else
      nmints := 0;
      for i in 1:N loop
        // Computes the intervals that remain in the ith dimension.
        ints := Interval.remove(nthInterval(int1, i), nthInterval(int2a, i));

        for int in ints loop
          auxint := createUnit(N);

          // Create a multidimensional interval for each interval.
          for k in 1:N loop
            if k < i then
              // If the dimension to add is previous to the removed dimension, add the removed interval.
              auxint := replace(auxint, nthInterval(int2a, k), k);
            elseif k == i then
              // If the dimension to add is the removed dimension, add the remaining interval.
              auxint := replace(auxint, int, k);
            else
              // If the dimension to add is greater than the removed dimension, keep the original interval.
              auxint := replace(auxint, nthInterval(int1, k), k);
            end if;
          end for;
        end for;

        outInt := auxint :: outInt;
      end for;

      outInt := listReverseInPlace(outInt);
    end if;
  end remove;

  function contains
    "Returns true if the given values belong to the corresponding intervals in
     the given multiinterval."
    input list<Integer> vals;
    input MultiInterval int;
    output Boolean res = true;
  protected
    Integer index = 1;
    Interval i;
  algorithm
    for v in vals loop
      if not Interval.contains(v, nthInterval(int, index)) then
        res := false;
        break;
      end if;
      index := index + 1;
    end for;
  end contains;

  function toString
    input MultiInterval ints;
    output String str;
  protected
    list<String> strl;
  algorithm
    if isEmpty(ints) then
      str := "emptyInterval";
    else
      str := stringDelimitList(list(Interval.toString(i) for i in ints), "x");
    end if;
  end toString;

annotation(__OpenModelica_Interface="util");
end MultiInterval;
