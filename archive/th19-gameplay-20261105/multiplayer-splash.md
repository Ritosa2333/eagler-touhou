# Multiplayer recording and splash damage

Original EXE: SHA256 `8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9`.

Single-player launch, sync, battle return and exit no longer start or save an input recording. Existing files remain readable. Network players and observers retain confirmed-input recording, gated on actual gameplay. Browser checks cover single-player battle/exit/reload without a replay, observer late join, pause, rewind, live catch-up, file replay and matching simulation hashes.

## Original damage evidence

`0x54c5f0` (hit3) and `0x54a7b0` (hit9) create a damage circle lasting16 frames, interval2, radius growth1 and movement0.3. SHT pl06 groups2/7 use hit3; pl16 groups2/7 use hit9. `0x520450` sums overlapping regions and `0x520864..0x520875` caps target damage using Player+0x2274. `0x516c78/0x516c91` load that value from SHT+0x28. Retail caps are90 except pl11=70 and pl12=120.

The port loads this field and shares a per-target per-frame damage budget after the proximity-card bonus. Browser tests exercise real pl06/pl16 firing and overlapping flame regions. `tests/spirit-splash-damage.cpp` executes original default.ecl to produce ordinary/enhanced wolf, otter and eagle targets; it verifies overlapping damage, next-frame budget renewal and proximity-card capping. Pass the extracted original default.ecl path as its argument; original binary assets are external.

## Difficulty validation

`difficulty-matrix.json` records actual menu entry into all four difficulties in story and local VS. Story keeps its separate native initialization; VS displayed EX/Boss levels are1/1/3/5. Multiplayer room results are recorded separately for both clients, including difficulty, initial ranks and matching state hashes.

Web recordings remain versioned web files with `.rpy` extension; this change does not claim original-file interoperability.
