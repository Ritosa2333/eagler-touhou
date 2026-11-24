# Main-thread review: geometry and Boss movement

Review completed on the user's requested main thread, without a delegated
reviewer. No Git branch/commit/publish operation was performed. This is a local
engineering review and bounded evidence audit, not independent original-game
or native GPU acceptance.

## Issues found and resolved during implementation

- Inherited TH12 anchor/mode bit shifts were not TH128's20/22/24. Failing tests
  against original65/67 handlers exposed the difference; the proper packed flags
  and portable fields now match. Factory412520's blend clearing also changes
  custom526 rows to blend0/mode12, distinct from Main82 blend3/mode0.
- Numeric layer ordering would put layer29 after gameplay/HUD. Original31
  callback registrations now govern world ordering, with target capture12 and
  Main82/rows14. WebGL2 desktop/mobile capture tests prove orientation and
  same-frame reuse for a declared identity fixture, with zero pixel differences.
-420 cannot be implemented as successful no-op or as a whole-world pause. The
  explicit held command preserves itsPC/time while other contexts, movement,
  geometry and ANM proceed; actual412 cancellation releases it.
-404 clamps combined quantized position, preserves its pre-clamp velocity and
  rebuilds absolute coordinates. Tests cover relative components, fractional
  positions, exact edges and inverted extents without undefined std::clamp use.
-410 changes base drop, not score or all drops. Additional drops/spread survive.
-312 consumes actual shared RNG before reading mode/speed/duration and only
  changes current motion on the next local pass. Direction tests cover actual
 2/3 call counts and thresholds. Mode4 tests include angles beyond±pi and exact
  intermediate float stores;305 mode0 retains its prior behavior.
- Rendering/snapshot serialization does not advance simulation,ANM orRNG.
  Read-only snapshots, owner retirement and per-generation asset ownership have
  dedicated ordinary/UBSan and browser regressions.

## Verified current result

Generationd26a8d3d39549eaa0f8347b0841ad79ff32a0d1d432141f030019cb3ec994975 has97
matching source files.162 Node/C++/WASM,343 Python and3 visual-probe tests pass
without failures/skips.16 browser scripts pass at8128. Additional no-exceptions
checks pass3 cases;1 throwing-host test is explicitly inapplicable, not counted
as passed. Original/prepared Japanese/Chinese resource hashes remain unchanged.

ActualNormal/seed34772 reaches3292/MBoss1At2/op524. The boss has begun real312
movement and16 originaltext82 mesh rows remain live. No missing command was
forced successful. Summary and source/artifact/log hashes are recorded in
`../enemy312-verification.json` and `../main-thread-verification.json`.

## Remaining risk and work

No claim of completeBoss/stage/game.524's radial birth consumer, actual type4
profile/ANM/effects and later extension transitions are next.414 selected phase
reset/resolve/invoke, full initialization/control callbacks, Z/freezing/Perfect
Freeze, lasers, backgrounds, routes/endings and replay simulation are unfinished.
Main82's later interrupt/multi-target lifetime is not established. Geometry
normalization has a measured maximum1/32768pixel portable RSQRT difference on
sampled displacedXY; this is documented, not hidden as bit-identical nativeCPU.
