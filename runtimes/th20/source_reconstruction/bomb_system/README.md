# Recovered BombInf and character bombs

This directory contains C++ source for the 0x3c-byte scheduler owner BombInf,
the independent 0xb8-byte Bomb polymorphic base, 0xcc-byte BombMarisaAInf and
0x14f8-byte BombReimuAInf with 24 0xd8-byte orbital records. The character names
and sizes come from the verified executable, including its diagnostic strings.
The six Bomb virtual slots are unrelated to CallbackOwner's three slots.

The owner factory, callback registration and teardown, trigger predicate,
update/draw/event dispatch, bomb consumption and Player meter changes are
reconstructed in source. Both character implementations include start,
per-frame behavior, animation attachment, shared timers, damage regions and
retirement. Reimu's orbit, launch and nearest-target tracking use the restored
shared Motion and Enemy search functions. Marisa retains the original order of
beam rotation, steering and three damage-region updates, including the repeated
position-z accumulation in the original reused temporary vector.

The source calls the real recovered ANM, scheduler, sound queue, ScreenInf,
Player state and HitCtrlInf implementations. Four rectangle/circle cancellation
operations still require the actual Bullet/Laser controllers; they are explicit
unresolved declarations in character_environment.hpp. No original machine code
or pretend-success implementation substitutes for those modules. A compiled
static library is not a linked independent game.

## Evidence and validation

- `evidence/*.asm`: selected functions disassembled from the SHA-256-verified
  original. This evidence is not built into the source library.
- `state_cpu_validation.json`: 68,387 checks against the unmodified original,
  including integer overflow and clamping, complete Player/Enemy storage,
  enemy lists and iterator cleanup, and the real 487bb0 secondary-owner state
  transition. That function changes the existing owner's flags and counter;
  it does not clear a projectile list.
- `pool_validation.md`: 31,168 additional Bomb owner/base checks in the shared
  Sprite/ANM CPU oracle. Its fixture normalizes only code/allocation addresses
  and observes virtual calls at the actual boundary.
- `../runtime_state/motion_cpu_validation.json`: 41,984 complete-state checks
  for the shared 72-byte movement record.

Character-specific differential tests are being added separately. The above
counts do not establish complete gameplay equivalence, full trigger execution,
or correctness of currently unresolved collision-controller behavior.
