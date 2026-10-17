# Type2 ETEX13: proven original out-of-bounds copy

This path is **not behavior-equivalent for invalid memory access**. It is not a successful empty substitute: the C++ implementation raises a descriptive `std::length_error` before the original heap overwrite.

The original v1.00c SHA256 is `a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897`.

`4d0380` creates metadata with `47b290`. Its constructor chain reaches `47bb90`. Thirty-two direct original-CPU factory probes found a PMR vector with size **2** and capacity **2**, providing `2 * 0x2c = 0x58` bytes. The instruction at `4d0f78` unconditionally requests a `0x420`-byte copy into this vector, equivalent to 24 commands. Neither the intervening instructions nor the construction path resizes it. The oracle does not execute the out-of-bounds copy.

`type2_reemit.cpp` preserves the recovered constructor, shot parameters and initial command-index increment. It validates the second source record and checks both copy ranges, then explicitly fails at the invalid copy. It does not silently resize the destination, suppress the instruction, invoke the original code, or claim this path passes an original-CPU comparison. Whether actual game scripts reach this path remains unverified; if they do, this limitation must be resolved before accepting complete-game equivalence.

The exact assembly is in `evidence/004d0380.asm`. Factory probe coverage and source hashes are recorded by the shared Bullet/Laser oracle; unrelated successful sampler, initialization or split comparisons do not validate this path.

The later static audit `../audit/type2_etex13_reachability.md` locates all four
ETEX13 setters and both Type2 creation sites in the supplied scripts. Both
creation sites have a dominating queue reset and then only ETEX7/3 writes,
excluding local flow of ETEX13 under the stated VM assumptions. Cross-thread
mutation, external callback re-entry, and full native/global alias behavior
remain outside that audit; this does not remove the invalid-input limitation.
