# Exact multiplayer ANM submission shortcuts

`AnmRenderer::submit` retains its original ordinary-build body. The multiplayer
WebAssembly build can avoid two conversions/arithmetic sequences, without a
cache, layout change, skipped draw, or persistent state.

- A signed-zero offset is an exact, flag-free identity for a finite normal f32
  value in every supported precision/rounding mode. Equal signed zeros are
  exact identities too. Mixed signed zeros, subnormal values, nonfinite values,
  and every nonzero offset retain the original `Scalar::add` operation.
- Viewport origin and wrapped unsigned endpoints at most `2^24` are exact f32
  integers. Ordered comparison against finite f32 bounds is therefore identical
  to the original Extended conversion/comparison, with no numeric flags. NaN,
  infinity, and larger viewport integers retain the original expression. The
  original short-circuit comparison order and strict edge inclusion remain.

All stores, later texture/material operations, callbacks, tinting, state reads,
and triangle appends retain their original order. The paths do not read the
arithmetic mode, mutate it, or clear sticky flags. Ordinary code is protected by
preprocessor branches rather than relying on optimizer inlining equivalence.

## Maintained, resource-free gate

From the repository root, with the existing WASI SDK:

```sh
WASI_SDK_PATH=/path/to/wasi-sdk node portable/check-anm-submit.mjs
WASI_SDK_PATH=/path/to/wasi-sdk TH10_TEST_UBSAN=1 node portable/check-anm-submit.mjs
```

The helper test makes 1,281,024 addition and 1,963,008 viewport comparisons per
ordinary/multiplayer invocation. It covers all three precision modes, four
rounding modes, two tininess modes, and every initial sticky-flag pattern 0–31;
signed zeros, subnormal boundaries, finite extremes, infinities, signaling and
quiet NaNs; viewport edges, `2^24` boundaries and wrapped unsigned endpoints.

The original-body test compares 2,304 complete submit calls per invocation.
It compares every raw manager byte (including the full vertex arena), VM,
sprite, quad, pipeline state, callback trace, return value, errno and arithmetic
state at identical addresses. Flush callbacks can mutate VM flags/colors,
texture, tint, vertices and arithmetic modes. Cases include flip/pixel flags,
color preservation, culling, batching, tinting, texture/state changes and
unusual scalar inputs.

`TH10_TEST_UBSAN=1` instruments C++ only. Vendored SoftFloat is explicitly
excluded; its independent zero-significand shift defect remains unchanged.
There is no sanitizer suppression, ECL change, or full-engine UBSAN claim.
