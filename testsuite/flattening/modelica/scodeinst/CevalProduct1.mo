// name: CevalCross1
// keywords:
// status: correct
//
//

model CevalProduct1
  constant Real r1 = product({{{1.0, 2.0}, {3.0, 4.0}}, {{5.0, 6.0}, {7.0, 8.0}}});
  constant Real r2 = product(fill(3.0, 2, 2));
  constant Real r3 = product({{3.0, 3.0}, {3.0, 3.0}});
  constant Real r4 = product(fill({1.0, 2.0, 3.0}, 2, 3));
  constant Real r5 = product({{{1.0, 2.0, 3.0}, {1.0, 2.0, 3.0}, {1.0, 2.0, 3.0}}, {{1.0, 2.0, 3.0}, {1.0, 2.0, 3.0}, {1.0, 2.0, 3.0}}});
  constant Integer i1 = product({1, 2, 3, 4, 5, 6, 7, 8, 9});
  constant Integer i2 = product(fill(3, 2, 2));
  constant Integer i3 = product({{3, 3}, {3, 3}});
  constant Integer i4 = product(fill({1, 2, 3}, 2, 3));
  constant Integer i5 = product({{{1, 2, 3}, {1, 2, 3}, {1, 2, 3}}, {{1, 2, 3}, {1, 2, 3}, {1, 2, 3}}});
end CevalProduct1;

// Result:
// class CevalProduct1
//   constant Real r1 = 40320.0;
//   constant Real r2 = 81.0;
//   constant Real r3 = 81.0;
//   constant Real r4 = 46656.0;
//   constant Real r5 = 46656.0;
//   constant Integer i1 = 362880;
//   constant Integer i2 = 81;
//   constant Integer i3 = 81;
//   constant Integer i4 = 46656;
//   constant Integer i5 = 46656;
// end CevalProduct1;
// endResult
