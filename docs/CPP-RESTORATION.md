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
| `CustomGeometry.cpp`, `PresentationGeometry.cpp`, `TextGlyphs.cpp` | UFO fill strips, three laser meshes, numeric popups and read-only mesh ABI |
| `BulletPool.cpp`, `HostilePool.cpp` | Original2000-slot reservation, reuse and physical traversal |
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

Original menus, campaign transitions, save/recording integration, remaining
sprite-boundary bullet gates and special background effects still need work.
Full-stage visual/Replay acceptance remains open. Browser font rasterization
and Web Audio output are not original GPU/GDI/audio-thread Oracles. Passing
subsystem samples does not establish whole-game parity.
