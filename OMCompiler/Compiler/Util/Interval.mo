encapsulated uniontype Interval

protected
  import System;
  import Util;
  import MetaModelica.Dangerous.listReverseInPlace;

public
  record INTERVAL
    Integer start;
    Integer step;
    Integer stop;
  end INTERVAL;

  function create
    input Integer start;
    input Integer step;
    input Integer stop;
    output Interval int;
  algorithm
    int := INTERVAL(start, step, stop);
    int := crop(int);
  end create;

  function createNoCrop
    input Integer start;
    input Integer step;
    input Integer stop;
    output Interval int;
  algorithm
    int := INTERVAL(start, step, stop);
  end createNoCrop;

  function createUnit
    output Interval int = INTERVAL(1, 1, 1);
  end createUnit;

  function createFull
    output Interval int = INTERVAL(1, 1, System.intMaxLit());
  end createFull;

  function crop
    input output Interval int;
  algorithm
    if int.stop < System.intMaxLit() then
      int.stop := int.stop - mod(int.stop - int.start, int.step);
    end if;
  end crop;

  function intersect
    input Interval int1;
    input Interval int2;
    output Interval int;
  protected
    Integer start1, start2;
    Integer new_step, new_stop;
    Integer search_start, search_stop;
  algorithm
    if int1.stop < int2.start or int2.stop < int1.start then
      // The intervals do not intersect.
      int := INTERVAL(0, 0, 0);
    else
      // The new step will be the least common multiplier of the two intervals' steps.
      new_step := Util.lcm(int1.step, int2.step);
      new_stop := min(int1.stop, int2.stop);
      start1 := int1.start;
      start2 := int2.start;

      // Try to find the smallest value common to both intervals to use as the
      // start of the intersection.
      if start1 <> start2 then
        // The new start must be within both old intervals, so step the smaller
        // start value forward until it's within the other interval.
        if start1 < start2 then
          start1 := start1 + int1.step * intDiv((start2 - start1), int1.step);
        else
          start2 := start2 + int2.step * intDiv((start1 - start2), int2.step);
        end if;

        // The start of the intersection must be within one new step and before
        // the end of either interval.
        search_stop := min(new_stop, min(start1, start2) + new_step - 1);

        // Step the start values forward one step at a time until they're equal
        // or we reach the end of the range of possible start values.
        while start1 < search_stop and start2 < search_stop loop
          if start1 == start2 then
            break;
          elseif start1 < start2 then
            start1 := start1 + int1.step;
          else
            start2 := start2 + int2.step;
          end if;
        end while;

        // Couldn't find a new start value, the intervals do not intersect.
        if start1 <> start2 then
          // Empty interval
          int := INTERVAL(0, 0, 0);
        end if;
      end if;

      int := create(start1, new_step, min(int1.stop, int2.stop));
    end if;
  end intersect;

  function remove
    "Returns a list of intervals corresponding to the removal of int2 from int1."
    input Interval int1;
    input Interval int2;
    output list<Interval> ints = {};
  protected
    Interval i1, i2, itmp;
    Integer count;
  algorithm
    i1 := crop(int1);
    i2 := intersect(i1, int2);

    if isEmpty(i2) then
      // No intersection, nothing to remove.
      ints := {i1};
    else
      count := 0;

      // Leftmost interval.
      if i2.start > i1.start then
        itmp := create(i1.start, 1, i2.start - 1);
        ints := intersect(i1, itmp) :: ints;
      end if;

      if i2.stop - i2.start > i2.step then
        count := intDiv(i2.step, i1.step) - 1;
      else
        count := 0;
      end if;

      for i in 1:count loop
        ints := create(i2.start + i * i1.step, i2.step, i2.stop) :: ints;
      end for;

      // Rightmost interval.
      if i2.stop < i1.stop then
        itmp := create(i2.stop + 1, 1, i1.stop);
        ints := intersect(i1, itmp) :: ints;
      end if;

      ints := listReverseInPlace(ints);
    end if;
  end remove;

  function affine
    "Affine function for scaling and offsetting an interval."
    input Interval int;
    input Real gain;
    input Integer offset;
    output Interval res;
  protected
    Real start, step, stop;
    Integer istart, istep, istop;
  algorithm
    INTERVAL(start, step, stop) := int;

    if gain > 0 then
      start := start * gain + offset;
      stop := stop * gain + offset;
      step := step * gain;

      if step < 1 then
        step := 1.0;
        start := ceil(start);
        stop := floor(stop);
      end if;

      if start < 0 then
        start := start + step * (1 + floor(abs(start) / step));
      end if;

      istart := integer(start);
      istep := integer(step);
      istop := integer(stop);

      if istart == istop then
        istep := 1;
      end if;

      if stop < start then
        // Empty interval.
        res := INTERVAL(0, 0, 0);
      else
        res := INTERVAL(istart, istep, istop);
      end if;
    else
      if offset > 0 then
        res := INTERVAL(offset, 1, offset);
      else
        // Empty interval.
        res := INTERVAL(0, 0, 0);
      end if;
    end if;
  end affine;

  function contains
    "Returns true if c belongs to the interval, otherwise false."
    input Integer c;
    input Interval int;
    output Boolean res;
  algorithm
    res := not isEmpty(int) and
           c >= int.start and
           c <= int.stop and
           mod(c - int.start, int.step) == 0;
  end contains;

  function isEmpty
    // TODO: Decide on best representation of an empty interval.
    input Interval int;
    output Boolean res = int.step == 0;
  end isEmpty;

  function toString
    input Interval interval;
    output String str;
  algorithm
    str := "[" + String(interval.start) + ":" +
                 String(interval.step) + ":" +
                 String(interval.stop) + "]";
  end toString;

annotation(__OpenModelica_Interface="util");
end Interval;
