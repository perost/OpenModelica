// name:     FunctionMultiOutput4
// keywords: 
// status:   correct
//

function f
  input Real x;
  output Real y;
  output Real z;
algorithm
  y := x * 2;
  z := x * 3;
end f;

model FunctionMultiOutput4
  Real x;
equation
  x = f(time) + 1;
end FunctionMultiOutput4;

// Result:
// function f
//   input Real x;
//   output Real y;
//   output Real z;
// algorithm
//   y := x * 2.0;
//   z := x * 3.0;
// end f;
//
// class FunctionMultiOutput4
//   Real x;
// equation
//   x = f(time)[1] + 1.0;
// end FunctionMultiOutput4;
// endResult
