# Bounded graze scalar and call requests

## Plan and scope

The production API consumes explicit signed global graze count, signed raw power,
and original bullet XYZ. It returns scalar writes plus an ordered conditional
request sequence, not completed sounds, an effect VM, or a completed native call.
Only the eight new task-owned files are written. Existing CollisionRules,
Manager, StageWorld, tick, effect, runtime, and site files remain untouched.

1. RED -> GREEN: probe original `0x43e090` scalar prefix to the first unknown call.
2. Prove independent effect/sound ABI slices without fabricating earlier returns,
   and the original caller's already-grazed helper-2 no-call continuation.
3. RED -> GREEN: implement pure scalar planning, ordered typed requests and stable
   word serialization; replay original-byte fixtures in WASM and UBSan WASM.
4. Review only owned files, verify pinned original hashes, document call obligations.

### Design constraints and review focus

- No default initial count or power. Inputs are explicit signed int32 snapshots.
- Reject raw-power addition that would signed-overflow; do not wrap or clamp early.
- Count increments iff below 99999999; count already above the cap stays unchanged.
- Power adds 100 then clamps only its upper bound to 10000. Negative inputs remain
  signed values, and threshold sounds depend on both old and stored new power.
- Threshold sounds precede script-175/layer-23 effect allocation, which precedes
  positional sound 44. Unknown calls are never executed with fake returns.
- Already-grazed flag 4 avoids the graze call; otherwise flag 4 belongs strictly
  after real successful completion, not at plan generation or scalar update time.

## Ledger

- Original SHA-256 read and verified:
  `ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4`.
- Explicit user task authorizes inline implementation in the assigned non-Git
  checkout; no agents, branches, commits, or separate worktree are created.
- Existing bounded `bullet_probe.emulate` is reused unchanged. Probing reads the
  original EXE as data only; no process launch, API stubs, or E: outputs are used.
- Probe TDD: 8 missing-module failures RED -> 8/8 GREEN; fixture-builder assertion
  RED -> 9/9 GREEN, including complete original-byte regeneration against saved cases.
- Production TDD: both WASM replay tests RED for missing API -> both GREEN for all
  238 original-byte cases, including the UBSan build. No shared source is changed.

## Production contract

All types are in `th128::graze`; the independent header is
`src/game/GrazeRules.hpp` and the implementation is `src/game/GrazeRules.cpp`.

```cpp
std::optional<Plan> planGraze(Globals state, Position bulletPosition) noexcept;
CallWords serializeCallRequest(const CallRequest& request) noexcept;
std::optional<CallerDecision> grazeCallerDecision(UpdateSite site,
    std::int32_t helperReturn, std::uint32_t bulletFlags) noexcept;
```

`Globals` contains explicit signed `grazeCount` and `powerRaw`; it has no invented
initial-stage values. `Position` contains `horizontal`, `vertical`, and `depth`
(original world XYZ), not draw-space coordinates. Supported positions are finite
binary32 values of magnitude at most 65536, including signed zero and subnormals.
All signed count inputs are supported. Power must be at most `INT32_MAX - 100`;
the original signed-overflow domain is deliberately rejected as `nullopt`, not
wrapped or silently repaired. No lower clamp is invented.

`Plan.updated` is the scalar state **stored before the first unknown call**, not a
claim about state after arbitrary unported call effects. `Plan.requests()` is an
ordered `std::span<const CallRequest>` with exactly two or three requests. Keep
the returned Plan/optional alive while consuming this borrowed span.

`CallRequest` includes the exact original `callSite`, `callTarget`, and typed
`payload`, a variant of:

- `ThresholdSoundRequest { soundId }`: 74 or 73, only on the observed crossing.
- `EffectRequest { scriptId, layer, bankSource, position }`: script 175, layer 23,
  exact original position bits, and symbolic binding
  `EffectBankSource::BulletManagerAnm8a7f9c`. The host must resolve its actual
  equivalent of global `0x4b8930` then pointer `+0x8a7f9c`; no bank value, effect
  handle, successful allocation, or live ANM VM is fabricated by this API.
- `PositionalSoundRequest { soundId, positionX }`: sound 44 with the raw original
  world X float, not a viewport transform or a guessed pan calculation.

These are required, ordered, conditional calls, not notifications that can be
discarded while claiming the original graze is resolved. Every later request
requires earlier requests to have genuinely succeeded in the host's supported
implementation. Unknown sound/effect work remains a stop. This code does not
implement effects, sound playback, call dispatch, or native function completion.

### Scalar behavior and thresholds

Original `0x43e090 .. 0x43e133` performs:

1. Signed compare of global `0x4b4d40` against 99999999. Increment only if below;
   an existing value above the cap is left unchanged, not clamped downward.
2. Read signed raw power `0x4b4d6c`, retain the old value, add 100, then store it.
3. If the stored signed result is greater than 10000, store 10000 instead.
4. If old power `< 10000` and updated power `>= 10000`, request sound 74 at
   `0x43e0db -> 0x458410`, passing 74 in EDX.
5. Check old `< 3000` and updated `>= 3000`: sound 73 at
   `0x43e0f9 -> 0x458410`, passing 73 in EDX. Within the nonoverflowing +100
   domain, both thresholds cannot be crossed together, so at most one threshold
   sound precedes the effect. The native ordering is 10000 before 3000.
6. Effect request at `0x43e119 -> 0x407ec0`: EAX=23, ECX=resolved bank pointer;
   stack arguments are scratch output-pointer, script 175, and original EDI
   position-pointer. No scratch output-pointer value is mistaken for an allocated
   effect handle. The public request captures XYZ without mutating it.
7. Positional sound at `0x43e129 -> 0x4584a0`: EAX=44 and stack float copied from
   `[EDI]`. All scalar stores precede these calls.

The native sound-74 path reloads the power global after that call. This is not
stubbed in the proof. For this bounded input domain the old value on that path is
already >=9900, so its later old-power >=3000 check skips sound 73 regardless of
that reload. The pure plan describes the proved prefix writes and ordered requests,
not a fabricated post-sound global snapshot.

| Old power | Prefix-stored power | Threshold sound |
| --- | --- | --- |
| 2899 | 2999 | none |
| 2900 | 3000 | 73 |
| 2999 | 3099 | 73 |
| 3000 | 3100 | none |
| 9899 | 9999 | none |
| 9900 | 10000 | 74 |
| 9999 | 10000 | 74 |
| 10000 or 10001 | 10000 | none |
| INT32_MIN | INT32_MIN + 100 | none |
| INT32_MAX - 100 | 10000 | none |

### Caller gate and successful completion obligation

`grazeCallerDecision` accepts only a real, resolved helper return 0 or 2 and a
known `UpdateSite::HalfStep/FullStep`. Return 1 takes prior hit-effects paths and
is unsupported here (`nullopt`), as are other helper returns and unknown sites.
This is not authorization to manufacture a helper return or to resume with the
wrong lifecycle context.

- Return 0, or return 2 with bit 4 already set: `CallerAction::NoGrazeCall`, no
  call address, no pending flag write. In particular the latter is a valid
  no-effect collision continuation, **not a geometric Miss classification**.
- Return 2 with bit 4 clear: `GrazeCallRequired`, original call target `0x43e090`,
  and `flagsSetAfterSuccess=4`. Planning or applying scalar stores does not mean
  success. Only after the required calls really resolve may the host OR bit 4.
- This API does not write flags, apply plans, replay movement, advance timers,
  or bypass a tick host's context checks. Lifecycle resumption belongs to main.

| Update site | Graze call site | Bit-4 OR instruction | No-graze continuation |
| --- | --- | --- | --- |
| Half step | 0x408883 | 0x408888 | 0x40888b |
| Full step | 0x408b67 | 0x408b6c | 0x408c93 |

The host should retain its transaction progress: apply the prefix scalar writes
once, service requests in order, then mark the flag and resume the appropriate
post-collision continuation. A blocked request must not trigger `planGraze` and
scalar application again on every browser frame. No API here claims idempotent
retry behavior or a completed native function.

## Request serialization

`serializeCallRequest` returns ten uint32 words (`CallWords`), a portable request
record, **not** a native stack frame or an effect handle:

| Word | Meaning |
| --- | --- |
| 0 | Payload kind: threshold sound=0, effect=1, positional sound=2 |
| 1, 2 | Original call site and target address |
| 3 | Sound ID for sound requests, otherwise zero |
| 4, 5 | Effect script and layer, otherwise zero |
| 6 | Effect bank-source selector 0 = BulletManagerAnm8a7f9c; otherwise zero |
| 7, 8, 9 | Effect XYZ float bits; positional sound uses only word 7 for X |

Unconsumed fields serialize as zero, never as a fabricated call return. Float bits
are copied exactly, including negative zero and finite subnormals. If these words
are transported as bytes, encode each word explicitly little-endian. This serializer
does not dispatch a call, validate external handlers, or grant completion authority.

## Original-byte evidence

`local/research/graze-fixtures.json` has 238 bounded records:

- 162 full-function **prefix** cases: explicit count/power/position, exact original
  code bytes and SHA-256, input registers, original instruction traces, scalar
  memory before/after, and first unknown sound/effect call boundary. There is no
  prefix execution past an unported call; observed maximum is 28 of 128 instructions.
- 10 independent effect-argument cases, stopped at `0x43e119`, including sentinel
  bank values proving the pointer chain rather than hard-coding a real bank.
- 6 independent positional-sound argument cases, stopped at `0x43e129`, including
  negative zero, minimum subnormal, original depth .1 and coordinate-domain extrema.
- 40 original caller-prefix cases, spanning both sites, return 0/2, bit 4, unrelated
  and high flag bits. Clear bit 4 stops **before** the graze call; already set bit
  4 reaches the original no-effect continuation without executing it.
- 20 **independent conditional** post-call OR-prefix cases. They prove only the
  bit-4 write once earlier work has been resolved; they do not fake a graze return.

Effect and positional-sound proofs have explicit entry points after prior unknown
calls, labelled `requiresEarlierCallsResolved=true`. They are not glued into a fake
completed native trace. Each planned request links to its own first-stop proof or
the appropriate independent suffix proof. The C++ request serializer is checked
against those original machine-code register/stack observations, not a guessed
TH12 effect convention.

Report SHA-256:
`c80b778830b6b74a940eb45b8f5ef6894225b9b0b92515a376b722839ec5dffd`.
All records have `completedNativeFunction=false`. The report also states
`nativeCpuTrace=false`, `windowsGameProcessLaunched=false`, and `fakeCallReturns=false`.
Neither missing audio nor missing allocation is treated as a successful zero return.

## Reproduction and verification

From the assigned `outputs/th128-web` workspace, with already available local
pefile/Capstone and Unicorn; no dependency install or executable launch:

```powershell
$env:PYTHONPATH = (Resolve-Path '..\..\work\reverse-libs').Path
$env:PYTHONDONTWRITEBYTECODE = '1'
$env:TH128_BULLET_EXE = 'E:\gee\妖精大战争\th128.exe'
python -m unittest discover -s tests -p test_graze_probe.py -v
node --test tests/graze-rules.test.mjs
```

To regenerate only this task's private fixture:

```powershell
python tools/research/graze_probe.py $env:TH128_BULLET_EXE --report local/research/graze-fixtures.json
```

The WASM tests compile only the owned new C++ source and native harness, using the
existing Emscripten 4.0.22 SDK. Both builds use C++20, `-O1 -Wall -Wextra -Werror
-fno-exceptions -fno-fast-math -ffp-contract=off`; one adds
`-fsanitize=undefined -fno-sanitize-recover=all`. Products remain in the dedicated
`work/graze-rules-tests` directory, not runtime/site or another owner's evidence.

Final review is a separate self-review pass, not an independent reviewer sign-off:
additional agents are prohibited. Checked signed bounds, count-cap asymmetry,
threshold ordering, typed request serialization, borrowed span lifetime, original
call ABIs, suffix provenance, and no-effect caller continuation. Runtime publishing,
effect VM execution, actual initial global binding, and tick resumption remain main's
independent work; no broad runtime suite that writes shared files is invoked here.

Final scoped verification (2026-10-29): Python 9/9 passed with the pinned original
and local Unicorn, no skips; JS 3/3 passed, including all 238 records in normal
WASM and all 238 again in UBSan WASM. Python AST and JS syntax checks also passed.
The fixture digest remains the pinned report digest above; the original EXE still
has the pinned SHA-256. No unresolved review findings or deferred minors remain.
Only the eight explicitly owned deliverables are new; test products stay in work.
