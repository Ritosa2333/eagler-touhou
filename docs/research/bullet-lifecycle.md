# TH128 original-byte bullet lifecycle prefixes

This continues the existing first-stage plan at opcode 501. It does **not**
complete 501, emit a bullet, run a complete bullet tick, or make the stage
playable. The configuration-only world baseline stops before the original manager call.

Later integration in `first-emission.md` consumes these bounded rules in an
opt-in real bullet/ANM constructor. This document records the component proof
boundary, not an assertion that the later integration is still absent.

## Evidence and executable identity

`tools/research/bullet_lifecycle_probe.py` reuses the bounded emulator from
`bullet_probe.py`. Every executable slice and constant is copied read-only
from the pinned Japanese executable with SHA256
`ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4`.
The private report is `local/research/bullet-lifecycle-fixtures.json`, schema
`th128-original-byte-bullet-lifecycle-fixtures/1`, **35 cases**, SHA256
`959afe7bf10aef9e0f46d348340d92a9d52dba3e7d13dc3efff131b7461ac997`.
Input/output EXE hashes agree. No original Windows process or API is launched.

This is Unicorn original-instruction emulation, **not** native Windows CPU
execution or a recorded complete-game trace. Code/data/stack budgets, explicit
state spans, instruction-boundary checking, 2048-instruction case budgets and
the 500000-microsecond timeout remain enforced. Unknown calls stop before the
call; no return registers, geometry or API results are invented.

## Actual manager aim prefix: 12 cases

The entry is the real emission manager at `0x40a8f0`. It calls the actual aim
helper `0x43c3a0`, reads the supplied player pointer at `0x4b8a80`, and stores
the result at `0x40a90c`. The probe stops at **`0x40a910`**, before any count
loop or call to bullet construction `0x4091f0`.

The two coordinate differences are separately rounded to float32 before the
original CRT path runs. That path includes the original `fpatan` instruction
at `0x4928ab`, sign/quadrant handling, x87 control-word selection and restore.
The declared CRT dispatch and exception flags are both zero. These are
explicit fixture inputs, not a reconstructed runtime initialization sequence.
Coincident positions return rounded float32 pi/2, not zero or an arbitrary
retained angle. Tests cover cardinals, quadrants, coincidence, a birth-position
anchor and float32 subtraction-rounding examples.

`bullet::targetAnglePrefix` matches all saved float bits in WASM. It is not a
claim that portable libm matches every possible x87 transcendental input.

## Post-animation spawn tail: 11 cases

The probe enters **`0x4097d0`**, after the original two animation initialization
calls, with explicit post-animation bullet state and a null interrupt callback.
It stops at **`0x4099df`**, before the real ETEX dispatcher `0x409a90`, animation
ticks, pool cursor update and constructor return. A non-null unknown callback
is separately tested to stop rather than return a fake result.

Only original type 8, color 0..15 and extension slot 0 are accepted in this
bounded tail. The type 8 profile is read from `0x4b2dd0`, stride `0x120`:
the stores at bullet `+0x9f8/+0x9fc/+0xa00` are **2.4/2.4/16**, and the raw
profile value stored at `+0xa40` is **4**. The first two values are full
hitbox dimensions in the statically decoded rectangular helper `0x43c410`, which multiplies each
by 0.5; **2.4 must not be described as the radius**. The third field's complete
gameplay role is not proven here and is named `auxiliarySize` in portable code.

For extension type 2, the native tail:

- Requests ANM interrupt `(parameterA + 7) mod 65536`.
- Sets motion state 2, subtracts **4 times each velocity component** from the
  stored position, and advances the first-extension index to 1.
- Copies the emitter extension data, retains emitter flags/transform sound,
  and clears the active-extension bitmask before later dispatch.

`parameterA` is **not a guessed delay counter**. The actual first configuration
has parameterA 1, thus interrupt 8. Eight signed/wrapping argument cases and a
non-type-2 case check these stores. Ordinary non-type-2 tails request interrupt
2 without consuming slot 0. Neither branch claims to have dispatched ETEX.

`bullet::spawnTailPrefix` requires supplied profile values and post-animation
flags; it does not pretend to initialize the actual ANM object. Its output
models only the supported tail fields, not the full original bullet structure.
The type8 tail reasserts flag `0x10`, including when the supplied earlier flags
are zero or 3. Two additional cases cover this store and distinct R/S/B/control
extension values; the C++ harness now compares all six extension fields and
the retained velocity, as well as the other modeled spawn outputs.

## Pre-collision movement: 12 cases

For state 2, original span **`0x408727..0x40878d`** rounds velocity times the
explicit timestep to float32, multiplies each stored product by 0.5, rounds
again, then adds position. For state 1, **`0x4089e2..0x408a2a`** omits the
half-speed multiply. The timestep is a fixture input from `0x4b6f58`.
These spans stop **before** collision/graze calls, animation gates, age/timers,
out-of-bounds handling, freeze logic and manager traversal. They cannot be
scheduled as a complete tick or used as evidence for full global frame order.

The portable `motionPrefix` preserves the intermediate float32 stores.
Two discriminating cases prevent collapsing those stores: state1 with
`position=-.07f`, `velocity=.1f`, `timeScale=.7f` returns zero, while state2
with zero position, smallest positive float velocity and timestep2.9 returns
float bits2 rather than1. Extreme finite products are checked against the
finite float maximum **before** narrowing to float; both signs/states are
covered by guards in the C++ test harness.
All three APIs reject nonfinite values and enforce an explicit bounded domain;
unsupported motion states, types or colors are rejected rather than guessed.

## Verification and next boundary

The regression was first observed failing for the absent probe/APIs, then
implemented. Thirteen targeted Python tests pass with the original EXE and
Unicorn enabled. The C++ fixtures compare all **35** saved vector outputs
bit-for-bit in ordinary WASM and UBSan builds. A vector-guard regression also
rejects arbitrarily large Python integers before any float conversion.
Independent review found no Critical/Important issues in this bounded component
and suggested the store-sensitive vectors, retained-field comparisons, flag
reassertion and pre-narrowing product guard. These were added with regression
coverage; the absent native/portable flag inputs first failed before correction.

Still required for 501: actual pool allocation/return semantics, ANM object
initialization and callbacks, the ETEX dispatch following this tail, manager
side effects and exact scheduling. Active bullets additionally need collision,
damage, graze, freezing, lifetime and animation integration. These APIs are
deliberately not installed as a fake successful 501 handler.
