# Source game gap audit — 2026-09-19

This is a bounded review of production callback registration, callback return
values, and apparent empty/unsupported bodies. It does **not** certify complete
source recovery or full-game equivalence.

## Findings

1. **Effect registration order differed from the original; corrected by the
   parent task during this audit.** `effect_system/lifecycle.cpp:12` originally
   registered both nodes enabled. Original `49d790` calls disabled registration
   helpers at `49d7d5 -> 412310` and `49d7ee -> 4123b0`, stores both node pointers,
   and only then calls `4216a0` at `49d7ff`. Source now uses `false` for both
   registrations and retains the immediate `CallbackOwner::enable_callbacks()`.
   Final enabled state was already the same; the actual difference was the
   interval in which the scheduler could observe the first inserted callback.
   The follow-up read-only review confirmed that both registrations remain
   disabled until the explicit enable call. No crash or gameplay divergence was
   reproduced by this audit for this particular interval.

2. **Player and HUD startup registration fixes are present.** Player `4f9520`
   registers update priority 29 and draw priority 30 disabled. HUD `4b5820`
   registers update priority 42, player draw priority 58, and secondary draw
   priority 52 disabled. Current `player_entity/initialize.cpp:44` and
   `hud_system/lifecycle.cpp:48` agree. These were repaired by the parent task,
   not discovered anew here.

3. **No additional initially-enabled mismatch was found in the indexed direct
   registration callers.** The inventory contains **31 original registration
   owners, 145 call sites, and 131 distinct callbacks**. An earlier informal
   count of 35 included the four helper definitions; 31 is the caller count.
   Corresponding production source exists for every indexed owner. This does
   not prove that every callback body or every indirect registration is exact.

## Callback return values checked against machine instructions

Ghidra's `void` declaration is insufficient for scheduler callbacks: return 0
removes a node, while return 1 retains it. The following current source `int`
returns agree with original instructions and must not be replaced by `void`:

| Original function | Original evidence | Source behavior |
| --- | --- | --- |
| `424e00` | `424e6d mov eax, 1`, preserved through security-cookie check | Screen draw returns 1 |
| `424e80` | `424f19 mov eax, 1` | Playfield draw returns 1 |
| `424f30` | `424f86 mov eax, 1` | Current view draw returns 1 |
| `424fa0` | `425057 mov eax, 1` | Offset playfield draw returns 1 |
| `4dd1c0` | `4dd2c0 mov eax, 1` | First depth clear returns 1 |
| `4dd2e0` | `4dd3e0 mov eax, 1` | Second depth clear returns 1 |
| `478bf0`, called by `49dea0` | `478bf7 mov eax, 1`; no rendering body | Empty shared draw returns 1 |
| `470300 -> 46b6e0` | thunk preserves EAX; `46b7c5 mov eax, 1` | Text update returns 1 |
| `4ba870` | both sides of game flag test load EAX=1 | Game draw clears counters, returns 1 |
| `4b5790` | calls viewport/context/player drawing, then `4b57bb mov eax, 1` | HUD player draw returns 1 |

These were static instruction checks, not a new dynamic differential suite.
Other module-specific callback suites remain separate evidence.

## Registration map reviewed

`4122c0/412360` insert enabled update/draw nodes. `412310/4123b0` insert disabled
update/draw nodes. Priority and enabled state were compared with production
registrations, including the Sprite and Graphics registration tables.

| Original owner | Production module | Initial state |
| --- | --- | --- |
| `423bf0` | screen_effect | enabled |
| `447e00` | sprite_renderer | enabled |
| `46c080` | text_renderer | disabled |
| `473160` | stage_background | disabled |
| `477dc0` | bomb_system | enabled |
| `480fa0` | bullet_system | disabled |
| `487b10` | card_system | disabled |
| `49d790` | effect_system | disabled, then explicitly enabled |
| `4a0600` | ending_scene | enabled |
| `4a7080` | gameplay Enemy | disabled |
| `4ac080` | platform_window FrameStatistics | enabled |
| `4b5820` | hud_system | disabled |
| `4bad40` | gameplay GameController | disabled |
| `4bf990` | help_system | disabled |
| `4c0b30` | damage_regions | disabled |
| `4c3c00` | item_system | disabled |
| `4c58d0` | key_config | enabled |
| `4d3ca0` | laser_system | disabled |
| `4d8220` | startup_scene | disabled |
| `4de1f0` | platform_window Graphics | enabled |
| `4df670` | notice_system | disabled |
| `4e0e80` | options_system | enabled |
| `4e5980` | pause_system | disabled |
| `4f9520` | player_entity | disabled |
| `508480` | replay_system | disabled |
| `510640` | small_score | disabled |
| `511270` | stage_clear | enabled |
| `519960` | stone_menu | enabled |
| `51f3c0` | title_system | disabled |
| `52e170` | trophy_system | enabled |
| `532fb0` | overlay_system | enabled |

## Apparent gaps excluded after review

- The `unrecovered` namespace is historical naming; linked adapters have real
  definitions. Its name alone is not evidence of an unimplemented module.
- Bomb base update/draw/finish map to original `412540` and base event maps to
  `477ce0`, both zero-return bodies. Character-specific overrides exist.
- Weapon base empty methods map to original `40e5e0`, `414b60`, and `52fbc0`;
  nullable offset methods and zero-return phase defaults have original vtable
  entries. These are not substitutes for missing concrete weapon overrides.
- The shared empty draw `49dea0 -> 478bf0` is used by Effect, Ending, Help,
  Notice, Trophy, and Overlay. It really returns 1 without drawing.
- `gameplay/enemy_opcode_dispatch.cpp` retains an "Unrecovered enemy ECL opcode"
  exception for a missing optional result. Current routed ranges have handlers;
  the original undefined/default range, including opcode 569, returns 0. The
  exception string alone is not proof that valid packaged scripts hit a gap.
- Bounds and null precondition throws in production generally document original
  invalid-input domains. This audit did not prove all such branches unreachable
  during packaged gameplay. Failures reached in the real source executable
  still require individual investigation.

## Reproducible evidence and remaining work

Run `audit_source_callbacks.py` with the workspace Python. It verifies original
EXE SHA-256, reads but does not execute the EXE, and emits
`source_game_callbacks.json`. The JSON contains the original registration
instructions, exact helper/priority/callback triples, selected return-value
instructions, current production registration snippets, and source hashes.

The audit leaves the main integration criteria open: loading/playing every stage,
all character/stone combinations, replay save/load and deterministic replay
comparison, pause/restart/continue, resource lifetime, audio/visual composition,
and same-input/same-seed frame-state comparisons. Module checks and a successful
link do not replace those acceptance criteria.

## Follow-up: allocation pairing during repeated demo diagnosis

The parent task reported heap corruption during the third automatic gameplay
demo and is performing separate runtime memory validation. This follow-up is
static ownership evidence. **All three allocation-pair mismatches below have
been fixed by the parent task and reviewed in the current production source.**
None has been proved by this audit to cause that specific crash. No production
changes, engine builds, or game executions were performed by this review.

`runtime::allocate_bytes` uses `std::malloc` under shared allocation lock 1;
`runtime::release_bytes` uses `std::free` under the same lock.
`runtime::retire_callback_owner` performs the virtual destructor explicitly,
then calls scalar `::operator delete` under lock 1. Original CRT equivalence does
not make crossing these API pairs a valid C++ allocation contract.

| Object | Previous allocation | Release in production | Current reviewed status |
| --- | --- | --- | --- |
| Player | `player_entity/initialize.cpp`, `create_player`: `allocate_bytes(sizeof(Player))` | `initialize_adapter.cpp`, `destroy_player` → `retire_callback_owner`; initialization failure uses the same retire helper | Fixed: scalar nothrow new / scalar delete |
| TitleInf | `title_system/lifecycle.cpp`, `create`: `allocate_bytes(sizeof(TitleInf))` | `platform_window/scene_transitions.cpp:23,37` → `retire_callback_owner(menu_scene)` | Fixed: scalar nothrow new / scalar delete |
| CurveNode | `laser_system/type2.cpp`, `create_curve_node`: scalar `::operator new(..., nothrow)` | `Type2Laser::finish`: walk `path.next`, `release_bytes(node)` | Fixed: malloc / free |

Applied repairs reviewed:

- Player and TitleInf now use `::operator new(sizeof(T), std::nothrow)` inside a
  local lock-1 scope, then check for null and zero-fill before the existing
  placement-construction and initialization sequence. Lock 1 is released before
  constructors and resource loading. Player initialization failure still retires
  the constructed owner through its virtual destructor. Title's input latch and
  worker startup remain after successful construction. The original Title
  factory `51d900` has an explicit null-allocation return; the newly added guard
  restores that case, which the previous source omitted.
- CurveNode changes only its factory allocation to
  `runtime::allocate_bytes(sizeof(CurveNode))`. The existing placement
  constructor and linked-list free path then agree. A CurveNode is trivially
  destructible. The independently correct malloc/free pairs for samples,
  geometry, and segment buffers remain in place.
- Generic `allocate_bytes` and `retire_callback_owner` contracts remain intact:
  most CallbackOwner factories already use scalar new, while Enemy,
  EnemyRuntime, raw assets, and many data buffers correctly use malloc/free.

The review found no issue with these minimal edits. It did not add exception
recovery around placement constructors or resource initialization, audit every
out-of-memory path, or establish that repeated-demo heap corruption is resolved.
`source_game_allocation_review.json` captures nine checked source conditions and
the hashes of eight reviewed source files. It explicitly identifies this as a
source review, not a runtime memory-safety test. `source_game_callbacks.json`
was also regenerated from the current source; its inventory remains 31 owners,
145 call sites, and 131 distinct callbacks.

Concrete CurveNode reachability: `Type2Laser::append_path` in
`type2_commands.cpp` allocates nodes for motion commands; `Type2Laser::initialize`
also allocates them when cloning an incoming curve path. Controller update calls
`finish` before destroying a laser, and the Type2 destructor also calls `finish`.
The latter is **not itself a double-free**: `finish` nulls `path.next`, `geometry`,
and `samples` after releasing them.

Other apparent double-free candidates checked and excluded:

- Replay playback `load` sets `user == decoded`, pointing to the start of one
  allocated block. Replay destruction frees `user` once; it does not also free
  `decoded`. Playback stage records point inside that block via `playback[i]`.
  The destructor's separately freed `stages[i]` array is for recording mode and
  stays null in playback mode. `PlaybackCursor` destruction only unlinks its
  list node.
- Replay recording chunks use malloc/placement construction and explicit
  destruction/free. Their destructor unlinks the embedded link before release.
- Background deliberately invokes destruction of its eight embedded animations
  twice to match original lifetime code. `destroy_animation_contents` clears
  the allocated-geometry and callback fields on the first call, so those fields
  are not freed a second time in the normal sequential path.
- Animation file unload calls clear then destroy, whose clear call sees
  `file.bytes == nullptr` and returns. Texture, sprite, script, and template
  arrays use matching new[]/delete[] pairs.
- Enemy entities, async EnemyRuntime objects, async list links, shot data, heap
  shots, and enemy mesh-owner records use malloc/free consistently in the
  inspected production factories and retirement paths. Nested iterator lifetime
  is being investigated separately by the callback verification task.

AddressSanitizer's allocation-pair diagnostics can be enabled with
`ASAN_OPTIONS=alloc_dealloc_mismatch=1`. That is separate from proving a heap
overwrite or use-after-free in the repeated-demo path.

## Follow-up: production menu input path

This was a read-only source review: no game process was started or controlled,
and no keys were injected. **No confirmed wiring, mask, or lifetime defect was
found in the reviewed path.** The parent subsequently reported a transition from
demo (`session_flags=32`) to actual Easy gameplay (`session_flags=0`); that is a
separate runtime observation, not a result generated by this audit.

The production path is connected as follows:

- `program_entry/program_entry.cpp` drains `PeekMessageW(..., PM_REMOVE)` and
  dispatches messages before advancing frames. `platform_window/window.cpp`
  writes `window_state.active` on `WM_ACTIVATEAPP`; its verified layout is +0x68
  from original storage `5b6758`, matching original `5b67c0`.
- `input/entry_adapter.cpp` passes the actual HWND/HINSTANCE and a live reference
  to that active byte into the persistent ControllerContext. Graphics update
  priority 1 calls `sample_game_frame`, ahead of Title update priority 11.
- Normal keyboard polling is `Host::keyboard -> GetKeyboardState`, not the
  DirectInput keyboard's `GetDeviceState`. Keyboard device 0 is initialized even
  when DirectInput setup fails. This agrees with original `421b00`.
- `input/input_state.cpp` only polls keyboard while active. VK_RETURN (`0x0d`)
  maps to `0x80000`; configured action 0 maps to `1` (default Z). The aggregate
  in `input/input_win32.cpp` assigns keyboard OR all gamepads to slots 0 and 2,
  matching original `41fe80`; selected gamepad does not discard keyboard input.
- `bind_button_slots` binds the published aliases to persistent shared slots.
  `MainEnvironment::pressed/repeated` reads slot 0, matching original pointer
  `5b889c`. `pressed=(current ^ previous) & current`; repeated also tests
  `repeat8`. Original helpers `419c00` and `41a280` use offsets +0x10 and +0x08.
  Main confirm uses `0x80001`, while directions use repeat masks `0x10/0x20`.

There are explicit input acceptance conditions, not demonstrated defects:

- Main menu `529e80` accepts confirmation only in phase 2 after its age exceeds
  130 and blocking notices finish. It uses a pressed edge, not held/repeated
  confirmation. An Enter edge during the intro is not queued for phase 2;
  holding Enter across that boundary does not produce another edge. After a
  valid confirm it waits 20 frames in phase 4 before entering difficulty state 5.
- The snapshot input design can miss a complete down/up pair processed between
  two samples. Static inspection cannot establish whether this happened during
  the earlier UI-tool short press, or whether the game had focus then.
- The title idle timer tests `current & 0xffff`, so Enter alone does not reset
  it. Original `51e3c0` has the same mask and the same 1799 threshold; this is not
  an introduced mask error. `window_state.input_latch` indexes frame-delay
  counters in `platform_window/present.cpp`, not a keyboard suppression flag.

If diagnosis is needed again, the useful observations are the active byte,
keyboard `raw[0x0d]`, device-0 current bits, slot-0 current/previous/pressed,
and Title state/phase/age/notice status in the same frame. A sampled active Enter
transition should set `raw[0x0d] & 0x80`, current `0x80000`, and one pressed edge.
No conclusion about the OS or UI tool's actual injection was made here.

The initial review found stale input.hpp/input_win32.cpp hashes in the historical
`input/cpu_validation.json`. The parent task subsequently rebuilt and reran
`th20_input_cpu_compare` and `th20_input_tests`: source tests passed and the
refreshed CPU report records **44,271 comparisons, zero failures**. This audit
then independently checked that the report's input.hpp, input_state.cpp, and
input_win32.cpp hashes match the current files (`fd36f455...`, `5598f4cd...`, and
`b655c5c2...`). The CPU run still uses an isolated Host, so it does not establish
the earlier UI tool's key-delivery timing or live window focus. The test run was
performed by the parent task, not by this read-only input review.

## Follow-up: shared pool CPU regression after CurveNode allocator repair

The existing `sprite_renderer/pool_test` target `th20_pool_cpu_compare` was
configured and compiled independently in `build_pool_audit_20260919` using
Visual Studio 16 2019, Win32, Release. The run completed successfully with
**2,915,831 checks, zero failures**: 2,915,767 original-CPU comparisons and 64
explicitly labeled source-only Type2 ETEX13 checked-error cases. No production
file was changed, no original game entry point or UI was run, and the parent
build directories were not used.

All **389 distinct bound source hashes** matched current source before publishing
the report (the generated header contains 390 entries because one input is
listed twice). `laser_system/type2.cpp` is bound to
`a7fb29bb5ceaa4a142554ff742d7a93752a6deade1b88e8c566b908de72a0b63`.
Laser groups total 431,652 checks, of which 171,130 have the `laser_type2_`
prefix; the 64 source-only cases are included in those totals. The Type2 cases
execute actual path-node allocations, clone/append operations and cleanup, then
compare normalized addresses, node contents and list relations. This is a CPU
regression, not an AddressSanitizer run or a whole-game heap-safety proof.

The canonical `sprite_renderer/pool_cpu_validation.json` was refreshed only after
the successful run and hash check. The existing `bullet_system/record_pool_evidence.py`
then refreshed all 13 module reports; their shared-report hashes and totals were
checked again. Raw results, the previous canonical report, build/run logs and a
compact manifest are retained as `audit/pool_allocator_regression*` and
`audit/pool_before_allocator_regression.json`. Player/Title owner factories and
Effect callback registration are outside this target's compilation and are not
claimed as validated by this result.

## Follow-up: runtime-confirmed Laser Segment allocation mismatch

After the first pool regression, the parent supplied a new ASAN failure at
`source_reconstruction/diagnostics/asan_after_enemy_fix/asan.27092`. It identifies
a **60-byte Segment allocation**, not an animation-handle allocation:
`Type0Laser::initialize` scalar nothrow new → `spawn_type0` → enemy laser opcode;
later `Controller::update` → `destroy_laser` → Type0/base destructors →
`runtime::release_bytes/free`. The captured error is `alloc-dealloc-mismatch
(operator new vs free)`. This particular API mismatch is demonstrated at runtime;
it should not be conflated with the earlier third-demo heap corruption.

The complete production write-site search for `allocated_6d8` found:

| Writer | Allocation before this fix | Shared release |
| --- | --- | --- |
| Type0 initialize | scalar nothrow new + placement Segment | Laser base destructor, free |
| Type1 initialize | scalar nothrow new + placement Segment | Laser base destructor, free |
| Type2 initialize | allocate_bytes(segment_count * 0x3c) | Laser base destructor, free |

The parent has changed the Type0 and Type1 allocations to
`runtime::allocate_bytes(sizeof(Segment))`, preserving the placement constructor,
null result, field assignments and original return behavior. This makes all
three owned segment-storage paths agree with the existing base destructor.
Segment has only scalar/vector-value fields and is trivially destructible.
Type3 initialization has no production assignment to this base pointer; the base
constructor clears it. Concrete Laser objects themselves still correctly use
scalar new and `destroy_laser`'s scalar delete.

The similarly named `animation_handle` in Type1 cancellation is a stack-local
32-bit ANM identifier passed by reference to named spawn, not an owning heap
pointer. It needs no allocator change. The independent target was then
incrementally rebuilt and rerun against the repaired Type0/Type1 sources:
**2,915,831 checks, zero failures**, with all 389 current source hashes matching.
The canonical pool report and all 13 module reports were refreshed again and
their shared-report hashes verified. The result is the same suite rerun, not an
additional disjoint set of 2.9 million checks. The first result remains retained;
the final run, logs and manifest are `audit/pool_segment_allocator_*`. This
non-ASAN CPU suite cannot by itself prove that the live ASAN symptom is resolved;
that runtime check remains with the parent task.
