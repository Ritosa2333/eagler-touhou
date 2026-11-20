# Marisa CPU versus CPU: third fresh five-seed batch

Final result: **5/5 PASS**, each 36000 continuous frames across rounds, total 180000. Coordinate tolerance and maximum observed coordinate difference: zero.

Build SHA256: 4e37d7915b5568db998ce32b5d009c9298df4b92715b6535a0c3191237b821fb

| Primary seed | Secondary seed | Frames | Result |
| --- | --- | --- | --- |
| 1893703640 | 328577206 | 36000 | PASS |
| 1354966619 | 39499591 | 36000 | PASS |
| 357622458 | 506629354 | 36000 | PASS |
| 1099740237 | 1764822807 | 36000 | PASS |
| 554623781 | 287782238 | 36000 | PASS |

All available battle states compare primary RNG, CPU input, player-position bits, and enemy-bullet kind/color/coordinates. Pure animation remains excluded by established user scope. Verified round-transition gaps: [{"seed": 1893703640, "frames": [14791]}, {"seed": 1354966619, "frames": [30541]}]. These are independently validated non-battle transition intervals; no gameplay mismatch is waived.

## Retained changes

1. ECL525 birth-tick clear: an animationless actor moves before being cleared, but the native chain damage descriptor still uses its preceding registration position. Preserve that center for damage while keeping the visible center. This prevents an extra six-point chain hit and premature death at frame 13961.
2. VS protection timing: expire invulnerability in the player callback before later BombInf refreshes, rather than in the late visual tick. Preserve death/respawn initialization behavior. Fixes shield and repel-position difference at frame 14325.
3. ECL613 full-field clear: discard deferred laser-graze rewards with removed controllers. Otherwise an early sampled reward is paid after the clear, diverging primary RNG at frame 14454.

No seed/frame-specific fixes, phase/length-based retirement predicates, or temporary diagnostic prints remain. Rejected candidates are retained in local work folders and are not PASS evidence.

## Paired screenshots

Web is on the LEFT and native on the RIGHT, annotated with cumulative/round frame and mismatch count. Before: 13961/13962, 14325/14326, 14454/14455. Final build: 13961, 14325, 14454, with zero accumulated gameplay mismatches.

Final image indexing imports real native BMP frames from same-seed captures. Each imported frame was checked against the base native trace for identical primary RNG, controls, player-position bits, and full enemy-bullet position arrays. See screenshots/final-native-provenance.json.

## Scope

These five passes belong to this final build only. Earlier batches retain their respective hashes and have not all been rerun on this build. Nothing was committed or pushed.
