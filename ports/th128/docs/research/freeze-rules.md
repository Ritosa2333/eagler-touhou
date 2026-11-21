# TH128 freeze-rule recovery: bounded original-byte evidence

Task D of `docs/2026-10-28-full-port-plan.md`. This is a bounded research handoff, **not** a freeze-system implementation or a native game verification. Only the fixed Japanese executable was read. No Windows game/installer process was started, no Windows API was supplied to the emulator, and no E: file was written.

## Identity, method, and reproduction

- EXE: `E:\gee\妖精大战争\th128.exe`, 752640 bytes.
- EXE SHA-256 before and after every successful probe: `ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4`.
- Libraries used for the generated evidence: pefile 2024.8.26, Capstone 5.0.9, Unicorn 2.1.4, with workspace-local `work/reverse-libs` prepended. Python and probe-script SHA-256 are recorded in the JSON.
- Authoritative machine-readable evidence: `local/research/freeze-fixtures.json`, schema `th128-original-byte-freeze-fixtures/1`.
- Each `code` entry has exact original bytes, full SHA-256, image addresses, exclusive end, file offset, disassembly, and an entry contract. Each case records its allowlisted chunks, explicit synthetic memory before/after, registers, stack input/output, instruction trace, cap, stop location, and mapped pages.
- The emulator maps only the selected original code pages, explicit data pages, a 64 KiB synthetic stack, and a return trap. Executing page padding or accessing undeclared data within an otherwise mapped page is rejected. Data is not guessed from an OS or a live game. The x87 control word is explicitly `0x037f`; the real game's control state is not established here.
- The normal cap is 1024 emulator instruction-hook visits per case. No probe supplies an API callback or a synthetic return value for an unsupported call. Calls are allowed only to another selected original-byte chunk; otherwise execution stops **before** the call instruction.
- A `returned` case completed that entry with the supplied state. `boundary` verifies only the named slice. `unsupported_call` verifies only the executed prefix and its pending call arguments. These must not be treated as completed functions.

Run from the shared workspace, with no installation/global PATH changes:

```powershell
python -B -X utf8 outputs/th128-web/tools/research/freeze_probe.py --exe "E:\gee\妖精大战争\th128.exe" --libs work/reverse-libs
$env:PYTHONPATH = (Resolve-Path work/reverse-libs).Path
$env:TH128_FREEZE_EXE = 'E:\gee\妖精大战争\th128.exe'
python -B -X utf8 -m unittest discover -s outputs/th128-web/tests -p test_freeze_probe.py -v
```

The report destination is restricted to `local/research/freeze-*.json` in this deliverable; research-directory aliases and escaping paths are rejected. `-B` prevents bytecode-cache writes. Original-byte regression tests are opt-in via `TH128_FREEZE_EXE`; without it those tests skip explicitly, rather than claiming native evidence.

## Sources and confidence vocabulary

The local Chinese manual pages `help_02.png` through `help_07.png` were inspected via the `zh` manifest parent in `local/catalog.json`. They identify the HUD resources, the 30% ordinary-freeze requirement, charge consumption, replenishment sources, area accumulation, and Perfect Freeze's purpose. They **do not** establish any numeric formula used below. In particular, the manual's wording about exceeding 100% must not override the native inclusive `>= 10000` gate.

THPrac was used only for field-name hypotheses: commit `33c78d989cca8891af07b513b468d88323c4fc1e`, local `work/upstream/thprac_th128.cpp`, SHA-256 `91d9a8b8312a4d4bfc9b7df36726983df2864966aba8b66be0cbb145a15b81c3`. The report pins this file, the manual images, the previous static index, and the existing circle fixture report individually. THPrac's labels/settings are not emulator results.

In this document:

- **Emulated** means the stated instructions executed from the pinned EXE, with the specified synthetic state and no unprovided call executed.
- **Static** means direct disassembly evidence, not a dynamically exercised whole path.
- **Candidate** marks a gameplay/object name that still needs call-graph or live-state confirmation.

## Fields and exact units

| Location | Storage and evidence | Interpretation / limit of evidence |
| --- | --- | --- |
| `0x4b4d6c` | signed dword; getter `0x410c40`; display division at `0x43f569` | Ice power: 100 raw units per displayed percent. Ordinary gate 3000; upper clamp 10000. The generic adder has **no lower clamp**. |
| `0x4b4d70` | signed dword; adder `0x427300`; use gate `0x43b7b7` | Perfect Freeze charge: 100 raw units per percent, 10000 per stock, upper clamp 30000. Cost at the examined use sites is 10000. |
| `0x4b4d60` | float32; `fld/fadd/fstp` in `0x410cb0` | Cumulative frozen-area percentage points. **Not** an integer percentage scaled by 100; not capped at 100 by the isolated adder. |
| `0x4b4d64` | signed dword; adder `0x427260` | Motivation: THPrac/manual identify 100 raw units per displayed percent. Cap 100000 (1000%). Negative values are retained and latch `0x4b4d68 = 1`, rather than clamping to zero. |
| `[0x4b8938]` | pointer loaded in activation | Candidate ice manager. The fixtures substitute a sparse synthetic manager at `0x300000`; no real game object is claimed. |
| `manager+0x6b200` | signed dword, written before slot search | Saved activation power, read by the duration and builder-argument blocks. Shared manager storage, not a per-slot field. |
| `manager+0x6b214` | float32 | Last frozen-mask area percentage used by the reward calculation. |
| `manager+0x6b218` | dword | Freeze-active **candidate**: set to 1 by a later activation instruction, tested by charge dispatch, and controls a replenishment branch. The complete set/clear lifecycle is not recovered. |
| `player+0x43910` | timer structure; current signed count at `+4`, float at `+8` | Charge/hold timer candidate. The caller passes its address to original timer helpers; do not reinterpret this structure as a single integer. |

Integer comparisons are signed. Additions are actual 32-bit x86 `add` instructions; nominal inputs do not overflow. Do not translate arbitrary out-of-range behavior to C++ signed-overflow expressions. Negative/over-cap fixtures demonstrate instruction behavior, not necessarily reachable gameplay states.

## 1. Ordinary power addition, clamping, and notifications

**Emulated entry `0x41f410`, one signed stack argument, callee cleanup `ret 4`.**

1. Read old power, add the argument, store the 32-bit sum.
2. If the signed sum exceeds 10000, store 10000.
3. Check the upward crossing of 10000 first (`old < 10000 && new >= 10000`); prepare `EDX=0x4a` and call `0x458410`.
4. Then check the upward crossing of 3000; prepare `EDX=0x49` and call the same target.
5. There is no lower-clamp instruction: `power-add-0--1` returns with -1.

The crossing cases stop before `0x458410`, with the new power already stored. That call is an unsupported notification/effect candidate, not an invented sound implementation. A large gain can statically qualify for both crossings, but because the 10000 call occurs first, the later callback's execution/effects are **not** proved by that case. Noncrossing paths return completely, including `10000 + 50 -> 10000` and negative deltas.

Other **emulated slices**, all using the same 10000 upper clamp and 10000-before-3000 callout order:

| Slice | Exact raw increment | Source attribution |
| --- | --- | --- |
| `0x417404..0x41745d` | +300 | Caller gameplay cause not established; do not label every enemy kill as +300 yet. |
| `0x43e0a7..0x43e0fe` | +100 | Function `0x43e090` also increments a separate counter. A graze-associated candidate; source and invocation frequency remain unverified. |
| `0x43fcb8..0x43fd24` | +100 if `manager+0x6b218 == 0`, otherwise +10 | Impact-associated candidate. Any nonzero flag, including -1, selects +10. Static context tests an accumulated result `> 0` once; the increment is not multiplied by that result in this block. |

These are increments **per executed block**, not verified per bullet, enemy, frame, or second. Case IDs `replenish_*` and `impact-*` preserve that distinction.

## 2. Held charge, release gate, and cooldown

The dispatch preceding the recovered slices is **static** evidence: `0x43f2e3` reads the timer; a negative value branches to a timer-update path. Bit 0 of `0x4db210`, a zero `manager+0x6b218`, and a zero `[0x4b892c]+0x3c` select the held-charge path. Otherwise the release path is used. The latter object's state is a Perfect-Freeze-active **candidate**, not a general OS/UI busy flag.

**Emulated rate-selection slice `0x43f35d..0x43f39f`.** Native context sets `EDI=45` at `0x43f348`; the fixtures supply that exact value. It prepares these arguments to `0x41f410`:

| Current timer count | Raw charge increment selected |
| --- | --- |
| `< 20` | No call in this held slice |
| `20..45` | 5 |
| `46..80` | 10 |
| `81..110` | 12 |
| `111..140` | 13 |
| `> 140` | 15 |

The charge-adder call is deliberately not substituted in these held-rate cases: the pending stack argument and `EAX` are captured. Native `push 5; call 0x41f410` at `0x43f462` separately provides a baseline +5 on that dispatch path. The `baseline-*` fixtures execute that continuation and the original power adder: input 0 becomes 5, 10000 remains 10000, and 2999 becomes 3004 before its crossing call. This is not a proved global per-frame schedule. Timer advancement/fixed simulation frequency is not established, so the counts above are not asserted as wall-clock milliseconds or measured frames.

**Emulated release gate `0x43f40c`:** with caller `EBX=0`, activation requires `timer > 0`, `timer >= 45`, `power >= 3000`, and `[ECX+0x3c] == 0`. The 45 and 3000 boundaries are inclusive. Rejected positive releases prepare a timer reset to 0.

The integrated `release-call-*` fixtures also execute the **original** `0x403280` timer setter and the activation entry, not a stub:

- Accepted release writes current timer -40, previous timer -41, and timer float -40.0, then calls activation with `EBX = player+0xecc` (three position floats).
- Rejected releases reset that timer to 0 before the next unsupported effect call.
- This supports a negative-count recovery/cooldown candidate; it does **not** establish a precisely measured 40 rendered-frame lockout or complete early-release cancellation semantics.

## 3. Activation, charge consumption, and duration

**Entry `0x4101d0`:** no stack parameters, position pointer supplied in `EBX`; plain `ret` on its complete no-free-slot path.

1. Copy current raw power to `manager+0x6b200` **before** searching for a slot.
2. Search exactly two slots, starting at `manager+0x10`, stride `0x54`, choosing the first whose leading dword is zero.
3. If neither is free, write power zero and return. This full path is emulated. It overwrites saved power and discards charge even with no slot allocated.
4. A free slot gets leading state 1, timer fields initialized/reset, `slot+0x3c = -1`, `slot+0x40 = 0`, and float `slot+0x44 = 0`. The original cdecl `0x47bb30` is executed to clear 0x34 bytes at `slot+4`; it is **not** a Python memset substitute. The three position floats are copied there.
5. The free-slot probes then stop at `0x410276`, **before** the unprovided call to `0x407ec0`. They have not completed activation, have not set the later manager active flag, and have not consumed power yet.
6. The later instruction at `0x4102d2` stores power zero. This is statically present in the successful path and separately emulated as `activation-zero-*`; it is **not** evidence that the unsupported intervening calls successfully return.

The caller subsequently reaches `0x43f462`'s +5 continuation after two unsupported effect calls at `0x43f453` / `0x43f45d`. Consequently, zero at the activation store is **not** a verified end-of-update power value: if those calls return along the displayed path with power still zero, the separately emulated continuation makes it 5. No callback was bypassed or given invented return semantics to claim a completed update.

The entry itself has no 3000 guard: the guard belongs to the examined caller. Direct-entry fixtures below 3000 demonstrate this distinction, not a usable sub-30% player action. The full-slot edge state may be unreachable through the complete live dispatch; its observed code behavior must not be advertised as a reproduced gameplay bug.

**Builder arguments, emulated slice `0x4102a0`:** before the pending `0x410590` call, the stack contains `(positionPointer, float32(50.0), trunc(savedPower / 100), 1)`. The raw-to-percent conversion uses `imul 0x51eb851f; sar edx,5; sign correction`. Thus 2999 supplies 29, 3000 supplies 30, and 10000 supplies 100. The meaning of the 50.0 argument is not proved as a radius here.

**Duration gate, emulated slice `0x4103e2..0x410409`:** calculate `D = trunc_toward_zero((savedPower - 2000) / 80)` with signed multiplication by `0x66666667`, arithmetic shift 5, and sign correction. Keep the active branch only while signed `slot+0x40 < D`; equality goes to the expiry/effect branch.

- At 3000, `D=12`: count 11 takes `0x410454`, count 12 takes `0x410409`.
- At 10000, `D=100`: counts 99 and 100 split the same way.
- At 3039, `D=12`; at 3040, `D=13`.

This gate reads the **shared saved power**, not a saved value inside each slot. The update entry has other original timer calls before this slice; those, subsequent expiry callouts, propagation updates, and exact creation-to-expiry scheduling remain unsupported.

## 4. Perfect Freeze charge, cost, and motivation

**Emulated `0x427300`, signed stack delta, `ret 4` on completed paths:** store the sum, upper-clamp at 30000, then compare signed truncated `new/10000` against `old/10000` (magic `0x68db8bad`, arithmetic shift 12, sign correction). An increased quotient goes to call `0x458410` with `EDX=0x48`, followed statically by other UI/effect calls; probes stop before the first call. The noncrossing path clamps a negative sum to zero and returns.

Examples: `0 + (-1) -> 0`; `9999 + 1 -> 10000` pending a call; `29999 + 1 -> 30000` pending a call; `30000 + 1 -> 30000` returns without a crossing; `30000 - 10000 -> 20000` returns. Do not replace the quotient-crossing condition with merely checking a displayed percent change.

**Static use sites:** `0x43b7b7` and `0x43b8d8` require charge `>= 10000`, state `[0x4b892c]+0x3c == 0`, and bit 1 of `0x4db210`, plus additional surrounding state checks. Their cost sequences push `0xffffd8f0` (-10000) and call `0x427300` at `0x43b7db` / `0x43b911`. Actual screen-wide freeze initiation and other side effects preceding/following the deduction are not emulated. The cost helper itself is fully exercised for noncrossing -10000 cases.

**Emulated motivation helper `0x427260`:** if `0x4b4d68 != 0`, return without changing motivation. Otherwise add the signed delta; an increased truncated quotient `/10000` causes an unsupported call **before** the 100000 clamp. On returning no-upcross paths, values over 100000 clamp to 100000, while negatives retain their value and set the latch. In particular, the `99999 + 2` fixture stops with 100001 already stored, before notification and clamp; claiming every stopped prefix is clamped would be wrong. Full life/death/retry state transitions are not recovered.

## 5. Frozen area and the area-to-Perfect-Freeze reward

**Static mask counter `0x410960`:** iterates 0x700 times, counts set bits in three dwords each time, advances by 12 bytes, and totals the results. This covers 172032 bits (384 * 448), from `manager+0x65de4` through the exclusive `manager+0x6b1e4`. This function was disassembled, **not** emulated in this worker; setting those mask bits, their geometry, and bullet coverage are not proved.

**Emulated percentage slice `0x40f5d3`:** read the supplied signed bit count at entry `ESP+0x24`, divide in x87 by original float64 172032.0 at `0x4a6b80`, multiply by original float64 100.0 at `0x4a6590`, and store to float32. Load that float32 again, store `manager+0x6b214`, add float32 cumulative `0x4b4d60`, and store cumulative float32. Therefore the explicit rounding/storing order matters; this is not integer hundredths or a proven arbitrary-precision area model. Counts 0, 86016, 172032 produce 0.0, 50.0, 100.0 respectively.

The complete leaf `0x410cb0` separately adds a stack float32 to cumulative area using `fld/fadd/fstp` and `ret 4`. Negative increments and totals over 3000 are not clamped by that leaf.

**Emulated reward slice `0x40f7ea`:** if saved power is signed `> 10000`, branch directly to `0x40f861` and skip this Perfect Freeze gain. The identity/purpose of that special saved-power state is unknown. For the tested ordinary saved power 10000 and finite nonnegative last-area `A`, prepare this signed argument to `0x427300`:

| Last-area domain | Native arithmetic used to prepare the argument |
| --- | --- |
| `A <= 10` | truncate toward zero `A * 50.0` |
| `10 < A <= 30` | `500 - trunc_toward_zero((A - 10.0) * -25.0)` |
| `A > 30` | 2000 |

All constants above are read from original EXE bytes. Probes execute the original conversion helper `0x48fac0` twice per area: feature flag `0x4dcd68=0` takes the x87 fallback, flag 1 takes original `fstp qword; cvttsd2si`. Both paths agree for the sampled float32 areas, but the real game's feature-flag state and the whole float/exception domain are not claimed.

Notable exact fixtures: 10.0 -> 500, 20.0 -> 750, 30.0 -> 1000, the preceding float32 29.999998092651367 -> 999, the following float32 30.000001907348633 -> **2000**. This is a real branch discontinuity in the sampled original instructions, not an interpolated manual-based reward formula. These cases stop before calling the charge helper; they do not also verify the charge-adder side effects in combination.

## Byte identities and unsupported callouts

Full exact byte hashes for **all 21** emulated code ranges are in the JSON. Important standalone identities (exclusive ends):

| Entry / slice | Bytes | SHA-256 |
| --- | --- | --- |
| `0x41f410..0x41f470` power adder | 96 | `01fd95c8b677ac66b722e84896f6a8d7633949d49b741f1de4e1da3f11820609` |
| `0x427300..0x427396` Perfect Freeze adder | 150 | `7e1f9835336d28ed89a1e0e4b12ec3ca4f8618df3df09d5a8cce65db2941f31c` |
| `0x4101d0..0x4102e1` activation | 273 | `363b704d688243d1ba144a4e6831f5c5bbb03da9b9b71c2072a22b85bbe2cf32` |
| `0x4103e2..0x410409` duration gate | 39 | `76732d6cbbfe59e0c922852f995a38ccb35403cae22f06fb05ace9e8d78305b7` |
| `0x40f7ea..0x40f85b` area reward | 113 | `c268aaf62f6b0a6af9a8435655d575162a6f3f9e9ec726a7e4571f14c1169989` |

The report separately enumerates unsupported call sites and counts. A target can be supported in one fixture group but deliberately absent from another group's allowlist:

- `0x403280`: stopped before the timer setter in gate-only cases; original bytes are actually executed in integrated release cases.
- `0x407ec0`: animation/effect construction candidate; no allocator/resource-handle return is fabricated.
- `0x410590`: ice-object builder candidate; arguments captured, implementation/return not substituted.
- `0x41f410`: stopped before the power adder in held-rate cases; separately exercised as an entry.
- `0x427300`: stopped before the charge adder in area-reward cases; separately exercised as an entry.
- `0x458410`: notification/effect candidate in threshold-crossing paths; no assumed resource-preserving callback behavior.

Other statically visible calls, including `0x4584a0`, `0x458590`, `0x424b80`, `0x425820`, `0x467480`, `0x410a60`, and `0x467420`, remain unexecuted where outside the selected slices. None is an OS/API stub.

## Validation, C++ handoff, and remaining unknowns

- TDD: 12 missing-support assertions failed before the harness existed; original-byte fixture tests were then added/observed failing before the additional fixture generation, and a directory-alias regression failed before the destination guard was fixed.
- Freeze test command above: **27 tests passed**, comprising 13 synthetic harness/safety tests and 14 original-byte fixture regression tests, with the EXE explicitly supplied. Synthetic bytes are used only for harness tests and are never labeled original game code.
- Generator: **225 original-byte cases**, with 82 completed returns, 46 intentional boundary stops, and 97 intentional unsupported-call stops. No fault, undeclared-memory, padding-execution, or budget-exhaustion stop is accepted into the generated evidence.
- An independent file-offset/hash check matched all 21 reported code ranges directly against the pinned EXE file. Reconstructing every sandbox from serialized JSON inputs alone, without the case builders, reproduced all 225 complete result records exactly. Peak usage was 119 of 1024 allowed instruction-hook visits and 118784 mapped bytes per case.
- Generated report SHA-256: `7e439c868fdf0169ee1665e839af0a91bac08b91546e3e1cb189af77144969ff`. This identifies this local evidence generation; rerunning after script/tool/reference changes can legitimately change the report hash.
- Each case names its original code ranges and supplies sufficient raw state to construct later C++ fixtures. Compare the relevant memory/stack/branch outputs, not incidental scratch-register values as if they were gameplay results. The evidence describes small original instruction contracts; no substitute gameplay model was written.
- The broad project suite is not a claimed result of this worker. Other components are owned and changed in parallel; existing project tests also create files outside this worker's strict write set. This worker's tests and probe perform no persistent writes beyond its permitted research outputs.
- Do not infer propagation from the previously probed circle predicate. Its 519 cases establish only an inclusive circle-overlap candidate for the sampled domain, not how bullet types, lasers, flames, eligibility flags, ice chains, or masks use it.
- Still unknown: ordinary freeze geometry/growth, propagation scheduling and eligibility, chain termination, laser-induced breakup, flame behavior, full Perfect Freeze initialization/lifetime, miss/death/cancel interactions, active-flag clearing, simultaneous-slot invariants, precise RNG/effect/resource dependencies, live FPU state, actual per-frame event frequency, and route/difficulty differences.
- **No native-CPU equivalence or full-game fidelity claim** follows from these results. The original executable was never run as a Windows game process.
