# Central GameController source

This module restores the central game object at original global `0x005ba828`: its 272-byte layout, constructor, creation/destruction control flow, loading-thread dispatch, suppression/freeze getters and restart mode. It now also restores the complete observed branch/order sequence of loader `4bad40`, player initialization helpers, and all eight original 308-byte stage definitions. It does not define the separate `GameSession` at `0x005ba568`; the adapter uses the actual `game_session` module.

Production is C++20, compiled as x86 with static CRT. It does not execute, map, embed or launch the original image. Missing entity and stage subsystems remain explicit required declarations in `subsystems.hpp`; a static library building successfully is not a complete game executable.

## Original evidence

All addresses use image base `0x400000`. Evidence is the corresponding file under `analysis/ghidra/pseudocode` and the actual instructions in `analysis/binary/disassembly.asm`, from SHA-256 `a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897`.

| Original address | Restored source | Evidence/verification |
|---|---|---|
| `4b9b10`, `4b9df0` | Allocation plus zeroed factory construction | Original CPU constructor vs all 272 original bytes; vtable address normalized |
| `41fd20`, `422d90`, `4b9c10` | Shared 16-byte `CallbackOwner`, two zero timers, actual configuration constructor | Included in full constructor comparison; prior runtime/configuration evidence |
| `424030` | `update_suppressed`: bit 0 OR bit 2 of `+e8` | 20,000 original CPU cases |
| `424060` | `animation_frozen`: bit 1 of `+e8` | 20,000 original CPU cases |
| `488830` | Restart mode at `+108` | 20,000 original CPU cases |
| `4bd920` | Clear only bit 6 of `+e8` | 20,000 complete-object comparisons |
| `450880` | Primary sprite-update suppression predicate | Actual original paused branch plus predicate composition; the normal `4497d0` sprite path belongs to the renderer |
| `4bec70` | Create object, reset input latch, evict managed D3D resources, publish global, set restart and loading bit, disable existing owner callbacks, launch loader | Source lifecycle test with recorded external boundaries and real `std::jthread` |
| `4b99f0`, `40b1d0`, `4bcca0` | Shared worker at graphics `+d90`, lock slot 6, detach previous worker, clear close flag, launch actual thread, read global controller at thread start | Source thread-dispatch/order test; invokes recovered loading control flow |
| `4bd3d0`, `4b9ef0`, `41f7c0`, `41fdf0` | Reset latch, virtual destruction and allocation-lock free; full destructor branch/control sequence | 130 source lifecycle cases with recorded required subsystem boundaries; not a whole-original-destructor CPU claim |

Layout assertions enforce base callbacks at `+0/+4/+8/+c`, timers at `+10/+20`, loading fields at `+30/+34`, 176-byte configuration at `+38`, game flags at `+e8`, fields `+ec/+f0/+f4`, doubles at `+f8/+100`, restart mode at `+108`, and final word at `+10c`. The source `Services*` follows at `+110` and is outside the original region.

The destructor preserves ordering and repeated reads of mutable scene/session state. It clears game bit 6, invokes optional progress persistence, resets the clock and surface callbacks, applies transition/continue state, performs the appropriate full or retained-session cleanup, removes its two scheduler callbacks, clears the global controller, conditionally stops music, stops effects, sets the final clear color and retires the final owner. Transition constants are the original floats 480 and 392 at `0x56cda8/0x570388`. Tests include a transition callback changing the scene from 10 to 22, verifying the subsequent scene re-read and flag updates.

## Integration and unfinished dependencies

`entry_adapter.cpp` provides:

- `platform_window::unrecovered::{create_game,destroy_game,game_restart_mode}`.
- `sprite::anm_environment::gameplay_frozen`: nullable current controller and getter `424060`.
- `sprite::controller_environment::suppress_primary_update`: nullable controller and both `424030` and `424060`.
- Actual `game_session` flag, mode, overlay-owner, primary-owner and continue-count access; actual graphics/window fields, shared worker, shared scheduler, runtime clock and audio cleanup.

The following remain real required dependencies, with no default values or empty implementations:

1. `loading_dependencies.hpp` lists the actual missing entities consumed by `4bad40`: save-manager profile selection/read/commit, HUD refresh, ten entity factories, preserved-owner reset, effect readiness and final entity initialization. `loading.cpp` performs the state writes, control flow, profile-offset reads/writes and sequencing; `loading_adapter.cpp` binds source session, renderer, audio, scheduler, worker, clock, Background and EnemyController. The twelve original factory positions include the now recovered Background and EnemyController factories. Unknown factories have no production definitions or fabricated success results.
2. `commit_progress_if_present()`, nullable `0x5c6108` then `50f660`. `screen_transition(float,float)` now binds the recovered TextRenderer `4a0aa0`.
3. Remaining owner aliases include `0x5c6120`, `0x5c60fc`, `0x5c60bc`, `0x5c06a4`, `0x5c6114`. Background `0x5c069c/0x5c06a0` uses the actual `background::primary/secondary` objects. Services returns owner pointers by value so it does not alias a derived pointer through a base-pointer reference.
4. Owning cleanup routines `4feff0`, `485340`, `4c47a0`, `4d5be0`, `510830`, `513cc0`, `4b64a0`, `4bc220`, `49d400`, `477fd0`, `488690`, `532f40`. Enemy cleanup `4aa940/4a80f0` is now source in this module.
Stage-selection comparison and restore (`474d80`, `4bd5c0`, `4dd8d0`) are now supplied by this module, backed by the actual session and selected-stage pointer.

`Services` requires all these boundaries. The test implementation records external effects and executes a real worker; it is linked only into test executables. It cannot make a production link appear complete.

## Verification

`cpu_validation.json`: **107,522 comparisons passed**, with SHA-256 binding for the reconstructed object header and implementation. These verify the constructor, exact getters, bit clearing, nullable predicate composition and the original sprite wrapper's paused branch. They do not assert that all lifecycle-dependent subsystems are restored.

`th20_gameplay_lifecycle_tests`: **130 cases passed**, testing factory publication and restart mode, actual thread dispatch, destructor side-effect order, all selected scene/session cleanup branches, continuation flag changes, nullable controller state, music predicates, final color and a mutable-scene callback. Those use recorded subsystem boundaries, not original CPU execution of the full destructor.

```powershell
& D:\cmake\bin\cmake.exe -S source_reconstruction/gameplay -B source_reconstruction/gameplay/build -G 'Visual Studio 16 2019' -A Win32
& D:\cmake\bin\cmake.exe --build source_reconstruction/gameplay/build --config Release
& source_reconstruction/gameplay/build/Release/th20_gameplay_lifecycle_tests.exe
& source_reconstruction/gameplay/build/Release/th20_gameplay_cpu_compare.exe '<verified th20.exe path>' source_reconstruction/gameplay/cpu_validation.json
```

Standalone targets are `th20_gameplay` and `th20_gameplay_entry_adapter`; the latter needs the actual platform/session/audio/runtime-state libraries and the explicit subsystem implementations when linked into a complete executable. Arbitrary corrupt object states, failed allocations and racing destruction during unresolved loading are outside the validated domain.

## Loading, player data and stage table

`player_state.cpp` contains 45 original setter descriptions, with their exact address, byte offset, write width and signed clamp. It restores `4bbde0` and `4bbd80` in original order, including the HUD callback before the fragments reset, preserving all untouched score/loadout/lives bytes. `4e15e0` also mutates the maximum-bomb field through its getters. `4bdfd0` uses SSE multiply and truncating conversion to preserve x86 results for NaN/overflow and the original ordering when the lower bound exceeds the upper bound. Other recovered helpers are `474d80`, `499480`, `4b81d0` and the table setters used by loading.

`stage_data.cpp` restores all eight `0x134` records at `5b0038..5b09d8`, including every message/resource string, all numeric fields and unused zeros. `extract_stage_data.py` is a read-only evidence tool with the original SHA check, not a production dependency. The three six-entry difficulty tables at `5afd08/5afd20/5afd38` are literal source data. `select_stage` reproduces `4be390`: it updates both table `+1f4` and the unique `selected_stage` pointer. Stage 4's background routing by player `+c` is retained. Out-of-range stage/difficulty/profile offsets throw instead of reading arbitrary adjacent memory; valid-data behavior is the equivalence domain.

`4bad40` now preserves sprite-task and graphics-event waits, surface animation interrupts, eight separately validated save-selection reads, old-game score comparison, fresh-game/practice/spell-mode initialization, all factory short-circuit failures, callback priorities, ECL/background routing, music queue synchronization, effect readiness, worker detach, statistics/timer resets, completion/failure display branches and profile/timestamp writes. The adapter defines actual BSS globals `slowdown_frames` (`5ba518`), `replay_file[256]` (`5c4b20`), `loading_overlay_state` (`5c4d3c`) and initialized `replay_selection=-1` (`5afcfc`). Three actual standalone BSS loading handles at `5c5b28/5c5b2c/5c5b30` are also defined. The original CRT initializer `40aa10` zeros exactly `0xde8` bytes for Graphics, so those adjacent handles are deliberately outside its unchanged type.

`player_cpu_validation.json`: **58,025 original CPU comparisons passed**, covering all 45 setters, two full player/table reset routines with the original nullable HUD absent, bomb count, random IEEE754 meter inputs, mutating getters, the eight stage rows and their pointed strings, selected-stage updates and all difficulty constants. The report binds the current source files by SHA-256. This is not an original-CPU comparison of the complete loading graph.

`loading_validation.json`: **40,345 source assertions passed** over three modes, all 64 low session-flag combinations, restart/fresh-state choices, seven playable stage IDs, all twelve factory failure positions, graphics-abort, wait phases, resource arguments and completion state. External entity/save operations are recorded test boundaries, never linked into production. These tests verify the restored loader orchestration without claiming those external entities are implemented.

```powershell
& source_reconstruction/gameplay/build/Release/th20_player_cpu_compare.exe '<verified th20.exe path>' source_reconstruction/gameplay/player_cpu_validation.json
& source_reconstruction/gameplay/build/Release/th20_gameplay_loading_tests.exe source_reconstruction/gameplay/loading_validation.json
```

## EnemyController and ECL resource ownership

`enemy.hpp/.cpp` restores the original `0x134`-byte EnemyCtrlInf prefix, followed by a source-only services pointer. Evidence is `4a28c0/4a2e80` construction, `4aba70` factory, `4a7080` initialization, `4a3920` teardown, `4a80f0` clear, and `4a7040` retained-stage reset. Its actual owner is `game_session::context(player).objects_04[1]` (Context `+8`). The layout includes `EnemyData` at `+10`, the 16-byte pmr vector of loaded names at `+b4`, countdown at `+d0`, eight AnimationFile pointers at `+e4`, loader at `+104`, intrusive list at `+108`, player index at `+12c` and context at `+130`. Initialization registers disabled callbacks at priorities `0x24/0x17`. Destruction unlinks entities and callbacks, clears the process ECL cache, destroys its loader and unloads ANM slots 25 through 32 in original order. This retains the original cross-player cache lifetime, including its assumptions about teardown order.

`script_loader.hpp/.cpp` restores the actual derived vtable order (load/include/destructor), original `0x23c`-byte prefix and writable file ownership. Base fields are two counts, 64 file pointers, 64 unused words, an ordered vector of 8-byte name/header records at `+20c`, and a **24-byte std::string** at `+21c`; player/context follow at `+234/+238`. The process cache at original `5c49f4` is a 12-byte pmr list of 32-byte data-pointer/pmr-string records. Source-only spans and size metadata validate bounds without fabricating script state.

| Address | Restored behavior |
|---|---|
| `4a2fc0`, `4ab1b0`, `4ab140` | Exact 164-byte controller data construction, first-48-byte reset and global generation/player packing |
| `4a3060`, `4a7170`, `4a3ac0` | Full 752-byte EnemyState construction/initialization and vector/shared-owner destruction |
| `4a7310`, `4a7360`, `48b550`, `478530` | Partial auxiliary reset, pattern reset, 388-byte movement-record and 72-byte motion constructors |
| `4a74d0`, `4bd4a0` | Per-player duplicate suppression before archive read, writable cache sharing across players, original resource path buffer |
| `53fe60`, `53fca0` | SCPT ID/version checks, stable sorted record insertion, file/count updates, include dispatch |
| `4a5be0`, `4aae10`, `4aaeb0` | ANIM resource lookup/reuse/load and recursively ordered ECLI includes; background file is the real slot-7 provider |
| `540340`, `53e8e0` | Binary midpoint name lookup including duplicate-name selection; 32-bit ECLH instruction address calculation |
| `4aa1b0`, `4a5040` | Original game-flag gates, controller frame orchestration, countdown, enemy traversal/retirement, special-manager call, player fields and timer tick |
| `4a8760`, `45d130` | Exact SSE slowdown operation order, animation slowdown writes, restored clock, and persistent bit 16 |
| `4ab5b0`, `4aaa40`, `4ab590`, `4ab850` | HUD time saturation and primary entity scale/flag helpers |
| `498600`, `498210`, `498a80`, `485660`, `4aaac0` | Complete integer/float destination tables, identifier lookup and original first-player handle resolution |
| `4aac00`, `47a370`, `47a3e0`, `47a2c0`, `498f90`, `478260` | Nearest-enemy identifier, target exclusion, position/identifier views, bomb mark |
| `4ab4c0`, `4a8260`, `4a5320` | Script-phase gate and actual callback ABI; full frame control flow, ANM parent/position/orientation, five timers |
| `4a7710`, `4a7da0`, `4a8d70` | Full movement flow, motion aggregation/clamp, extended global/per-axis Vec3 interpolation |

The `4a5040` return is `1` (`004a5300 MOV EAX,1`), despite Ghidra's inferred `void`. Its primary entity is **Context `+4`**, not the callback owner at Context `+0`. The `4a8760` initial COMISS/JB takes the slowdown branch for NaN; the zero/negative path does not clear old bit 16. Entity opcodes, mesh update `4a4190`, defeat `4a5640`, variable reads, damage and lifecycle now call their recovered source implementations. Special-state and HUD adapters use their actual shared owners. See `entity_opcode_validation.md` for current per-group coverage and remaining unverified composition boundaries.

`enemy_entity.hpp/.cpp` restores the actual `428`-byte Enemy with a distinct `70`-byte `ScriptManager` base, six virtual slots, `48`-byte main runtime, real PMR stack/interpolators, parent/child lists and 40-byte `std::function` member. It restores `4a33c0/4a35f0/4a3580`, `4a73f0/4ab970/4ab930`, `4972c0/4973c0`, `540200/540280` selection and `4a3b10/4a3ce0/4a2720` teardown. Stack reset reserves 256 words but retains logical size zero; this distinction was found by the original CPU comparison. Destructor marks children, unlinks parent/controller lists, adjusts counters/selected handles and destroys ANM/mesh/function/state/runtime owners in original order. Async clear deliberately leaves the sentinel link unchanged, matching the original clear-then-reset calling convention.

`enemy_spawn.hpp/.cpp` restores `47bb30` and complete `4a89c0`: all 84 parameter bytes, motion position, health/accounting fields, difficulty/rank byte, twelve script-variable words, flags, timers, shared stage clamp and default death-animation selection. It invokes the real `4a8760` composition before assigning defaults and discards that update's return as the original does. `spawn_enemy` implements `4a8920`: allocation and initialization, optional parent linking, parameter application/update, controller prepend and count increment. The root Bullet ETEX 24 source calls this typed API. Generic ECL ticks this actual runtime through enemy_vm.cpp; enemy_opcode_dispatch.cpp routes every recognized entity instruction to its source handler.

`enemy_state.hpp/.cpp` defines the actual `Enemy+88` subobject through `Enemy+377`. All 752 constructor bytes compare exactly, including real PMR allocator pointers. Its animation vector has 20-byte elements; its movement vector uses 388-byte records containing the original motion and interpolation fields; the singly linked queue has 80-byte nodes with 76-byte values and a shared owner; its auxiliary vector has 136-byte values. Original destructor `4a3ac0` was also run against populated source-owned containers and shared-pointer control blocks, verifying that it releases the expected owners and empties the containers. Repeated initialization deliberately preserves an existing first animation record while clearing/reconstructing movement records.

`enemy_variables.cpp` owns the actual four BSS script globals at `5c49d8..5c49e4`. Destination lookup preserves each original default-null case and fallback destination. `selected_enemy(owner,slot)` reads the requested owner's handle but resolves it against **player 0's controller**, matching `4aaac0` even when called for another player. Identifier lookup returns the first duplicate and preserves the iterator's observer cleanup.

Nearest-enemy search visits the real controller list in order, excludes `Enemy+350` bits 0/5 and `+354` bits 10/11, computes squared x/y distance with the original SSE operation order, and keeps the first strictly closer target. Equality at the radius is excluded; negative radii square as in the original; unordered distances do not select a target. `enemy_position`, `enemy_identifier`, `enemy_excluded` and `set_enemy_bomb_mark` expose the verified non-owning views needed by Bomb code. The mark writes only `Enemy+358` bit 5 from the argument's low bit.

`enemy_update.cpp` preserves both update gates, failure return values, script callback invocation with `ECX=EnemyState`, ANM resolution side effects and the entire remaining `4a8260` control flow. Normal ANM positioning adds link offsets and an optional parent's base position; the hidden branch copies only the motion position and deliberately does not invalidate failed handles. Five orientation modes update rotation, scale and dirty flags; the flip mode uses the original angle constructors and therefore wraps the added angle, including its unordered-comparison branch. The original pattern timer, two positive-only countdowns and two age timers update in order. Production adapters call these source functions and the recovered movement chain; entity-specific opcodes, defeat and mesh behavior call their recovered source bodies. The damage entry now calls the restored enemy_damage.cpp sequence. Timer mode 0 is the supported valid clock domain: the original indexed getter reads adjacent non-pointer globals for other indices, so the source rejects them rather than dereferencing fabricated pointers.

`enemy_movement.cpp` restores parent-relative following through the actual parent link owner, scalar/Vec2/extended-Vec3 interpolation, individual motion updates, hidden viewport offsets, summed motion/clamping, direction animation replacement, absolute animation extents and screen exit decisions. The offset at `5c50fc` is the existing Graphics `viewports[0].final_vector`, not another global. Original `4a7710` returns `-1` when an already-visible entity leaves the enabled horizontal or vertical deletion boundary; these return paths are absent from Ghidra's inferred `void` source and are restored from assembly. Invalid animation handles clear as in `44ced0`. All actual sprite operations in the production adapter use the recovered pool, named-spawn and geometry libraries.

The 100-byte movement-position interpolation has current/start/end/tangents at `0/c/18/24/30`, timer at `3c`, duration `4c`, three axis modes `50..58`, global mode `5c` and flags `60`. `enemy_interpolation.cpp` restores both whole-vector and per-axis modes, including endpoint returns that leave current storage unchanged. It reuses the verified ECL scalar sampler for the other scalar and Vec2 records. Motion aggregation uses the shared `runtime_state::Motion` implementation; a wide-angle CPU counterexample exposed an originally missed second wrap in that shared helper, now fixed and covered by both modules' regressions. `enemy_motion_counterexample.json` preserves the historical failing input, not a current validation result.


`enemy_damage.cpp` restores the complete `4a5df0` branch sequence plus `4aa250/4aa400` life/time phases. The actual 136-byte phase records retain threshold/time at `0/4` and script names at `8/48`; negative thresholds are consumed records. Phase health, reward, timeout flags and script dispatch retain their original order. `497c60` updates the actual current Player at Context `+24`, with signed wrapping addition and clamp `0..999999`; `499330/4994d0` clamps the shared session table `+1fc`; `4aaf50` clamps Player `+ac` to `0..100`. Helpers preserve all 28 health bytes, timer feedback at PlayerEntity `+2244` and raw hit color at ANM `+494`. Low-health flash thresholds read health `+8` (remaining phase health), not the threshold at `+10`.

The production damage adapter uses the actual Context `+28` `damage::HitCtrlInf`, recovered Bomb state/notification, SoundInf, pool binding and named spawn. Original `44bcd0` replacement marks the old animation, publishes the new handle, then writes the old animation's two file IDs; invalid input handles remain untouched. Enemy defeat `4a5640` and all entity-specific ECL opcodes have recovered source implementations and production bindings. Clearing/reset/selection now bind the actual ScriptManager; PlayerEntity circle/rectangle/graze bind the recovered Player module. Original `40e5e0` is a verified no-effect release-build body, so it contributes no production operation.

`script_program.cpp` bridges the loader into the source ECL VM through `ScriptLoader::bind_program`. The program contains borrowed writable instruction views into the real cache and copies no instruction bytes. The containing file's valid tail bounds the view, matching the original address model. Bind after include loading, and rebind if the set/order of loaded subroutines changes. The cache must outlive all views. Existing owning `ecl::Program` use remains supported; `Runtime::current` and interpolation origin lookup now consume either storage form. A source VM arithmetic instruction actually mutates a borrowed instruction field, and a second player's view observes that same mutation.

`enemy_vm.cpp` applies the decoded 75 core instructions directly to the real
72-byte EnemyRuntime and its PMR containers. It restores `53b5c0`, argument and
stack helpers, `53f3b0`, interpolation updates, and the actual manager
`53e2b0/53e390/53e920/53e560` linked-list scheduler. There is no shadow Runtime
or copied production script storage. `enemy_entity_adapter.cpp` binds the
frame loop to this source, using the shared RNG stream 0 and timer-rate pointer.
Spawned tasks are inserted after the main sentinel; traversal saves next before
execution, so a new task waits until the next traversal. Finished tasks are
actually destructed and freed. Call lookup restores manager.current only on
success, retaining the target pointer on failure.

An additional **37,707 original CPU comparisons** cover the core instructions,
4096 stack bytes, vector size/capacity, mutable script headers, interpolation
records, RNG state, synchronous return, 800 direct asynchronous calls including
missing lookup, and 400 six-frame native allocation/traversal/termination chains
with identifier queries. Original heap and lock bodies run against initialized
Win32 imports; no original code is patched. Continuing execution after a missing
async script name caused the original to dereference null in `53ed50`; only the
safe call-setup failure boundary is compared. Invalid scripts, allocation
failure, CRT fault/errno behavior, external entity opcodes and entity-variable
getters remain outside this equivalence claim.

`enemy_reads.cpp` implements both virtual getter bodies `49abc0/4995d0` and
uses real Enemy, Player, Context and session storage. It preserves the distinct
integer/float switch tables, type conversion, timer +4 versus +8 reads, missing
target float defaults (x=0/y=128), player-0 handle resolution, both enemy counts,
RNG consumption and mutating power/stage/score clamps. The production adapter
references Graphics.field_0b18 for 5c5858 and the existing replay-selection
global; no parallel game state is introduced. Another **88,666 original CPU
comparisons** execute all cases in d8f0..d96e with matching return bits, complete
PlayerTable writes and RNG state. Original integer915/916 null-target paths
have no fallback and are excluded; their float variants are tested with null.

Validation:

- `enemy_cpu_validation.json`: **1,254,114 original CPU comparisons passed**. Covers controller data/generation, the entire EnemyState constructor/initializer/ownership domain above, all destination-variable cases, nullable identifier lookup, SCPT record/count/file fields without include callbacks, stable duplicate records, exact name lookup, instruction addresses, HUD/player fields, nearest-enemy targeting including strict radius/ties/IEEE inputs, and slowdown with real pool animations and stale handles. Another 2,400 cases execute the full original `4a8260` body with movement already processed, the first ANM handle invalid (the original no-damage branch), and no mesh. Other real ANM slots exercise five orientation modes, parent offsets, invalid-handle clearing, all pool/state bytes and timers. Original instructions are unpatched. Another 4,000 comparisons of movement aggregation and 4,000 full `4a7710` calls cover all motion/interpolation records, parent following, viewport offsets, bounds flags/return, animation extents and handle resolution. The motion interpolation sampler has 17,408 output/state comparisons over 34 global/per-axis modes. A further 42,000 comparisons cover health arithmetic and feedback helpers, and 84,000 cover stage/bonus/reward clamps, both phase transitions and full 4a5df0 with pending damage, protection, callbacks, real ANM flash/cooldown, accounting and feedback. Added spawn coverage includes 5,000 each parameter/runtime constructors, 800 complete Enemy constructors, 500 full initializations with all stack/motion/ANM payloads and generation effects, original destructor count effects, plus 1,600 full 4a89c0 applications with the nested update already processed. Its active region-query, replacement-allocation, live entity-opcode ECL, defeat, native collision/graze and active timeout-notification branches remain outside this CPU comparison domain, as do mesh and device behavior.
- `enemy_resource_validation.json`: **3,304 assertions passed** against the actual encrypted archive and independent `src/ecl.cpp` parser. Seven playable stage closures contain respectively 145, 153, 167, 150, 134, 141 and 168 subroutines; every ordered name, ECLH/instruction byte sequence and cross-player pointer sharing matches. Every VM view points directly to those shared code bytes. Stage-table row 0's `st00.ecl` is absent from the shipped archive and has no synthetic fallback. GPU allocation is a recorded required test boundary.
- `enemy_frame_validation.json`: **5,378 source assertions passed** for all 256 four-entity pre-delete/returned-delete combinations, intrusive unlinking, callback order, countdown/HUD/player writes slowdown propagation/restoration/stale-handle invalidation, and all 256 pre-update/movement/script/callback/damage branch combinations. These last cases check the actual callback ABI, exact short-circuit order, gate lifetimes and timers. Thirty direction-change cases also verify old/new animation lookup, file selection, deletion, spawn arguments, transition-script selection, dimension updates and state writes. Another 1,280 assertions exercise damage geometry/query order, bonus and secondary-state reductions, preview defeat short-circuiting, phase-script dispatch, timeout writes, player collision/graze arguments and bomb-animation transitions. Active entity and special-manager calls are recorded boundaries, so these are not whole-frame original-CPU comparisons.

The three reports bind the enemy source/test files and shared ECL VM header/implementation by SHA-256. The updated ECL VM additionally passed its **41,067 original CPU comparisons** and source ownership/traversal/shared-write tests. Invalid or truncated resources, out-of-range indices, missing files and allocation failure are outside the original equivalence domain; source bounds checks throw rather than reproduce arbitrary reads or null dereferences. No archive bytes are executed as native code.

```powershell
& D:\cmake\bin\cmake.exe --build source_reconstruction/gameplay/build --config Release --target th20_enemy th20_enemy_cpu_compare th20_enemy_resource_tests th20_enemy_frame_tests th20_gameplay_entry_adapter
& source_reconstruction/gameplay/build/Release/th20_enemy_cpu_compare.exe '<verified th20.exe path>' source_reconstruction/gameplay/enemy_cpu_validation.json
& source_reconstruction/gameplay/build/Release/th20_enemy_resource_tests.exe '<th20.dat path>' source_reconstruction/gameplay/enemy_resource_validation.json
& source_reconstruction/gameplay/build/Release/th20_enemy_frame_tests.exe source_reconstruction/gameplay/enemy_frame_validation.json
```



Current entity opcode, mesh and defeat coverage is documented in [entity_opcode_validation.md](entity_opcode_validation.md). Older paragraphs describe the narrower original fixtures; the current report adds the separately identified groups, not whole-game equivalence.
