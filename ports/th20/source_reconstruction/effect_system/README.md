EffectInf is recovered as C++ with its actual 0x13044-byte owner, 1024 animation
handles, 1024 0x48-byte deferred requests, asset bindings and scheduler lifetime.
Production code does not map or call original machine code.

`effect.cpp` implements parameters47ba30, request49ce30, context49e010,
spawn49db70/49dcf0, enqueue49dc60, update49d4a0, clear49d400 and recursive
enable44fa70. `lifecycle.cpp` implements construction49ce80,
initialization49d790, loading49da70, destruction49d2f0 and ownership factories.
`environment.cpp` connects actual source resource/session owners and ANM's
spawn-effect operation. Controller draw49dea0 calls the original literal-return
behavior: the effect callbacks perform their own rendering.

The descriptor table5afab8 has 15 valid records and an eight-byte terminator.
Implemented callbacks are:

| Indices | C++ type | Original functions |
| --- | --- | --- |
| 13 | RoundedPanel | 461000, 461310, 4610d0, 461140, 4613d0 |
| 4 | ShortLine20 | 465df0, 466140, 465e90, 4660a0 |
| 10 | ShortLine30 | 4663e0, 466730, 466480, 466690 |
| 2, 3 | LongLine | 45ded0, 45e3a0, 45e240, 45df80, 45e190 |
| 5, 6 | SpiralTrail, ReverseSpiralTrail | 460180, 460620, 460240, 460570, 460910, 460b60, 4609e0 |
| 7, 8 | TripleRing, FilledRing | 45e6d0, 45eb10, 45e770, 45ea20, 45d5b0, 45db30, 45d6f0, 45da40 |
| 9 | BurstRings | 45f340, 45fb60, 45f5e0, 45f9b0 |
| 11 | RadialTrails | 466990, 466c70, 466a90, 466ae0 |
| 14 | WaveringTrail | 4672e0, 467990, 467430, 467820 |
| 1 | ConvergingParticles | 45c1f0, 45cfe0, 45c360, 45ceb0, 45d060 |
| 0 | TransitionPanels | 4646e0, 4657d0, 464840, 4648e0, 4659a0 |
| 12 | StoneSelection | 461730, 462c80, 4618c0, 461b80, 462c10, 463310 |

Polyline43d1a0 is actual Direct3D submission in `short_line.cpp`. The common
short/long-line motion uses the production shared RNG stream, including its
exact floating-point operation order. LongLine preserves the original alpha-byte
wrap for its last 16 initialized samples. Its orange variant installs the actual
64-frame alpha interpolator.

All 15 descriptor initializers now have production C++ implementations. The
retained `unrecovered` namespace is a historical name; it no longer means those
initializers lack definitions. This does not establish behavioral equivalence
for every type or complete game integration.

`spiral_validation.json` and `additional_validation.json` record CPU comparisons
for newly restored effects, including RNG, complete VM state, timer boundaries,
polyline/fan/strip vertices and individual COM submissions. Each report applies
only to its recorded source hashes and listed cases. The latest additional group
contains 357,860 checks, including types 0, 1, 12 and 14. The shared report has
485,796 effect checks including the earlier owner and line groups.
Type 12 uses actual GameController scheduler flags, WeaponStoneInf queries,
Player selection fields, SaveManager and StoneMenu file storage. Those external
dependencies remain separately scoped in tests. StoneSelection uses six named
query boundaries in its core test: the original reads real fixture storage, and
four Weapon virtual queries return configured booleans. Its production query
adapter is not covered by those checks. All animation spawning, interruption,
state updates and primitive rendering run actual recovered source.

TransitionPanels checks its five-ANM construction and empty-owned destruction,
initialization, closing/finished-frame paths and every mask/ANM draw submission.
ConvergingParticles checks actual script-149/150 spawning and both Hermite
phases, with complete animation-pool, RNG and file-counter state. WaveringTrail
checks screen scaling, zero extents, lifetime boundaries and its player0 gate.

`pool_cpu_cases.inc` and `short_line_cpu_cases.inc` execute unmodified original
instructions only in the test executable and compare full callback/Animation
storage, returns, actual shared RNG state, rendered vertices and Direct3D COM
traces. The aggregate report `../sprite_renderer/pool_cpu_validation.json` binds
production and fixture hashes and exposes individual `effect_*` counts. The
owner's queue/update/clear are tested, while its full resource-loading and
scheduler lifetime integration has only been compiled. Deferred update tests
use negative entries or the real terminator; factory allocation/spawn chains are
not yet compared end to end.

Original request-clear copies three uninitialized parameter-padding bytes to
each record. Only those three bytes per request are normalized in that test;
the C++ implementation explicitly chooses zero padding. Negative line-sample
indices are outside the original valid lifetime (out-of-bounds write), and the
source throws for that invalid state. This is not evidence of complete gameplay
or complete source recovery.
