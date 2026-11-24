# Actual first-segment runtime, beyond first emission

The new live browser mode runs original first-stage ECL, not an invented pattern.
It remains an incomplete port with an explicit initial player context, ordinary
keyboard movement and a separate real-shot diagnostic, no complete Z/charge/
freezing/death sequence, no STD/background and no complete native
global-frame acceptance.

## Joined components

- `BulletManager` owns2000 slots, ascending updates, cyclic births and bounded
  retained sound requests. Age increments **after** a successful body callback.
- `BulletTickRules` provides84 pre-ANM,17 post-zero-return and6 resolved-hit suffix vectors,
  including half-speed birth motion, control-int transition, full motion, grace,
  cooldown and offscreen removal. Its returned unknown calls are not fake returns.
- `BulletCollisionRules` reproduces270 helper/update decisions and141 gate-free
  circle-geometry cases. The runtime continues on a proved miss without inventing
  player gates. A possible hit remains an explicit stop until actual player state
  and damage callbacks are integrated. Unsupported numeric domains also stop.
- `BulletVisuals` executes the actual type8 script43, birth IRQ8, control-int0
  handoff and subsequent native-style explicit animation ticks.
- `GrazeRules` plans238 original-byte scalar/caller/ABI cases. The runtime supplies
  explicit initial grazeCount0/power10000, commits native scalar-prefix writes,
  performs the required original ANM175 effect creation and retains ordered sound
  requests before setting the bullet's already-grazed flag. Already-grazed contacts
  do not create duplicate events. Sound requests are not a complete native audio
  manager or a claim of audible playback.
- `TransientEffects` executes original bullet.anm175, sprite428, authored12-tick
  lifetime. The creation layer request is23; the script itself changes layer to16.

## RNG correction found during review

The native float getter routes by VM flags2 bit2: set selects RNG0x4d2ad8,
clear selects the same game RNG0x4d2ae0 used by ECL. Testing only this selector
does **not** establish what the effect constructor preserves.

Original effect creation sets bit2 before calling bind, but the actual chain
`0x4593b0 -> 0x402ea0 -> original memset` clears it. A new bounded probe starts
with flags2=2, executes those original reset/bind bytes and stops before the real
ANM execution call0x407080. Its resulting flags2=0x80 selects **game RNG**, not
display RNG. Chaining the actual getter consumes two game-RNG16bit steps and zero
display steps. The earlier isolated-display assumption was rejected.

The production sidecar therefore binds a fresh VM and requires an explicit game
RNG callback forwarding to the **same StageWorld Random** as ECL. Missing RNG is
an error. It does not couple RNG by guesswork or silently return zero. An explicit
display seed remains reserved host configuration, but script175 does not consume
that RNG. Review and regressions distinguish native selector proof from the full
constructor-prefix proof; no Windows game process is run.

## Local enemy retirement and bounds

The first original ECL ANIM include belongs to **bank1**, because native manager
slot0 is reserved for bullet ANM. This is `enemy.anm`, not `enemy_ll.anm`.
The first script0 has32x32 constant sprite dimensions. The conservative appearance
analysis accepts only unmasked sprite changes, layer-only68 and backward timed
loops after a real birth-time sprite assignment; variable dimensions/scales,
deferred first sprites and pre-sprite loops are rejected. It does not substitute
hitbox values for display extents.

Bounds are checked **before ECL**, using original X±192/Y0..448, the native seen
flag0x20000 and distinct X/Y exemptions4/8. Regressions prove an offscreen unseen
entity may continue, a seen nonexempt entity cannot execute its next ECL marker,
and the horizontal exemption follows the original branch before considering Y.

Ordinary supported offscreen/finished enemies are unlinked from the local manager,
their owned logical animation handles cleared, and their VM disposed only after
execution returns. Bounded tombstone snapshots remain for diagnostics. Original
destructor prefixes prove head/tail/next/previous unlinking and count adjustment;
the remaining original destructor statically kills owned ANMs and frees helpers
and VM contexts, without item/score creation. This is not damage/death-item logic.
Constructor initial-update completion is not treated as immediate destruction:
the native constructor ignores that return, inserts its guarded node, and a later
eligible manager visit retires it. An empty controller phase is an explicit stage
transition stop, never a fabricated win.

## Presentation and host safeguards

The live browser mode executes one explicit local kernel pass per60Hz step;
global native callback/frame equivalence is still not proven. Native static
priorities constrain the implemented active-path order: enemies21, bullets24,
then primary ANMs30. Enemy roots register at the primary head; graze/hit effect
roots register at the **tail of that same primary list**, not at secondary8.
Original registration prefixes0x466d60 and0x466de0 both update manager+0x8b9704:
the first appends at the tail, the second prepends at the head. A bounded emulation
regression proves both list updates. The earlier function-name-based secondary
assumption was rejected; a first-graze regression caught1tick instead of2.
A newly created enemy/effect receives both
its birth tick and that later primary pass. The original manager registration
inserts at the head; owned enemy roots follow newest-first order for shared RNG.
The real secondary priority8 remains a separate domain, not used for these effect
roots. Activation/global startup branches remain unproved. Presentation access never
ticks VMs. Rendering uses
original textures and native bullet/effect origin(320,16).

JSON float output uses float32 round-trip precision. Nonfinite composed positions,
scale or UVs return valid[] and a dedicated observable presentation error, not nan
tokens or substituted zeros. Sticky world faults freeze effect advancement too.
Rejected reconfiguration cannot change future target/animation ownership; a faulted
world cannot accept new graze globals. Version and action tokens prevent delayed
resource/audio completions from overwriting a newer reset/title/segment operation.

Current focused verification demonstrates300 local passes with37 moving real
ANM-backed bullets; later first-contact/unsupported gameplay remains a named stop.
Check `STATUS.md` and current machine-readable smoke reports for the latest full
suite and observed later boundary, rather than treating this component count as
acceptance of the entire first stage or full port.

The separate shot diagnostic integrates the actual SHT pool/ANM/movement and
retirement, including retained dirty-slot birth state and genuine mode1 corner
helpers. It checks after-ECL enemy contacts using opcode400 full-size extents.
Original after-ECL flag0x400000 clears before flags0x21 gate the query. Existing
ANMs receive level-triggered Hide/Show from402/403; no-owned-root is a genuine
lookup miss/no-op, not an invented animation. A mutated real ECL regression first
exposed hidden enemies still drawing and now verifies actual WASM behavior.
Z dispatch, sound playback, full damage/death and freeze remain incomplete.
