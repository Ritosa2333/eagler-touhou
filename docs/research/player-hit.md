# Bounded player-hit state prefixes

## Plan and constraints

1. RED -> GREEN: original-byte probes for confirmed-overlap gates, first hit entry,
   conditional timer suffixes, state-4 window, and independent return-tail rules.
2. Preserve original call boundaries and state writes in private golden fixtures.
3. RED -> GREEN: independent production C++ prefixes and required call-request
   serialization, replayed in normal WASM and UBSan WASM.
4. Self-review only owned code; verify original hashes and document uncertainties.

No initial player state, invulnerability, counters, power, HP, or motivation is
invented. All consumed snapshots are explicit. Previously completed GrazeRules,
CollisionRules, movement, main runtime/effects/tick files are frozen and untouched.
The assigned non-Git checkout is used in place; no agents, branches, or commits.
The pinned original EXE is read as data only, never launched or modified.

Important recovered distinction: `0x43d0c0` sets `+f78=4` before its first unknown
effect-allocation call. Only after a script-83 effect and 32 more effect iterations
does it reset timers. The subsequent state-4 branch tests `+f84 < 8`; the separate
hit-protection timer `+438dc=6` is not that eight-tick window. No API may skip earlier
unported effects in order to apply later suffix state writes.

## Public API and explicit snapshots

The independent implementation is `src/game/PlayerHitRules.hpp/.cpp`, namespace
`th128::player::hit`. It has no dependency on existing movement, collision, graze,
Manager, StageWorld, tick, runtime, animation, or effect code. Requests are not
executed by this API. No function provides invented initial player snapshots.

`Timer` fields correspond to a native 20-byte timer, in order:
`previous` signed int32, `current` signed int32, `fractional` binary32,
`clockSource` opaque original pointer identity, `flags` uint32. Every field is an
explicit input. Signed integer and flag domains are complete; floats must be
finite of magnitude at most 65536, including signed zero and subnormals. Unsupported
input is `nullopt`, not a no-op or a successful player hit.

| Timer binding | previous | current | fractional | clockSource | flags |
| --- | --- | --- | --- | --- | --- |
| State | +f80 | +f84 | +f88 | +f8c | +f90 |
| Focus | +f94 | +f98 | +f9c | +fa0 | +fa4 |
| Hit counter | +438d8 | +438dc | +438e0 | +438e4 | +438e8 |
| Recharge | +438fc | +43900 | +43904 | +43908 | +4390c |
| Held freeze | +43910 | +43914 | +43918 | +4391c | +43920 |

`Position` fields are `horizontal/vertical/depth`, the actual float mirrors at
player `+ecc/+ed0/+ed4`, not raw integer coordinates or viewport coordinates.
Existing PlayerRules movement uses raw int32 XY at `+ed8/+edc` and mirrors at
1/128; this API does not change that representation or movement scheduling.

### Confirmed circle-overlap gate

`gateDecision(PlayerGate player, HitControl control)` assumes actual confirmed
circle overlap. It does not perform geometry, and it must not be called with a
fabricated positive-overlap result. It consumes only player state `f78`, flags
`438ec`, counter `438dc`, and explicit control presence/counter from global
`0x4b8950 -> +5a1c`.

- Nonzero present control counter, states 2/4/3, or `flags438ec & 2` suppress the
  hit path: proved complete circle helper return 0.
- Otherwise signed counter >0 gives `HitWithoutHitCall`, proved helper return 1
  **without entering 43d0c0**. Bullet hit/cancellation effects are still required
  by the caller; this is not an instruction to ignore contact.
- Counter <=0 gives `HitCallRequired` at `43c648 -> 43d0c0`; no helper return is
  provided. The helper's eventual 1 cannot be invented before that call resolves.

The address is the circle-helper caller used by type 8, not an assertion that every
other collision helper has the same call site. The original-byte gate fixtures use
explicit zero XY and size/profile 2 to ensure real positive geometry, then execute
the original helper. They do not assume the runtime gates equal those fixture values.

### Normal initialization and constructor boundary

`normalInitPrefix(stateTimer, focusTimer, rechargeTimer, flags438ec)` executes the
pure part `43ab20 .. 43ac1e`:

- Set state `f78=1`.
- Recharge timer becomes previous=-2/current=-1/fractional=-1.
- State and focus timers become previous=-1/current=0/fractional=0.
- Clear the low three bits of `flags438ec`; zero fields `f70` and `f74`.
- If a timer's low flag bit was clear, set it and bind clockSource to `0x4b6f58`;
  otherwise retain its supplied clock and all flags.
- Stop with `NormalInitDetach` at `43ac1e -> 4679b0`, ESI=player+87a8.

This prefix **does not read or set hit counter 438dc**. Later detach/control/player
calls remain unknown and are not bypassed. It is not a complete new-game initializer.

`constructorCounterPrefix(explicitInitializedTimer)` independently proves only
`43aadb .. 43aafb`: previous=119/current=120/fractional=120. Clock and flags remain
the caller's explicit initialized snapshot. A clear timer-initialized bit is
unsupported because that state requires the preceding constructor initialization,
which this slice does not execute. The original constant is float bits `0x42f00000`.

Therefore **120 is a proved constructor write, not an assumed live counter at a
later game frame**. Neither zero nor 120 is installed as a default. The complete
constructor-to-normal-init handoff, later calls and scheduling still require main's
own verified context. Supplied raw power 10000 is not inferred to initialize these
timers or to equal perfect-freeze charge.

### Ordinary hit-counter step

`counterStep(counter, optionalResolvedClock)` is only the `438d8` timer's decrement
operation, not a generic state-timer increment or complete player tick:

- If signed current <=0, leave every timer field unchanged. No clock dereference
  occurs in the original; `std::nullopt` clock is allowed in this branch.
- Positive current requires the supported native global-clock binding `0x4b6f58`
  and an explicit finite resolved clock in [0,4]. Unknown/different binding is
  unsupported rather than assigned clock 1.
- Copy current into previous. For strict
  `0.9900000095367431640625 < clock < 1.0099999904632568359375`, subtract 1;
  otherwise subtract the actual supplied binary32 clock. Store fractional binary32
  **before** truncating toward zero into current. Clock and flags do not change.
- If that stored fractional result exceeds the bounded float domain, reject rather
  than introducing undefined conversion behavior.

The proof uses unchanged `enemy_probe.emulate`, runs the original 2,000-record loop
with explicitly inactive synthetic shot records, then the actual counter prefix
and SSE2 `48fac0` conversion helper. This establishes the original FPU stack
without injecting instructions or fake helper returns. Five bounded <=64KiB player
regions hold the explicit synthetic records; each main run has a 15,000-instruction
budget (observed maximum 12,042). Large regions are saved by before/after hashes.
The SSE2 selector at `4dcd68` is explicitly 1, not a claim about a captured game CPU.

At clock 1 the original gives 120->119, 6->5 and 1->0; a positive counter with
fractional 6 and clock .5 stores fractional 5.5 then current 5. A current zero or
negative value is not decremented further. This does not establish when main may
schedule the operation relative to enemy/bullet phases; do not apply it twice or
pretend it also increments f84.

## First hit entry and required effects

`entryPrefix(stateF78, playerPosition)` models `43d0c0 .. 43d11b`. The native
function unconditionally overwrites state with **4**; its old-state input does not
gate this entry. Only a caller with a genuinely required hit call may enter it.
The result writes no counter, HP, motivation, power, or invulnerability default.

It computes a **deferred** effect position with original constants and float stores:
X+128+192, Y+16, unchanged Z. The first allocation itself passes EAX=0 (null position
operand), ECX=bank from `4b8930 -> +8a7f9c`, and stack scratch output-pointer,
script **83**, layer **0**, flags **0**. The first required request is
`43d11b -> 4670b0`. The computed position is not falsely serialized as that call's
position-pointer argument or an allocated handle.

The remaining original sequence, statically identified but **not executed** here:

1. Lookup the real returned handle at `43d12a -> 467420`, global manager `4d2e50`.
   If nonnull, write deferred XYZ to its `+464/+468/+46c` fields.
2. 32 effect iterations: conditional Windows synchronization at IAT `49a088` and
   `49a08c` if `4d33f8 & 0x8000`, increment actual bank counter `+130`, allocate via
   `468010`, set real VM fields, load script **84** via `4593b0`, attach via `466d60`,
   look up live handles, and conditionally write deferred positions.
3. Only after that loop, enter timer/sound/animation prefixes below.

These are named pending operations, not a fake 33-effect completed plan. No Windows
call, allocation, ANM, or handle lookup returns zero merely to make progress. Main
must implement/support those effects or retain an explicit stop after state 4.

## Conditional post-effects timer suffixes

Only call these APIs when the preceding real hit effects are genuinely resolved.
Their golden cases are independent suffix entries, not a fabricated trace that
jumps over unknown calls.

`timerResetPrefix(stateTimer, SoundControl)` proves `43d264 .. 43d2d1`:

- Set state timer previous=-1/current=0/fractional=0. Initialize clock/flag only
  when the timer's initialized bit was absent, preserving unrelated bits.
- Read **separate** sound-control pointer global `4b8954`; if present and
  `flags60 & 0x200` is clear, require sound **2** at `43d2cc -> 458410`.
- Stop there before any hit-counter reset. Whether that sound is required or not,
  the next instruction boundary for the later suffix is `43d2d1`.

`counterResetPrefix(hitCounter)` proves the later counter writes and next required
call: previous=5/current=6/fractional=6, conditional clock/flag initialization,
then **player inline animation script 0** at `43d32c -> 4593b0`, EAX=player+14,
EDI=player field +10, EBX=0. It cannot be used before a required sound-2 call resolves.
The fixtures reach it through the explicit no-sound suffix branch so that original
`fldz` establishes FPU state; no invented sound return is spliced into the trace.

These functions do not mark the hit call complete. Return-tail work remains after
the real animation call; the circle helper has not yet been given a fabricated 1.

## State-4 deathbomb-window prefix

`windowPrefix(stateF78, stateTimer, heldTimer, WindowControl)` supports only state 4,
proved by original jump table `43c1bc[4] -> 43b843`. Other states are unsupported,
not silently treated as invulnerable or as ready to die. All snapshots are explicit:

- State timer current is actual `+f84`, not hit counter `+438dc`.
- Held timer current is actual `+43914`.
- Window control is global `4b892c`, mode `+3c`, **perfect charge** `4b4d70`,
  and raw held-key word `4db210`. Raw **power** `4b4d6c` is a different value.

Original branch priority and precise writes:

1. Signed f84 >=8: require `43b84e -> 43cdd0` with player in EAX. No death,
   item, power, life, or motivation mutation is fabricated. Continuation `43b853`
   remains conditional on actual completion of that call.
2. Otherwise signed held current >0: run real pure timer setter `403280` with 0,
   so held timer becomes previous=-1/current=0/fractional=0 with conditional init.
   Stop at `43b8b2 -> 458590`, argument 31 in EAX. Next boundary is `43b8b7`;
   original sound operation 70 at `43b8bc` and later checks remain pending.
3. Otherwise require a present control, mode ==0, signed perfect charge >=10000,
   and keys &2. If any gate fails, no call on this prefix, continue at `43b9ee`.
4. With those gates satisfied, run actual `403280` timer setter with 60:
   state timer becomes previous=59/current=60/fractional=60. Stop at
   `43b902 -> 407810`; continuation is `43b907`, **not** automatic state 1.
   Original later calls `427150`, `427300(-10000)`, then write f78=1 at `43b916`
   still require genuine support and resolution.

Even a `NoCall` result covers only this state-4 branch, not a full player frame.
No API advances f84; actual ordinary state-timer progression is a named unresolved
dependency, distinct from the supported hit-counter decrement.

The death-commit callee `43cdd0` itself immediately calls `427260(-10000)`, then
`427300(2500)` before the observed power reset at `43cdf5`. Consequently this
window prefix does not apply the power reset early. Numeric argument meanings,
HP/motivation/life changes, item emission, and respawn are unimplemented here.

## Independent return-tail rules

`tailPrefix(TailState, TailControl)` covers only `43d331 .. 43d373`, after the real
player script-0 call has resolved. Manager global `4b8934` fields are deliberately
named `flags7c`, `counter28`, `value80`, without guessing HP or motivation semantics.

- If flags mask 0x1 is clear, fields remain unchanged and original tail returns.
- If mask 0x1 is set and signed counter28 >=60, apply flags mask `0xffffffb9` and
  set value80=0, then original tail returns.
- Otherwise the original dereferences global `4b892c -> +3c`. Missing control is
  **unsupported**, not a fabricated zero mode. Nonzero mode ORs flags with 0x40;
  zero mode leaves them unchanged. The independent tail returns in either case.

The probe uses an explicit bounded saved-register/return frame to execute the real
RET instructions. `completedIndependentTail=true` is not a claim that the original
function ran from entry; `completedNativeFunction=false` remains on every case.

## Required call packets

Every `CallRequest` carries `kind`, exact original `callSite/callTarget`, a kind-
specific signed `argument`, `playerOffset`, and symbolic `bankSource`.
`serializeCallRequest` returns those six uint32 words in that order. This is a
portable request record, not a native stack layout, allocated handle, or call return.

| Kind value | Request | argument | playerOffset | bankSource |
| --- | --- | --- | --- | --- |
| 0 | First script-83 allocation | 83 | 0 | 1 = BulletManagerAnm8a7f9c |
| 1 | Sound 458410 | 2 | 0 | 0 = None |
| 2 | Player script 0 | 0 | 0x14 | 2 = PlayerField10 |
| 3 | Sound operation 458590 | 31 | 0 | None |
| 4 | Death-commit call 43cdd0 | unused 0 | 0 = player base | None |
| 5 | Deathbomb system 407810 | unused 0 | 0 | None |
| 6 | Normal-init detach 4679b0 | unused 0 | 0x87a8 | None |

A continuation address describes the next original instruction **after** the
pending call has truly resolved. It is never authorization to jump over it, skip
remaining operations, or rerun an already applied scalar prefix on every frame.
Repeated processing needs host-owned transaction/phase tracking, not fake success.

## Fixtures and verification

`local/research/player-hit-fixtures.json` contains **421 original-byte cases**:
24 entry, 57 gates, 48 state-timer reset, 12 counter reset, 180 state-4 window,
74 return tail, 6 normal init, 6 constructor-counter write, 14 ordinary counter step.
All code/constant bytes and their hashes, explicit registers/state, instruction
traces, stops, timer float bits, call packets, and prefix memory writes are recorded.
Unknown calls stop before execution; only real mapped pure helpers return normally.

Fixture SHA-256:
`867a50b52a97e509c10f8d38569da6198862286a8d6990712086498dedf15489`.
Original EXE SHA-256 before/after:
`ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4`.
Report flags `nativeCpuTrace`, `windowsGameProcessLaunched`, `fakeCallReturns`, and
`completedNativeFunction` are false; all cases retain completedNativeFunction=false.

From the assigned `outputs/th128-web` workspace, using existing dependencies:

```powershell
$env:PYTHONPATH = (Resolve-Path '..\..\work\reverse-libs').Path
$env:PYTHONDONTWRITEBYTECODE = '1'
$env:TH128_BULLET_EXE = 'E:\gee\妖精大战争\th128.exe'
python -m unittest discover -s tests -p test_player_hit_probe.py -v
node --test tests/player-hit.test.mjs
```

Regenerate only this task's private evidence:

```powershell
python tools/research/player_hit_probe.py $env:TH128_BULLET_EXE --report local/research/player-hit-fixtures.json
```

C++ tests compile only owned sources with the existing Emscripten 4.0.22 SDK,
C++20, `-O1 -Wall -Wextra -Werror -fno-exceptions -fno-fast-math -ffp-contract=off`,
plus a second build with `-fsanitize=undefined -fno-sanitize-recover=all`.
Products remain under dedicated `work/player-hit-tests`, not shared runtime/site.
No dependency installation, original process, E: output, branch, commit, additional
agent, or frozen-source modification is used.

## Ledger and named uncertainties

- Probe tests were RED for the missing module, then GREEN. Fixture-builder test
  was RED before implementation. Production WASM and UBSan tests were RED before
  the new header/source existed, then GREEN for the initial 395 cases.
- Following the explicit initialization/decrement request, normal-init/counter-step
  assertions were RED, then GREEN. The separate constructor-counter assertion was
  RED before its proof existed. Final fixture set is 421, not a full frame trace.
- Self-review is separate from implementation, but not independent reviewer
  sign-off: extra agents are prohibited. Checked gate precedence, signed branches,
  float stores/truncation, timer initialized-bit preservation, exact request ABI,
  null-control safety, partial-entry/return labels, and previous-file freeze scope.
- Still unknown: complete game initialization and all constructor-to-live-counter
  handoffs; ordinary state-timer scheduling; real controls/charge snapshots at hit;
  Windows locks, effect/ANM allocation/attachment/lookup; post-window system calls;
  HP/lives/motivation/items/respawn semantics. No default or fake result fills them.
- Broad runtime publication/integration suites are left to main, since they modify
  shared files on the active critical path. Verification here stays within the owned
  component and its original-byte evidence.

Final scoped verification (2026-10-29): Python 14/14 passed with the actual pinned
EXE and local Unicorn, no skips; the full 421-case report regenerated identically.
JS 3/3 passed: byte/provenance validation, all 421 cases through normal WASM, and
all 421 again through UBSan WASM. Python AST and JS syntax checks also passed.
Only the eight owned new deliverables are written. Original and fixture SHA-256
remain pinned above; no unresolved self-review finding or deferred minor remains.
The named external uncertainties are deliberate stops, not completed game features.
