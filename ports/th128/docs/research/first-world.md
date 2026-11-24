# TH128 first-stage world kernel: current integration boundary

This is a partial engine integration, not a playable first stage. It implements
the next work unit from `docs/2026-10-29-first-stage-plan.md` without importing
TH12 enemy, player, stage or scoring state.

Continuation: `first-emission.md` describes a separate opt-in mode that now
integrates bounded local motion and actual type8 bullet/ANM construction beyond
501. The configuration-only baseline below intentionally remains available;
its501stop/unchanged coordinates are not the new emission mode's current state.

## Linked scripts and typed game references

`Program::linkIncludes(providers,error)` resolves the actual ECLI dependency
closure transactionally, preserves pre-link VM snapshots, retains source bytes,
and rejects missing dependencies, cycles, ambiguous subroutines and resource,
instruction, subroutine and longest-path depth overruns. The actual 16 stage
entry files all link successfully. First-stage A1-1 links exactly three files:
`st_a1_1.ecl`, `default.ecl`, `st_a1_1b.ecl`.

The added `Vm::setReferenceResolver(read,write,error)` provides explicit typed
negative-reference reads/writes. Unhandled values, nonfinite floats, exceptions
and reentrant VM mutations are errors, not silent zeros. A review found that
deduplicated DFS nodes could bypass the depth budget depending on include order;
subtree-depth memoization and 64/65-edge order-independent regressions fix this.

`tools/research/ecl_globals_probe.py` samples the actual float getter at
`0x41d2c0`, including its dispatch table at `0x41daa8`. Twenty-seven bounded
original-byte cases validate selected entity position fields and random-value
paths. The signed random angle uses a rounded float32 signed unit and the
original widened float32 pi constant **3.1415927410125732**; using mathematical
pi changes recorded results by one ULP for some seeds. This is instruction
emulation, not native Windows CPU or live game-state verification.

## World execution reached

`StageWorld` owns linked per-entity VMs, root/child state records, bounded birth
depth and entity/instruction budgets, actual callback argument decoding, and
explicit sticky diagnostics. It currently accepts the independently evidenced
flag/bounds, animation-request, spawn-request, polar-setup and initial emitter
configuration operations. Debug registration opcode 22 is recorded but never
invented as an active retail debug callback system.

Using actual first-stage Normal data and original demo0 seed **34772**:

- The kernel's root advances beyond the former opcode 402 stop.
- At kernel tick **100**, the first actual script birth is `GirlA1b` at
  `(-128,-32)` with life 20, score 1000 and drop 2.
- Bank 1 / slot 0 / script 0 is recorded from actual opcode 262 arguments;
  this is an animation request, not completed native ANM object lifecycle.
- The first emitter is type 8, color 10, count 1, layers 1 and speed 1 on
  Normal. Opcode 435's rank-dependent 3/3/5/7 value is a later loop count;
  the first volley reads a different local. It is not a 3-bullet first volley.
- All six first-extension field stores in opcode 509 now use the verified raw
  setter. This stores parameters only; it does not execute the extension.
- After the authored wait, the explicit boundary is the first opcode **501**,
  offset **816**, before the unimplemented bullet manager. No bullet is
  fabricated or emitted, and no incomplete callback is silently skipped.

These are portable-kernel observations from real scripts, not a claim that
native whole-frame scheduling, construction defaults or all enemy state has
been reproduced. Native helper evidence and integration policy remain separate.

## Independent player and enemy components

`PlayerRules` is compiled into WASM through explicit raw input/context exports.
The motion tests cover actual unit-scale speeds, contradictory keys, focus gates,
inclusive bounds and wrap behavior. The world does not substitute an always-on
manager or guessed freeze hold state; player scheduling and shooting/collision
integration remain unfinished. Actual `pl00a.sht` is decoded as 16 groups and
114 raw records, including unknown callback IDs, not replacement formations.

Enemy helpers verify flag/bounds operations, polar setup/interpolation and a
bounded simple XY helper. TH128's sampled enemy coordinate quantum is **1/100**,
not TH12's 1/128. This helper alone does not establish frame order relative to
ECL or other callbacks. The world snapshot keeps spawn coordinates with
`positionIntegrated: false` while advancing only ECL control time. It stops at
the first emission call before claiming continuous enemy motion or guessing
frame order. The native ordinary per-enemy path statically places motion before
ECL, but the full constructor/new-node/frame schedule remains unproved.

Spawn mode 1 and unimplemented constructor paths stop explicitly rather than
being treated as normal mode 0. Unsupported globals, ETEX execution, emitted
bullet lifetime, hit/graze/damage, freeze propagation and full stage flow remain
real work; no full-game completion is claimed.

## Running the integration checks

```powershell
node --test --test-name-pattern="include linkage" tests/ecl-runtime.test.mjs
node --test tests/stage-world.test.mjs
node tools/build-runtime.mjs
node --test tests/world-runtime-integration.test.mjs
```

The local browser animation page has a separate world-kernel diagnostic section
that reads only the three actual first-stage files, reports the first enemy and
stop reason, and remains labelled **not a playable stage**. It does not replace
the original title/ANM checks or the earlier raw ECL diagnostic.

## Independent review and regression fixes

Invalid difficulty/missing dependency/main validation now occurs before replacing
an active world. Rejected starts report an operation error without clearing the
retained frame/entities/linked sources; the previous world may continue. Full
resource replacement still requires explicit reset.

Opcode 305 preserves the native accessor sequence: float target angle, float
target speed, duration; nonpositive duration cancels without reading mode. The
positive branch resolves mode, then resolves duration a second time. A stateful
second duration that switches to nonpositive is explicitly unsupported rather
than silently assigned different setup semantics. Regressions cover a masked
empty-stack mode on the zero-duration branch and two RNG-consuming duration reads.

The shot reader now shares immutable terminator tails between aliased groups.
A 256-alias fixture verifies one retained bytes object rather than duplicating a
large tail for every alias. This is a memory-safety fix, not a formation change.
