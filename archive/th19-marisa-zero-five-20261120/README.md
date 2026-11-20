# Five fresh seeds: exact gameplay validation

All five new Marisa CPU-vs-CPU seeds passed 36000 cumulative frames each across rounds. Total:180000 frames. Bullet coordinate tolerance and maximum coordinate difference are both zero. Primary RNG,CPU inputs,player positions and bullet counts match. Visual-only animation remains outside the requested gameplay scope.

WASM SHA-256:827f2af70f0962c30ab6e072cf29be8c490e4931de2598e2e94adf38221ca6b5.

[Aggregate](complete.json), [per-seed results](final-results.json), [capture identities](final-status.json), [raw zero-tolerance validations](validation/).

The initial run had two exact passes,two substantive differences,and one microscopic coordinate difference. All three failed cases were corrected and the entire five-seed batch rerun on the same final build.

- Rectangular laser cuts retain a retiring short head controller. A surviving separated tail then registers a fresh controller instead of inheriting the original Aura slowdown and graze clock.
- VS collision clears the charge gauge in the charge-manager phase after CPU sampling. The CPU must observe the pre-contact gauge on that visit.
- ECL569 normalizes the float angle to the native range before trigonometry. Equivalent angles outside that range can otherwise produce different float edge coordinates.

Before/after images use Web on the left,native on the right,with cumulative frame and mismatch count:
- 665982791:frame23119 in screenshots/seed665982791-before and screenshots/seed665982791-after.
- 1504457712:frame10046 in screenshots/seed1504457712-before and screenshots/seed1504457712-after.
- 1869345750:frame10641 in screenshots/seed1869345750-before and screenshots/seed1869345750-after.

The earlier35-seed results were produced on an older build and are not counted as final-build coverage here.
