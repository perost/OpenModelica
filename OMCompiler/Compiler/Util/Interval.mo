encapsulated uniontype Interval

protected
  import System;
  import Util;
  import MetaModelica.Dangerous.listReverseInPlace;

  function euclid
    "uses the extended euclidean algorithm to compute
     - greatest common divisor d = gcd(a,b)
     - least common multiple m = lcm(a,b) = a*(b/d)
     - Bézout coefficients ua + vb = u*a + v*b = d
    "
    input Integer a;
    input Integer b;
    output Integer d "gcd";
    output Integer m "lcm";
    output Integer ua;
    output Integer vb;
  protected
    Integer q;
    Integer r1 = a, r2 = b;
    Integer s1 = a, s2 = 0;
    Integer tmp;
  algorithm
    while r2 <> 0 loop
      q := div(r1, r2);

      tmp := r2;
      r2 := r1 - q * r2;
      r1 := tmp;

      tmp := s2;
      s2 := s1 - q * s2;
      s1 := tmp;
    end while;
    d := r1;
    m := abs(s2);
    ua := s1;
    vb := r1 - s1;
  end euclid;

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
    Integer new_start, new_step, new_stop;
    Integer gcd_, ua, vb, x;
  algorithm
    if int1.stop < int2.start or int2.stop < int1.start then
      // The intervals do not intersect.
      int := INTERVAL(0, 0, 0);
    else
      // The new step will be the least common multiple of the two intervals' steps.
      (gcd_, new_step, ua, vb) := euclid(int1.step, int2.step);

      if 0 <> mod(int1.start - int2.start, gcd_) then
        // The intervals step through each other without touching
        int := INTERVAL(0, 0, 0);
      else
        // x is an integer on both intervals (modulo new_step)
        x := div(int1.start, gcd_) * vb + div(int2.start, gcd_) * ua + mod(int1.start, gcd_);

        // Find new start and stop, crop with x
        new_start := max(int1.start, int2.start);
        new_stop := min(int1.stop, int2.stop);
        new_start := new_start + mod(x - new_start, new_step);
        new_stop := new_stop - mod(new_stop - x, new_step);

        if new_stop < new_start then
          // Empty interval
          int := INTERVAL(0, 0, 0);
        else
          int := INTERVAL(new_start, new_step, new_stop);
        end if;
      end if;
    end if;
  end intersect;

  function remove
    "Returns a list of intervals corresponding to the removal of int2 from int1."
    input Interval int1;
    input Interval int2;
    output list<Interval> ints = {};
  protected
    Interval i1, i2;
    Integer count_r, count_s;
  algorithm
    i1 := crop(int1); // is this necessary?
    i2 := intersect(i1, int2);

    if isEmpty(i2) then
      // No intersection, nothing to remove.
      ints := {i1};
    else
      // Rightmost interval.
      if i2.stop < i1.stop then
        ints := INTERVAL(i2.stop + i1.step, i1.step, i1.stop) :: ints;
      end if;

      count_r := div(i2.step, i1.step) - 1;
      count_s := div(i2.stop - i2.start, i2.step);

      if count_r < count_s then
        // create an interval for every residue class not equal to i2.start
        for i in count_r:-1:1 loop
          ints := INTERVAL(i2.start + i * i1.step, i2.step, i2.stop - i2.step + i * i1.step) :: ints;
        end for;
      else
        // create an interval for every space between removed points
        for i in count_s:-1:1 loop
          ints := INTERVAL(i2.start + i1.step + (i - 1) * i2.step, i1.step, i2.start - i1.step + i * i2.step) :: ints;
        end for;
      end if;

      // Leftmost interval.
      if i2.start > i1.start then
        ints := INTERVAL(i1.start, i1.step, i2.start - i1.step) :: ints;
      end if;
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
