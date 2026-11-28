# Original ice bucket conversion

The original410660 helper keeps the X+192 sum and division by-48 in x87 extended precision. Its48fac0 SSE conversion then stores the quotient as double before truncating. Rounding X+192 to double first loses meaningful negative values around zero; broadly rejecting such coordinates or points close to a grid boundary crashes legitimate freeze propagation.

For tiny X, the192 sum has an extended-precision quantum of2^-56. The portable implementation rounds the delta to that lattice, ties to even, then forms the quotient around-4 without losing the delta. This preserves the integer bucket result while retaining the bullet's original float coordinates. Other float32 coordinates and the float32-derived Y denominator are safely resolved by double arithmetic in the bounded playfield domain.

The expanded52-case original-byte fixture includes negative/positive tiny values, subnormals, column/row boundaries, and the three float32 neighbors of the extended halfway point-1537*2^-57. Valid native buckets are accepted; actual out-of-table indices are still rejected. This is not coordinate snapping or replacing nonzero values with zero.
