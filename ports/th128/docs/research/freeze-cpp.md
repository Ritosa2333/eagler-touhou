# Portable TH128 freeze scalar rules

## Scope and implementation plan

This component implements only the pure integer gates, charge arithmetic, and bounded float32-input area-reward calculation evidenced in `freeze-rules.md` and the pinned `local/research/freeze-fixtures.json`. It does not start the original executable or implement a freeze manager, propagation, timers, notifications, rendering, input dispatch, or a complete update/lifecycle.

The implementation is confined to `src/game/FreezeRules.hpp` / `.cpp`; C++ fixtures live in `tests/native/freeze-rules.cpp`, the Emscripten/Node runner in `tests/freeze-rules.test.mjs`, and this document records the contract and results. Generated test output is `artifacts/tests/freeze-rules.cjs` with its `.wasm` companion. Earlier research files remain read-only.

Plan executed: write the C++/WASM fixture first; observe its missing-implementation build failure; implement the scalar API; compile with Emscripten 4.0.22 and run both fixed and serialized original-evidence vectors; audit integer/float boundaries and preserve the earlier evidence identities.

## API contract

All functions are pure and `noexcept`, in namespace `th128::freeze`. `Raw` below means `std::int32_t` in the actual signatures, not a new type. Integer charge units are hundredths of a displayed percent: 3000 = 30%, 10000 = 100%. Counts are original signed timer counts, **not measured milliseconds or proven rendered frames**.

| Signature | Contract |
| --- | --- |
| `bool canRelease(Raw powerRaw, Raw heldCount, Raw perfectState)` | True only for `powerRaw >= 3000`, `heldCount >= 45`, and `perfectState == 0`. This is the isolated caller gate, not activation. |
| `Raw durationCount(Raw savedPowerRaw)` | Signed truncation toward zero of the original 32-bit subtraction `(savedPowerRaw - 2000)` divided by 80. Ordinary evidenced saved-power domain: 0..10000. |
| `bool durationPending(Raw savedPowerRaw, Raw elapsedCount)` | Strict `elapsedCount < durationCount(...)`; equality is the expiry-side branch. No timer is advanced. |
| `Raw addPowerRaw(Raw powerRaw, Raw deltaRaw)` | Original wrapping dword addition, signed upper cap 10000, **no lower cap**. Only the numeric store is reproduced; no crossing callback is run. |
| `Raw baselineRechargeRaw(Raw powerRaw)` | The separately proved `+5` continuation followed by the above numeric store. Calling it every frame or automatically after activation is outside this contract. |
| `std::optional<Raw> heldRechargeRaw(Raw heldCount)` | No selected call below 20; otherwise 5 for 20..45, 10 for 46..80, 12 for 81..110, 13 for 111..140, 15 above 140. This selects a call argument, not a completed native charge update. |
| `Raw activationStoreRaw()` | Returns the zero written by the isolated activation reset store. Not an end-of-update power value or proof of successful slot allocation. |
| `Raw addPerfectFreezeRaw(Raw chargeRaw, Raw deltaRaw)` | For nominal input charge 0..30000 and nonoverflowing changes, the numeric clamped result is 0..30000. The implementation preserves wrapping dword addition without C++ signed-overflow undefined behavior. No sound/UI calls or quotient-crossing effects. |
| `std::optional<Raw> spendPerfectFreezeRaw(Raw chargeRaw)` | For charge 10000..30000, returns charge minus 10000; otherwise no result. This is an affordable nominal-charge numeric deduction, not a Perfect Freeze activation/state gate. |
| `std::optional<Raw> areaRewardRaw(float lastAreaPercent, Raw savedPowerRaw)` | For finite area 0..100 and ordinary saved power 0..10000, returns the proved reward call argument; saved power above 10000 bypasses it. Other unsupported inputs return no result, never silently a zero reward. |

The header publishes named raw-unit thresholds/caps/cost and the +5 continuation increment. No mutable state or dependency on engine, renderer, host, or asset readers is introduced.

## Evidence policy

The golden-data runner pins the original EXE identity and the entire prior report SHA-256 `7e439c868fdf0169ee1665e839af0a91bac08b91546e3e1cb189af77144969ff`. It derives expected integer memory stores, branch outcomes, and pending stack arguments directly from report bytes. It must retain separate categories for a completed original return, an intentional slice boundary, and a value observed **before an unsupported call**. The 97 unsupported-call cases in the source report are not 97 completed-rule verifications.

## Floating-point and state boundaries

`areaRewardRaw` takes an IEEE binary32 `float`, not integer hundredths and not an unconstrained binary64 value. Invalid/nonfinite, negative, or over-100 area inputs return `std::nullopt`; negative saved power is also outside this API's supported domain. These invalid-input guards are host safety policy, **not** a claim that the original executable rejected those values. A valid zero-area reward returns the integer zero, which is distinct from the absent/bypassed reward path.

The function promotes the float32 input to `double` and uses the original constants and truncation order:

- `A <= 10`: truncate `A * 50.0` toward zero.
- `10 < A <= 30`: `500 - truncate((A - 10.0) * -25.0)` toward zero.
- `A > 30`: 2000.

For the accepted binary32 inputs in these small arithmetic ranges, the intermediate subtraction/product fits binary64 precision; no rounding-mode-dependent integer conversion or reassociated float32 calculation is substituted. The exact preceding float32 of 30 is `0x41efffff` and yields 999; 30 itself yields 1000; the exact successor `0x41f00001` yields 2000. The golden runner reconstructs float32 arguments from their **recorded raw bits**, not a locale-dependent decimal parse. Source fixtures cover both original x87 and SSE conversion-helper branches; neither branch is executed by this C++ module.

All integer parameters/results are explicitly signed 32-bit. `durationCount` and charge addition use defined unsigned arithmetic plus `std::bit_cast` to preserve the original two's-complement dword operation; division uses C++ signed truncation toward zero. Out-of-range/wrapping inputs are not promoted into newly verified reachable game states. The Perfect Freeze numeric clamp is supported for nominal charge 0..30000; it is not an emulation of the original notification call sequence for arbitrary invalid starting charge.

`spendPerfectFreezeRaw` checks affordability within the nominal cap and computes the deduction only. Its no-result behavior for over-cap input is an explicit host-domain guard, not inferred native clamping. It does not check Perfect Freeze activity, input bits, player state, deathbomb timing, or the native manager's existence.

`activationStoreRaw()` represents only the zeroing store. A later caller continuation adds 5 **if that original path is reached**. The component never calls `baselineRechargeRaw` automatically, never treats activation's zero as an end-of-update state, and never combines the held/baseline increments into an invented per-frame scheduler.

## Test and build commands

From the shared workspace:

```powershell
node --test outputs/th128-web/tests/freeze-rules.test.mjs
node outputs/th128-web/artifacts/tests/freeze-rules.cjs
```

The test runner follows `tests/native-random.test.mjs` but owns only its own fixture artifact. It invokes the SDK's Python and compiler directly (no installation or PATH changes):

- SDK root: `work/tools/emsdk-package/emsdk-main`.
- Python: `<sdk>/python/3.13.3_64bit/python.exe`.
- Compiler: `<sdk>/upstream/emscripten/emcc.py`.
- `EM_CONFIG=<sdk>/.emscripten`, converted to forward slashes.
- `EM_CACHE=<workspace>/work/emscripten-cache-4.0.22`, converted to forward slashes; the normal compiler/cache sanity behavior is not disabled.
- `EMSDK_PYTHON=<bundled Python>`, `PYTHONUTF8=1`, `EMCC_CORES=2`, `BINARYEN_CORES=2`.
- Arguments: `tests/native/freeze-rules.cpp src/game/FreezeRules.cpp -Isrc -std=c++20 -O1 -Wall -Wextra -Werror -fno-fast-math -ffp-contract=off -sENVIRONMENT=node -sEXIT_RUNTIME=1 -sASSERTIONS=1 -o artifacts/tests/freeze-rules.cjs`.

Do not compile this numeric contract with fast-math/finite-math assumptions or change the input type to double. The SDK config and default SDK cache are not edited. Emscripten emits `freeze-rules.cjs` and its `freeze-rules.wasm` companion in `artifacts/tests`; no main build/host/renderer is changed.

If the optional SDK is absent, compile/run tests skip visibly; if the private local evidence file is absent, provenance/original-evidence tests skip visibly. A missing module or a mismatched source-report hash is a failure, not a skip. `TH128_EMCC` / `TH128_PYTHON` can explicitly override the default compiler/Python paths as in the existing native-random test pattern.

## Validation results

- **TDD red:** after writing both fixture files and before writing either implementation file, `node --test outputs/th128-web/tests/freeze-rules.test.mjs` had two passing provenance/Unicode checks and two failing C++/WASM tests. Both failures reported the missing `src/game/FreezeRules.cpp` compiler input.
- **TDD green:** the same command passes **4 tests, 0 failures, 0 skips** after implementation. The C++ fixture first passes **54 fixed scalar/guard checks**, then the evidence-driven invocation passes **169 scalar queries** drawn from **153 distinct original-byte cases**.
- Original evidence categories remain separate: **65 completed emulator returns**, **24 proved slice boundaries**, and **64 scalar values observed before unsupported calls**. The last category is **not** full native-function or full-rule verification; it covers already stored numbers, pending arguments, or a gate branch only. The two formula operations derived from each duration case and the affordable-cost queries derived from returning deduction cases explain why query count exceeds source-case count.
- Original-evidence coverage includes the inclusive 3000/45 release gate, signed integer duration/equality boundary, returned cap/deduction behavior, isolated zero/+5 stores, held-charge pending arguments, both area-reward conversion-helper fixture paths and their 30% discontinuity. Invalid-input guard checks are identified as host policy in the C++ fixture.
- SHA-256 checks confirm `freeze_probe.py`, `test_freeze_probe.py`, and `freeze-fixtures.json` are byte-for-byte unchanged from the preceding research task. The report still identifies the same fixed EXE and is not regenerated by this implementation's tests.
- A follow-up review found three genuinely corrupted Unicode paths in the earlier `freeze-rules.md`, not just a display issue. Those three strings were repaired through Python Unicode escapes and a Unicode-safe patch invocation. A regression test now verifies every copyable EXE path in that document against the exact original Unicode path; the original path also matched the unchanged JSON identity and existed on this machine. That narrowly authorized correction is the only change to the earlier files.
- This test fixture executes the portable C++ code as WebAssembly under Node. It is **not** a native Windows CPU trace, original-game run, proof of native compiler equivalence, or complete-game comparison. No original E: data is written or game/installer process started. No commits are created.
- Main build/UI/rendering integration and the broad concurrently changing project suites are intentionally left to the main worker. No source outside this component is linked into the fixture; no propagation, input scheduling, active-flag lifecycle, sound/resource callback, motivation, cumulative-area update, or full Perfect Freeze lifecycle is implemented.
