# C++ runtime

The playable core is compiled from named C++20 translation units. The automatic
decompiler output is evidence and is never linked into either the native library
or the WebAssembly runtime. It is not the original author's C++ source.

## Source layout

| Source | Responsibility |
| --- | --- |
| `src/game/GameTypes.hpp`, `GameState.cpp`, `GameSession.cpp` | Typed entities, resource ownership and session initialization |
| `EclProgram.cpp`, `EclInterpreter.cpp`, `EnemyManager.cpp` | ECL resource loading, typed stack/variables, enemy creation and update |
| `PlayerSystem.cpp`, `BulletManager.cpp` | Six player loadouts, damage sources, Bombs and hostile projectiles |
| `Items.cpp`, `Ufo.cpp`, `ItemWorld.cpp`, `ItemPresentation.cpp` | Native item pools, inventory, UFO lifecycle, rewards and logical animation ownership |
| `AnmLogic.cpp`, `AnmScene.cpp`, `ScreenAnimations.cpp` | Resource registry, ANM execution, intrusive child links and scene scheduling |
| `Laser.cpp`, `LaserCollision.cpp`, `LaserWorld.cpp` | Moving, timed and curved laser classes, native collision query and game bridge |
| `Stage.cpp`, `SpellController.cpp`, `Dialogue.cpp` | Background, spell and message update bridges |
| `BossHud.cpp`, `EnemyLife.hpp` | Original Boss HUD fill/thresholds/stars and cumulative sevenfold spell life |
| `Application.cpp` | Native callback priority order |
| `CameraEffects.cpp` | Original priority14 shake lifecycle, shared game RNG and screen offsets |
| `CustomGeometry.cpp`, `PresentationGeometry.cpp`, `TextGlyphs.cpp` | Spell rings, UFO fill strips, three laser meshes, numeric popups and read-only mesh ABI |
| `BulletPool.cpp`, `HostilePool.cpp`, `FriendlyPool.cpp` | Original2000-hostile/256-friendly slot reservation, reuse and physical traversal |
| `UfoEffects.cpp` | Summon/defeat trails, native RNG ordering and custom draw geometry |
| `SoundQueue.cpp` | Original indexed sound grouping, integer pan averaging and read-only audio events |
| `Presentation.cpp`, `LaserDraw.cpp`, `Snapshot.cpp`, `LaserSnapshot.cpp` | Read-only draw records and semantic Oracle inspection |
| `src/wasm/Exports.cpp` | Stable browser-facing C linkage; implementations and types remain C++ |

Legacy headers forward to these public interfaces. `src/core.cpp` is a
compatibility entry point, not a concatenation of implementation files.
`scripts/runtime-sources.mjs` determines the actual compiled source list and
includes every C++ source/header in the build identity. `CMakeLists.txt` also
provides a static library target for native consumers.

## Behavioral evidence

Tests use original-executable samples for ANM initialization, child linking,
interrupt propagation, item attraction/pickup, UFO inventory rotation, deferred
death and reward dispatch, all three laser lifecycles and rotated laser collision.
Fixture inputs are explicit and independent of the candidate implementation.
Full-game acceptance must use original execution rather than snapshots produced
by the browser candidate.

## Remaining scope

This is a reconstructed portable runtime, not recovery of the author's class
names, project files or all original source. Unsupported ECL special callbacks
and unverified laser extension combinations are reported instead of receiving
invented behavior. The preview loads all six stages and Extra, including their
STD, ANM, SHT and MSG inputs. Native samples cover UFO/laser vertices, special
enemy callbacks, Hermite movement, hostile pool allocation and sound queues.

StageInfo road/Boss music slots are selected by native MSG19 and retained in
logical state, independently of muting. Boss health uses original412050 life
bookkeeping; its 348-hit fixture retains rounding remainders and phase floors.
HUD41e439/41f33d samples verify fill, names, remaining spell stars and262 solid
rectangle arguments. These samples do not execute original audio output or GPU.

Spell titles now replace the virtual `text` sprite7 with a dynamic CP932 title.
Bonus/History and UFO counts, percentage, timer, defeat multiplier and score use
original ASCII glyphs, formatting, alpha and callback priorities. Independent
samples cover12 spell text cases,252 animation poses,46 UFO text cases and630
ASCII glyph submissions. Browser GDI font rasterization remains unverified.

The128-byte pose ABI is accompanied by16-byte read-only scheduling records.
Embedded Bullet31/Item27/Player24/HUD44 callbacks bypass ANM layer ordering;
registered primary/secondary chains retain their native ordering. Laser29 and
Font45/69 draws interleave with those chains. UFO immediate text uses22/60.
The renderer selects full, playfield or stage camera clipping explicitly, and
places the frame at native layer21/23 rather than painting over every object.
Compact spell/UFO getters and schedule capture do not advance clocks or RNG.

Spell-start scripts125/126 use ANM mode9's closed textured rings, not ordinary
scaled sprite quads. Original455630 emits count-1 polar vertex pairs and closes
with copies of the first pair, retaining the final repeated V coordinate.
Sixteen independent retail samples compare864 vertices, including the actual
scripts' expanding rings, negative radius, repeated UV, secondary color and
immediate parent translation. Meshes retain their registered ANM draw order.
Enemy animation defaults come from413230/415b70: layer offset1 plus7, overridden
by ECL452 or subsequent ANM commands. Actual factory registration uses Primary
head insertion. Laser external scale remains intact when its ANM scale
interpolator duration is zero, as in458066.

The browser preview supports holding either Ctrl key for4x replay speed.
It consumes the original held/pressed samples in order and still executes
every60Hz game tick. Releasing Ctrl, pausing, losing focus or ending playback
clears acceleration; ordinary manual play retains its normal tick rate.

Replay stage restoration is atomic: native fixed position, signed resource
counts, fragments, stock, rank and replay globals are installed before the
synchronous main-script birth. Input0 clears the Enemy creation guard; it does
not perform an extra main update. Friendly damage queries traverse their256
physical slots, including the native ordering when a hit creates new fragments.
Hostile birth velocity uses the raw emitter angle while the stored angle is
normalized separately. Draw headings use the original ANM opcode82 flag and
40a150's angle+pi/2 rule, independently of ETEX blend control.
Emitter radius placement separately uses that normalized heading, with polar
offsets stored as float before adding the origin, as in40a671..40a69e. The63-case
native CPU fixture distinguishes angle wrapping and those storage boundaries.
ECL312/313 random movement retains the extra RNG16 direction selection, quarter
clamp bounds and near-vertical angular guards;132 native cases cover both
instructions and their1056 subsequent movement frames.

Continuous damage sources also perform the original Source16 coordinate
flooring, even at zero velocity. Circle queries retain coordinate differences
at double precision until storing the squared distance as float. Enemy hurt
queries process friendly slots, Bomb and continuous sources in that order,
before applying the80-point cap. UFO defeat reads the current Enemy18 pose and
binds the ten original trail effects after bullet208; their simulation and RNG
continue in Primary27, independently of rendering.
Circle bullet queries likewise retain unrounded deltas until their distance
store, covered by72 actual437980 cases for both round bullet types. Radius
cancellation converts in physical-slot order using the original2px item window.
ECL510 uses its separate40cf60 viewport/hitbox selection, immediate item9
conversion, script96 effect and next-Bullet21 retirement rather than ordinary
cancel IRQ/lifetime behavior; active spells96..99 retain their skip gate.

ECL445 has a distinct phase-end laser clear: it resets protection/offscreen
grace, includes retiring beams and converts cut tails in original list order.
ECL512/513 retains its protection gate. Seven actual ECL fixtures verify point
order, cancellation effects and RNG, including28 points and336 RNG16 calls for
the midboss-style circle-then445 sequence. Laser full clearing accumulates a
float16px step, preserving horizontal beam coordinates rather than repeatedly
recomputing trigonometry at the cumulative distance.

All six Bomb loadouts retain independent front1 background handles and their
actual camera requests. Marisa A and Sanae B retain the real main ANM handle;
a missing main stops the Bomb before the next source or leaf birth, and
synchronization never recreates a retired VM. Camera profiles1 and8
executes at priority14, before Player16/Bomb17, and consumes actual RNG32 choices
for its horizontal/vertical shake. The preceding frame's draw42 reset is a
declared CPU seam, keeping offsets clear when the camera effect has retired.
Every cancellation region processes its bullets and lasers in original order.
Independent complete two-Bomb samples cover4260 manager/ANM ticks across all
six loadouts, including main/background VM clocks/alpha, camera lifecycle and
RNG. Reopening the same core also retains the original interrupt semantics.
Screen translation is applied only to the actual textured quad paths before
draw42. Native mode9/13 and curve-laser preformed strips, solid shapes and UFO
line trails remain unshifted; six actual draw samples verify that distinction.
Ordinary mode0 retains floor-before-offset-before-nearest-even vertex rounding,
while mode1 does not round even at zero rotation.108 actual CPU poses and four
primary-alpha guards cover the native quad submission path.

Normal Bomb use preserves the player's state timer. Only an eligible deathbomb
window resets that timer to60; at age8 the native miss path runs first. Sixteen
original Player16 cases verify those distinct branches. ECL419 observes MSG12's
one-frame pulse, allowing the boss script to continue before dialogue ends;
this restores the actual6538 Boss birth without shifting replay input.
Moving/curve80000 consumes its paired100000 emitter payload and dispatches
child bullets at the laser head. Timed lasers ignore those commands. Nine
retail CPU samples check the emitter ABI and optional parent cancellation;
the child emitter boundary is explicit and does not establish replay parity.

Original menus, campaign transitions, save/recording integration and special
background effects still need work.
Full-stage visual/Replay acceptance remains open. Browser font rasterization
and Web Audio output are not original GPU/GDI/audio-thread Oracles. Passing
subsystem samples does not establish whole-game parity.
