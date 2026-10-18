# ANM billboard product reuse check

Run the resource-free production/original-expression comparison:

```sh
WASI_SDK_PATH=/path/to/wasi-sdk node portable/check-anm-billboard.mjs
TH10_TEST_UBSAN=1 WASI_SDK_PATH=/path/to/wasi-sdk node portable/check-anm-billboard.mjs
```

Each command builds ordinary and multiplayer variants from current sources.
Each variant checks 628,224 cases: 6,544 input cases, all 9 anchor pairs,
3 precision modes, 4 rounding modes, 2 tininess modes and 4 initial sticky-flag
patterns. Inputs include signed zeros, subnormal boundaries, infinities,
quiet/signaling NaNs and random f32 bits. The oracle contains only the original
billboard expression. It checks full vertex bytes, return values, projection
calls and their world/input values, sticky flags and errno. It uses no game
assets, renderer copy, cached outputs or performance threshold.

The optimization is selected only by TH_ENABLE_MULTIPLAYER_GAMEPLAY with WASM.
It reuses four identical ordered Extended products. All remaining arithmetic,
operand order and float-store points remain unchanged. SoftFloat flags are
sticky OR; no callback, mode change or flag read/reset occurs between the
original first and repeated evaluations. Reuse therefore retains the exact
result and accumulated flags. This reasoning would need review for a future
arithmetic backend with traps or exception hooks.

## Sanitizer scope and independent baseline failure

The normal UBSAN command instruments the C++ test, AnmProjection, Arithmetic
and GameMath. It **does not instrument third-party SoftFloat**. Test output and
build metadata identify this scope; a pass is not a full-backend sanitizer pass.

Full-backend checking is available separately and preserves a failed manifest:

```sh
TH10_TEST_UBSAN=1 TH10_TEST_SOFTFLOAT_UBSAN=1 WASI_SDK_PATH=/path/to/wasi-sdk node portable/check-anm-billboard.mjs
```

The unchanged baseline SoftFloat file with SHA-256
`2d7fed55bea49ff38635f93770e99345ef7f86721b9fa5b9eeec2d829fe5d12b`
fails this additional check. UBSAN identifies an oversized shift at
`softfloat.c:8692`, `softfloat_normSubnormalExtF80Sig`:
`z.sig = sig << shiftDist`. For a zero significand, shiftDist is 64. The caller
is `softfloat_addMagsExtF80` at line 9116. This was observed in the frozen original
expression before changing production geometry. Separately compiled candidate-first
and original-first checks retain evidence that both reach the same backend fault.
No third-party change, suppression or input filtering is part of this optimization.

Successful and failed build records include source dependency hashes, compiler
flags, sanitizer scope and per-variant results under the ignored artifacts
folders. The optional full-backend failure must stay distinct from the
C++-focused sanitizer result.
