# Bounded player-death rules

## Scope and execution plan

Only the eight explicitly owned new PlayerDeathRules/probe/test/doc/fixture files
are written. Frozen Hit/Graze/Collision, main NativePlayerWindow, World, effects,
runtime, and shared tests remain untouched. No agents, branches, commits, original
process launch, installer, or E: output. The original pinned EXE is data only.

1. RED -> GREEN: original call/dataflow and signed scalar-helper probes.
2. Prove independent state-2 timer, record-cleanup, tail, and respawn suffixes,
   preserving all earlier unknown-call obligations and continuations.
3. RED -> GREEN: portable explicit-snapshot C++ prefixes and required request plans;
   replay original-byte cases in normal WASM and UBSan WASM.
4. Review owned scope, document unresolved effects/initialization, freeze results.

The original death entry first calls 427260(-10000), then 427300(+2500). It does
not reset raw power or write state 2 before those calls. Later state-2 setup writes
protection counter 180; a separate state-2 update checks f84 >=30 before taking
respawn/negative-resource scene paths. Actual repositioning and counter 280 occur
after further required calls. Neither death nor game-over is fabricated by writing
the last scalar states first. Globals 4b4d64/68/30 retain neutral field identities
unless their exact behavior, rather than a TH12 stock/lives analogy, is proved.

## Public production API

`src/game/PlayerDeathRules.hpp/.cpp`, namespace `th128::player::death`, is independent
of frozen player-hit/graze/collision rules. It allocates no dynamic storage and
mutates no supplied snapshot. Every optional `nullopt` means **unsupported**, not
successful death, a missing handle, or permission to continue. There are no implicit
live initialization values. Only a host with the correct real phase may apply a
prefix's writes; no public function grants authorization to skip earlier calls.

`Timer` has explicit `previous`, `current`, `fractional`, `clockSource`, `flags`.
The first two are signed int32, the last two uint32. Timer fractions and supplied
XYZ must be finite binary32 of magnitude <=65536, including signed zero/subnormals.
`Position` fields are `horizontal/vertical/depth`, actual player `+ecc/+ed0/+ed4`.
Timers bind state `+f80..+f90` and protection `+438d8..+438e8` as in frozen HitRules,
but use independent local types; no frozen header is edited or included.

### Commit calls and real resource helper

`commitCalls()` returns exactly two required requests, in original order:

1. `43cddf -> 427260`, signed stack delta **-10000**.
2. `43cde9 -> 427300`, signed stack delta **+2500**.

Neither request applies a scalar write or manufactures a call return. Complete the
first before the second, then proceed to the independent post-awards prefix only
after the second and any nested requests have resolved.

`motivationAfterDeath(MotivationSnapshot)` implements the actual `427260` helper
for this fixed negative delta and the explicit globals `value4b4d64/flag4b4d68`:

- Nonzero signed flag: leave both globals unchanged, even if the resource is above
  its normal cap or below zero. No subtraction occurs on this branch.
- Flag zero: reject values below `INT32_MIN+10000`; otherwise subtract 10000.
- Clamp only an upper result >100000 to 100000; if result <0 set flag to 1.
  Negative resource is **not** clamped to zero.
- Within this nonoverflowing fixed-decrement domain the signed truncated /10000
  bucket cannot increase, so the helper's increase-only sound/notification path
  is not reached. Golden cases execute the entire real helper and RET 4.

This establishes signed raw units, a guard flag, and branch behavior. It does not
establish an integer-stock/lives layout or assign a new game's resource value.

### Perfect-charge helper and nested mandatory requests

`chargeAwardPrefix(explicitPerfectCharge)` implements the original `427300(+2500)`
stores and first call decision. Reject input >`INT32_MAX-2500` before addition.
Compute `updated=min(old+2500,30000)` using signed values. Compare truncated signed
quotients `updated/10000 > old/10000`:

- No increase: clamp a negative result to zero and report
  `completedIndependentHelper=true`, with zero required requests. These fixtures
  execute the real helper return, not an injected zero-return stub.
- Increase: preserve that prefix-stored updated value and return the **three
  ordered mandatory** requests below, with `completedIndependentHelper=false`.
  Original execution stops before the first. Independent suffix proofs establish
  the other two; they are not concatenated into a fake completed trace.

| Request | Original site -> target | Actual arguments/binding |
| --- | --- | --- |
| Sound | 427355 -> 458410 | EDX=72 |
| Charge notification | 427369 -> 424b80 | EAX=2, stack (2,0), EDI=global 4b8950 |
| Charge UI operation | 427375 -> 425820 | stack global 4b8950 |

`requiredCalls[0..requestCount)` is the valid queue; the unused array capacity is
not another request. `chargeCompletionCalls()` exposes the latter two requests for
a host resuming after genuine sound completion. It does not claim that sound ran.
The queue cannot be discarded because audio/UI work is inconvenient.

Negative cases are material: old -10000 becomes -7500 and requests sound 72 because
truncation changes bucket -1 to 0. That sound branch does not perform the no-sound
lower clamp. Old -12500 instead reaches a complete no-sound return with stored 0.
Old 7500 reaches 10000 with sound; old 10000 reaches 12500 without sound. Raw power
`4b4d6c` is distinct from this perfect charge `4b4d70`.

### Post-awards power and state-2 setup

`afterAwardsPrefix(AwardControl)` covers `43cdee..43ce17`, **after** real completion
of both initial calls. It unconditionally stores raw power **10000**. For explicit
signed `selector4b4cfc>=0`, it requires `43ce12 -> 41fc80`, stack (resolved global
4b8950, selector4b4cfc, value4b4d00). Negative selector reaches `43ce17` without
that call. No null control pointer or working UI is fabricated.

`stateTwoSetupPrefix(stateTimer, protectionTimer)` is a later independent suffix,
valid only after that conditional control request resolves. It writes:

- State `f78=2`.
- State timer previous=-1/current=0/fractional=0.
- Protection timer previous=179/current=180/fractional=180.
- For an uninitialized timer, set flags bit 0 and bind clock `0x4b6f58`; preserve
  supplied clock/all flags when already initialized.
- Require player script **0** at `43cec6 -> 4593b0`, EBX=0,
  EAX=player+14 and EDI=player field +10. Continuation is `43cecb` only after the
  actual animation call succeeds.

There is no position/raw-coordinate/focus-timer rewrite in this setup. Main's
ordinary f84 progression remains outside this module. Values 180 here, 280 below,
and frozen hit values 6/8 remain different counters/thresholds.

### Eight-record cleanup is ordered, not fake bulk retirement

The original uses eight records at `player+8864 + index*e4`. For each record,
`[record-b0]` (first = player+87b4) is cleared before processing handles at
`[record]` and `[record+4]` (first = +8864/+8868). Handle fields themselves are not
zeroed or declared retired by this prefix.

`cleanupRecordPrefix(record, CleanupEntry::RecordStart)` clears **only this record's**
active field, then stops at the first needed live-animation lookup:

- Nonzero primary: `requiredHandle=primary`, phase 0, lookup instruction `43cef4`,
  conditional next boundary `43cf8c`.
- Zero primary/nonzero secondary: phase 1, lookup `43cf97`, next boundary `43d02c`.
- Both zero: no lookup required; record reaches `43d02c`.

`CleanupEntry::AfterPrimary` starts independently at `43cf8c`, preserving the
supplied current active field; it is valid only after primary handling truly
resolves, and checks the supplied current secondary handle. Unknown enum values
return nullopt. Do not clear a later record's active field while an earlier record
is blocked; perform the native stride/count step only after this record resolves.

A nonzero `requiredHandle` is an **inline resolution obligation**, not a made-up
call to a resolver function. Search actual live lists from global `4d2e50` fields
`+8b9704/+8b970c`. If a VM is found, invoke its `+4c8` callback with EDX=1 if nonzero,
then and only then write word IRQ1 to `+3f8`. If VM `+18==0`, walk children at
`+14` and apply the same callback-before-IRQ ordering. Original possible callback
sites are primary root/child `43cf46/43cf74`, secondary root/child
`43cfe6/43d014`. This module neither resolves unknown handles nor invents their
absence, calls unknown callbacks, or reports those IRQ writes complete.

### Independent death tail

`tailPrefix(TailSnapshot, TailControl)` is conditional on all cleanup/animation
obligations already resolving. Original `43d03e..43d0b1` clears player `+438f8`.
Global manager `4b8934` fields remain neutrally named flags7c/counter28/value80:

- With flags mask 0x1 clear, leave them unchanged.
- Otherwise counter28>=60 applies mask `0xffffffb9` and sets value80=0.
- Otherwise require known nonnull control global `4b892c`; mode `+3c!=0` ORs
  flags with 0x40, zero mode leaves them unchanged. Missing control is unsupported.

Then subtract **1024** from explicit `scalar4b4d30` and clamp to [-1024,1024]. Reject
input below `INT32_MIN+1024` rather than overflowing before clamping. This scalar
is not relabelled lives or motivation. Tail proofs run the real epilogue with a
bounded saved-register frame: `completedIndependentTail=true`, but
`completedNativeFunction=false` remains false for the complete death callee.

## Separate later respawn prefixes

`respawnGatePrefix(stateF78, elapsedF84, resource4b4d64, scene, position)` supports
only **state 2** and bounded explicit position:

- Signed f84<30: `Waiting`, no writes/calls on this branch, boundary `43b9ee`.
- f84>=30 and signed resource<0: a known scene pointer (`4b8a84`) is required;
  explicit `scene.mode10==1` requests `43b925 -> 437050`, otherwise
  `43b88d -> 438030`. Missing scene is unsupported, not guessed game-over. No state,
  position, power, or timer reset is invented before these calls.
- f84>=30 and resource>=0: write state **0** and global clock **1**, then require
  `43b964 -> 43df90`, ECX=player and stack (player+ecc, float32 32, float32 16,
  int 30, int 150, int 0). This is an actual bounded-record field allocation call,
  not a completed item spawn or automatic bullet-clear effect. Position binding
  resolves from the same actual player context; its pointer is not serialized as
  an invented handle or transformed draw-space position.

`respawnAfterFieldPrefix(explicitDifficulty4b4d04)` independently covers the next
branch. Difficulty<2 requires `43b977 -> 427190`, EAX=global-state address 4b4cc0.
Other signed difficulties reach `43b97c` without the call. This operation writes
global state and may call further control code; it is not silently omitted.

`respawnPositionPrefix(stateF78, oldPosition, protectionTimer, stateTimer)` is valid
only in state **0**, after the previous real field/control work has resolved:

- Save old XYZ bits to player `f58/f5c/f60`.
- Set actual X=+0, Y=480, preserve supplied Z bits.
- Raw XY becomes (0,61440), exactly 480*128, not normal movement bounds (432).
- Protection timer becomes previous=279/current=280/fractional=280.
- State timer becomes previous=-1/current=0/fractional=0; preserve/init timer
  clock/flags as the real `403280` setter does. The probe actually executes that
  pure helper twice; it is not an API stub.

No focus timer is assigned a default. The tail's opaque `438f8=0` is the proved
record-count reset; focus/input mode, subsequent state-0 movement, and transition
to active play are outside these prefixes. No function here advances f84 or
provides full respawn scheduling/context authorization.

## Portable call packets and provenance

`serializeCallRequest` emits ten uint32 words: kind, original site, target,
symbolic binding, then six operand words. Operand words preserve signed int bits
or exact float bits; zeros in unused fields are padding, **not native returns**.
Binding IDs are None=0, global Control4b8950=1, PlayerAnimationField10=2,
PlayerPositionEcc=3, GlobalState4b4cc0=4. The kind fixes the ABI described above;
these are not native stack frames or dereferenceable original pointers.

The saved `local/research/player-death-fixtures.json` contains 607 independently
bounded cases. Every code/constant slice has original bytes and SHA-256; cases
record explicit inputs, registers, state spans, exact timer/position bits, complete
instruction traces, stops, and actual packet arguments. No unknown call appears
in its executed trace. Independent suffixes carry `requiresEarlierCallsResolved`;
their composition is not a whole native-function proof. Complete independent
scalar helpers are labelled as such, never as a completed death function.

Fixture SHA-256:
`b15a3ded92a89cb5517711d1f69fec11af38e5be89575b9a7491633abab02a39`.
Pinned original EXE SHA-256 before/after:
`ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4`.
Report and cases retain `completedNativeFunction=false`; report flags also include
`nativeCpuTrace=false`, `windowsGameProcessLaunched=false`, `fakeCallReturns=false`.

## Reproduction and limits

From the assigned outputs/th128-web workspace, using already available local
pefile/Capstone/Unicorn and Emscripten 4.0.22; nothing is installed:

```powershell
$env:PYTHONPATH = (Resolve-Path '..\..\work\reverse-libs').Path
$env:PYTHONDONTWRITEBYTECODE = '1'
$env:TH128_BULLET_EXE = 'E:\gee\妖精大战争\th128.exe'
python -m unittest discover -s tests -p test_player_death_probe.py -v
node --test tests/player-death.test.mjs
```

Regenerate only this task's private evidence:

```powershell
python tools/research/player_death_probe.py $env:TH128_BULLET_EXE --report local/research/player-death-fixtures.json
```

C++ builds compile only the owned source/harness with C++20, `-O1 -Wall -Wextra
-Werror -fno-exceptions -fno-fast-math -ffp-contract=off`. The second build adds
`-fsanitize=undefined -fno-sanitize-recover=all`; products stay in the dedicated
`work/player-death-tests`, not shared runtime/site or publication.

Initial probe tests were RED for absent module, then GREEN; fixture-builder test
was RED before it existed. Normal/UBSan C++ tests were RED for missing production
files before implementation, then GREEN for all 607 original-byte cases. Complete
fixture regeneration is checked by the Python test, not merely selected fields.

Self-review checks signed arithmetic boundaries, truncated negative charge buckets,
call/record ordering, explicit nullable controls, timer clocks, prefix versus return
labels, request serialization, and the frozen ownership boundary. It is not an
independent review. Actual callbacks/ANM/control/scene/field work remain required;
live initial snapshots, ordinary state-timer progression, lives/HP semantics,
full OS/effects execution and world integration are not claimed. No broad shared
runtime/publication tests are run on main's active integration path.

Final focused verification (2026-10-29): Python 13/13 passed with the pinned
original and local Unicorn, no skips, including identical regeneration of all 607
saved cases. JS 3/3 passed: evidence validation, all 607 cases in normal WASM,
and all 607 again with UBSan. AST and JS syntax checks passed. Maximum original
case length is 52 instructions; no case exhausted its bounded instruction budget.
Original/report hashes remain pinned above. No actionable self-review finding or
deferred minor remains. Component is frozen for main's independent review;
the explicit unknown-call obligations remain unresolved integration work.
