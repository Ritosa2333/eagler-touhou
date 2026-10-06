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
| `Application.cpp` | Native callback priority order |
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
invented behavior. Original menus, save integration, audio consumption, exact
laser mesh/UV rendering and complete-stage visual/Replay acceptance still need
further work. Passing subsystem samples does not establish whole-game parity.
