# Marisa CPU parity: five fresh seeds, 2026-11-20

Each seed is compared through 36,000 cumulative frames across rounds. Gameplay validation requires identical primary RNG, CPU inputs, player positions, and the complete kind/color/coordinate multiset of enemy bullets with coordinate tolerance zero. Animation-only differences are excluded.

## Shared mechanism changes

- A straight-laser child created by a circle cut joins the native manager after BombInf callbacks. Registered-only rectangular clears must skip children that have not received their first manager visit. The original 9,334-frame divergence in seed 776739383 was a second cut of an unregistered child, removing a graze reward.
- A late player hit resets existing item magnets before the next item movement. Nearby items reacquire at one third of pull speed. The original 30,784-frame divergence in seed 435245857 followed a charge pickup collected one frame early, taking energy from 2,485 to 2,500 and changing CPU charge input.

The original run and the fixed-build run are recorded separately. Screenshots show Web on the left and the original on the right. The source changes, validation summaries, and paired screenshots accompany PR #214. Raw native captures remain in the local evidence store.

## Completed verification

All five fresh seeds pass on build cebd32579d54d46f0e4e14d955d6429ec5d7a4bd408433f7de22524cf2d6b1e0.

| Primary seed | Secondary seed | Cumulative frames | Result |
| --- | --- | ---: | --- |
| 435245857 | 1672009235 | 36000 | PASS |
| 776739383 | 2089740602 | 36000 | PASS |
| 178592377 | 875265876 | 36000 | PASS |
| 1571043821 | 2123938083 | 36000 | PASS |
| 377606074 | 1674020854 | 36000 | PASS |

Total: 180000 cumulative frames; 179998 compared battle states; 2 independently verified round-transition gaps. Gameplay comparisons report zero differences. Screenshots include the original first-difference frames 9334 and 30784 and matching repaired frames.

Documentation checks are recorded alongside the published verification bundle. Historical 55-seed results remain recorded against their previous build and are not relabeled as validation of this new build.

## Screenshot pairs

| First difference | Before repair | After repair |
| --- | --- | --- |
| Seed 776739383, frame 9334 | [Web left / native right](screenshots/before-776739383/frame-9334.png) | [Web left / native right](screenshots/after-776739383/frame-9334.png) |
| Seed 435245857, frame 30784 | [Web left / native right](screenshots/before-435245857/frame-30784.png) | [Web left / native right](screenshots/after-435245857/frame-30784.png) |

[Earlier 55-seed regression](../th19-marisa-all-seeds-regression-20261120/README.md) records the preceding build separately. Each historical seed also covers 36000 cumulative frames.
