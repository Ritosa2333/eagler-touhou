# Enoko trap cancellation

Retail executable SHA256: `8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9`.

`pl15.sht` groups 10 and 11 use init 21 (`0x54aca0`) and update 13 (`0x54aae0`). The init preserves the projectile on enemy contact. The update cancels during `[transition + 10, transition + 20)`, using a 24 by 56 rectangle at the projectile position and angle.

`0x4ee0d0` accepts ordinary bullets in states 1/2 with zero cancellation lock. Both the rotated rectangle and the 296 by 448 playfield must intersect the bullet collision radius. It does not filter by descriptor or color.

`0x5303f0` dispatches to laser rectangle cancellation, excluding inactive/phase-1 objects. `0x52a970`, `0x524fc0`, and `0x52d350` respect the cancellation lock. Curves test nodes; straight/infinite lasers test 16-unit samples through `0x4d2c30` and preserve surviving segments. Rectangle cancellation is enabled for all descriptors rather than the older circle cancellation descriptor subset.

Validation: `tests/enoko-cancel.mjs` compiles and executes the current C++ logic; covers timing, ordinary descriptors 0..38 across five states and two lock values, orientation, viewport, straight/infinite laser descriptors and curve locks. Evidence was obtained by static inspection without launching the retail game. This is not a claim of a frame-by-frame retail gameplay comparison.
