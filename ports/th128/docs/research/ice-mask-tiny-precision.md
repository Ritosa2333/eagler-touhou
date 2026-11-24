# Tiny-coordinate ice rasterization

Actual keyboard freezing at first-stage frame8185 produced a finite near-zero X. The former mask coordinate guard rejected it before merging the large bullet's real bitmap. Removing the guard alone is insufficient: native helpers4106e0/410820 keep intermediate additions in x87 precision64, and48fac0 then rounds to double before integer truncation.

The portable placement calculation retains tiny input deltas on the original extended-precision lattice of384,192, or the negative half-height. It applies ties-to-even at that lattice and only then performs the final double addition. An exact power-of-two approached from below uses the adjacent lower binade. Ordinary float32 positions retain the existing placement path. Shape, finite-coordinate, byte-span and alias validation still occur before mutation.

The bounded read-only native report `local/research/ice-mask-fixtures.json` contains108 cases: complete bit counters, OR/AND-NOT kernels, real selector attachment/removal prefixes, subnormal coordinates, and neighboring float32 values around four rounding boundaries. Its SHA256 is754e363dff1333172eb99edb612861063554755f2070c70859a60d9690f14438. Exact full masks, changed-bit counts, placement metadata and writes match in WASM and UBSan. No original game process or fabricated API return was used.
