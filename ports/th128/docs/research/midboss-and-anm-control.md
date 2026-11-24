# Actual MBoss dependencies and ANM111 control

Main thread only, 2026-10-30. These are bounded component/integration results,
not acceptance of a complete boss, stage or game. The original executable stays
read-only; no original game process, OS/API return or allocator stub is used.

## Shared integer words

The actual `MBoss` starts by writing -9949/-9948/-9947, before opcode412.
Integer lvalue resolver41d140 selects manager-global4b8948 +10/+14/+18,
not the executing enemy's inherited words. Integer getter41c950 reads these
same signed words; float getter41d2c0 uses actual FILD int32 conversion.
Fifteen original read/lvalue paths cover zero, positive, negative and both
int32 extremes. The original preallocated constructor tail4152e0 actually calls
47bb30 to zero the cc-byte manager, with explicit CRT scalar-fill capability0.
This is not a complete allocator/manager initialization trace.

`StageWorld` now owns the three words per manager lifetime. Parent/child and
later reads observe shared updates, reset restores zero, and float-lvalue writes
are rejected. The first synthetic expectation incorrectly ignored native angle
normalization and child-before-parent list order; the fixtures were corrected,
not production movement or scheduling. Real missing-reference RED was observed.

## Boss registration and life

Original412 handler41a4bb clears manager7c bit0 on each accepted operation.
Positive slots store the actual owner in manager5c +4*slot, set enemy flag01000000
and retain the index. A negative argument only clears that retained slot when
the prior boss flag is set; it does not reset the stored index or other flags.
The portable manager uses owner identities, not native memory addresses. Its
eight-slot budget ends before manager7c; invalid indices fail without mutation.

Original411 handler41a472 writes the same signed-life bits to three consecutive
words at enemy-data3e3c/3e40/3e44 and sevenfold uint32-wrapped life to3e48.
The two remaining life words stay intact. An already registered boss also gains
flag80000000; unrelated flags remain. Thirty412 and twenty-four411 complete
original handler paths match ordinary and UBSan portable execution. Actual
`MBoss` reaches412, but its later411 is still behind unsupported animation work.

An explicit regression forbids a registered boss from taking the ordinary
enemy death/drop/retirement path. Missing spell-mode state still fails at its
own earlier boundary. No boss phase logic is fabricated by this guard.

## Actual259 secondary requests

The real resource requests bank0, slot1/script99, then slot2/script189.
Four original positive/nonprimary259 prefixes execute real argument and empty
handle-retirement helpers, then stop at original41f2a0 before ANM allocation.
These prefixes prove the requested bank/slot/layer/script, not a native allocator
return or complete animation creation. Secondary positive259 is connected to
the existing real ANM host. Primary259 and negative-script retirement remain
explicitly unsupported. Bank0 is genuinely `bullet.anm`, not an enemy reskin.

##111 and its consumer

The handler45a4a1 replaces only flags2 bitsc0 with `(uint32(value)<<6)&c0`.
Eighteen bounded original handler paths and sixty original gate prefixes verify
the write and consumer459deb..459e1a. A pending signed-word IRQ bypasses this
gate. Otherwise only mode40 reads control-global4b8954: a present object with
flags60 bit1 set takes the common early-return epilogue45d0ba. Other modes,
absent object, or cleared bit1 proceed normally.

Portable `Runtime::updateControl` is an explicit optional snapshot. Missing
required state faults before clocks, PC or interpolation advance; it is not
silently treated as absent. A known paused tick preserves all those values.
The world supplies the same explicit control presence/flags already retained
by its player context, not a hard-coded pause result. Actual authored99 runs
ninety ticks, holds its real interpolation for twenty paused ticks, then resumes.
This does not implement player freeze, Perfect Freeze use, global initialization
or the entire native scheduler.113/114 remain unsupported. The inherited86/87/89
flag-parity observation remains open and is not silently rewritten here.

## Current real boundary

Normal/seed34772 reaches frame3170 with eight deaths/eight respawns. Original412
has committed a boss and actual259/script99 produces bank0 sprite454. The next
real189 command, opcode88 at byte24, requires enemy child scheduling and stops:

`enemy ANM entity=75 slot=2 bank=0 script=189 opcode=88: enemy ANM child scheduling is unproved`

There is no invented replacement animation, skipped child or completed boss.
Actual189 creates186/187/188 on its authored timeline; these have their own RNG,
scale/color/angle tracks,111 mode, lifetime and IRQ behavior. Next work must
retain original global root/child ordering, owner/slot replacement and retirement,
geometry ancestry, shared RNG order and sticky callback failures. Existing Scene
evidence can be reused only after proving its integration into the enemy host.

## Evidence files

- `local/research/enemy-variable-fixtures.json`
- `local/research/enemy-boss-fixtures.json`
- `local/research/enemy-animation259-fixtures.json`
- `local/research/anm-update-gate-fixtures.json`
- `artifacts/tests/respawn-world.json`
- `docs/main-thread-verification.json` records the latest completed whole-suite
  checkpoint, not inferred from focused tests or this prose.
