// name: ceval2.mo
// status: correct

model A
  parameter Integer n = 1;
  parameter Integer m = 2+n;
  Real x[m] = fill(1.0, m);
end A;

// Result:
// class A
//   final parameter Integer n = 1;
//   final parameter Integer m = 3;
//   Real x[1];
//   Real x[2];
//   Real x[3];
// equation
//   x = fill(1.0, 3);
// end A;
// endResult
