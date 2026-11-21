# Marisa CPU parity: batch 9, five fresh seeds

Five previously untested seeds each pass 36000 cumulative frames across rounds on the unchanged batch-8 build. No gameplay repair was required for this batch. The initial first-seed capture produced no battle frames and was retried; it is an acquisition failure, not a gameplay mismatch.

Build: d38eda254e4188948c359e123ff97b6d4ec4e9991b85a07dee7b2f533ba2f7ca

| Primary seed | Secondary seed | Frames | Result |
| --- | --- | ---: | --- |
| 1111888692 | 1942808069 | 36000 | PASS |
| 778927144 | 1832312959 | 36000 | PASS |
| 1559918373 | 876156128 | 36000 | PASS |
| 1301224571 | 1236112832 | 36000 | PASS |
| 429629600 | 391328776 | 36000 | PASS |

Acceptance checks primary RNG, CPU controls, player positions and full enemy bullet kind/color/coordinate multisets. Coordinate tolerance is zero. Under the user instruction, animation-only and tiny differences without later gameplay effects may be accepted separately, with strict validations preserved. Gameplay first-pass status is distinct from capture startup retries. The preceding batch validated ten seeds on this identical build; this batch adds five, totaling 540000 cumulative frames across fifteen seeds.
