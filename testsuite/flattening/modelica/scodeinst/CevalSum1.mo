// name: CevalSum1
// keywords:
// status: correct
//
//

model CevalSum1
  constant Real r1 = sum({{{1.0, 2.0}, {3.0, 4.0}}, {{5.0, 6.0}, {7.0, 8.0}}});
  constant Real r2 = sum(fill(3.0, 2, 2));
  constant Real r3 = sum({{3.0, 3.0}, {3.0, 3.0}});
  constant Real r4 = sum(fill({1.0, 2.0, 3.0}, 2, 3));
  constant Real r5 = sum({{{1.0, 2.0, 3.0}, {1.0, 2.0, 3.0}, {1.0, 2.0, 3.0}}, {{1.0, 2.0, 3.0}, {1.0, 2.0, 3.0}, {1.0, 2.0, 3.0}}});
  constant Integer i1 = sum({1, 2, 3, 4, 5, 6, 7, 8, 9});
  constant Integer i2 = sum(fill(3, 2, 2));
  constant Integer i3 = sum({{3, 3}, {3, 3}});
  constant Integer i4 = sum(fill({1, 2, 3}, 2, 3));
  constant Integer i5 = sum({{{1, 2, 3}, {1, 2, 3}, {1, 2, 3}}, {{1, 2, 3}, {1, 2, 3}, {1, 2, 3}}});
end CevalSum1;

// Result:
// class CevalSum1
//   constant Real r1 = 36.0;
//   constant Real r2 = 12.0;
//   constant Real r3 = 12.0;
//   constant Real r4 = 36.0;
//   constant Real r5 = 36.0;
//   constant Integer i1 = 45;
//   constant Integer i2 = 12;
//   constant Integer i3 = 12;
//   constant Integer i4 = 36;
//   constant Integer i5 = 36;
// end CevalSum1;
// endResult
