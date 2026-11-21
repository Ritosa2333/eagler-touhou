# Marisa CPU parity: batch 6, five fresh seeds

Five fresh seeds and the preceding five regression seeds each pass 36000 cumulative frames across rounds on the same build. Primary RNG, CPU controls, player positions, and every enemy bullet kind/color/coordinate match exactly; coordinate tolerance is zero. Animation-only differences are recorded independently.

Build: 0fdafbb2211bb7311413e8bee554f71251c650c92330115c357a51efe38108f0

| Primary seed | Secondary seed | Frames | Result |
| --- | --- | ---: | --- |
| 1065733957 | 267368346 | 36000 | PASS |
| 1766797787 | 2131122726 | 36000 | PASS |
| 1833594954 | 1478809317 | 36000 | PASS |
| 1233334686 | 381609119 | 36000 | PASS |
| 1729599178 | 1148959835 | 36000 | PASS |

The shared damage path includes the first active respawn tick when scheduling deferred contacts. Bomb rectangle splits admit new laser tails to the same-tick graze pass; collision-specific player cuts retain their next-visit registration. Native captures at the first divergent frames establish both callback orders.

Before/after comparisons in screenshots/ show Web on the left and native on the right, with cumulative frame, RNG, CPU input and position annotations. fresh-five/ and previous-five/ preserve build identities, native trace hashes and exact gameplay validations.
