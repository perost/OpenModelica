// name: FillEq1
// keywords:
// status: correct
//
//

model FillEq1
  Real x[3];
equation
  zeros(3) = x;
end FillEq1;

// Result:
// class FillEq1
//   Real x[1];
//   Real x[2];
//   Real x[3];
// equation
//   0.0 = x[1];
//   0.0 = x[2];
//   0.0 = x[3];
// end FillEq1;
// endResult
