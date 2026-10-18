"""Record module-specific evidence from the real shared Sprite CPU oracle.

Run only after rebuilding and executing pool_test/Release/th20_pool_cpu_compare.
This recorder refuses a failed report or any changed source input.
"""
from pathlib import Path
import hashlib
import json

root = Path(__file__).resolve().parent.parent
report_path = root / "sprite_renderer/pool_cpu_validation.json"
report = json.loads(report_path.read_text(encoding="utf-8"))
if report["status"] != "passed" or report["failed"]:
    raise SystemExit("The shared CPU oracle has failures; no successful evidence recorded.")
hashes = {}
for name, expected in report["source_hashes"].items():
    source = (report_path.parent / name).resolve()
    actual = hashlib.sha256(source.read_bytes()).hexdigest()
    if actual != expected:
        raise SystemExit(f"Source changed after the bound oracle build: {source}")
    hashes[str(source.relative_to(root.parent)).replace("\\", "/")] = actual

modules = {
    "effect_system": (
        "additional_validation.json", ("effect_spiral_", "effect_reverse_spiral_", "effect_filled_ring_", "effect_triple_ring_", "effect_burst_rings_", "effect_radial_trails_", "effect_wavering_", "effect_converging_", "effect_transition_", "effect_stone_"),
        ["SpiralTrail and ReverseSpiralTrail complete constructors over 256 fill patterns each, including animation callback attachment.",
         "4,096 initialization/update cases per class compare whole objects, whole attached ANM, complete RNG state and return values.",
         "Update covers repeated/new frames, the 16/50/70/80-frame boundaries, timer rates/null rate pointer, nullable player0 and a distinct player1.",
         "256 draws per spiral class execute actual polyline generation, compare all vertex-buffer bytes, ANM mutations, render cache and COM call trace.",
         "FilledRing, TripleRing, BurstRings and RadialTrails each have 256 full constructors, 2,048 initialize/update cases and 256 complete draws.",
         "The four ring/radial draw groups capture vertex bytes at every actual DrawPrimitiveUP call, as well as final buffer/cache state. This covers the original thick-polyline write-2N/advance-N behavior without losing overwritten earlier submissions.",
         "StoneSelection covers 256 constructors, 2,048 full init/update/interrupt/retire cases and 256 actual draws, including initial six-ANM allocation and replacement on expansion, all handled events, timers, selected-profile switches and every fan/circle/outline vertex submission.",
         "TransitionPanels compares all five ANMs in 256 constructors and empty-owned destructors, 2,048 init/update/interrupt triples and 512 actual masked/unmasked draws; every COM submission vertex and state order is checked.",
         "ConvergingParticles adds full constructor/attachment and 4,096 initialize/update/retire/interrupt cases. Updates execute real named child spawning, 200 handle slots, both Hermite phases, whole 64-animation pool, file counter, RNG and returns.",
         "WaveringTrail adds 256 full constructors, 4,096 initialize/update pairs and 256 draws, including screen scales, zero extent, 30/60-frame boundaries and the exact player0 region alpha gate.",
         "RadialTrails initializer uses the actual two-stack-argument ABI and compares integer return zero; the omitted Ghidra argument is verified by RET8 at 0x466e1a."],
        ["Native vptr is normalized only in constructor comparison; native instructions are never patched.",
         "COM draw endpoints are trace fixtures; this is not a GPU pixel comparison.",
         "Negative sample indices are outside the recovered valid input domain and are excluded.",
         "StoneSelection gameplay queries are explicit test boundaries. The native side uses real Gameplay/Player/SaveManager/StoneMenu storage and four boolean virtual-call fixtures; the source core receives corresponding named environment inputs. Its production query adapter, metadata validation, weapon behavior and game scheduling are not claimed by this group.",
         "Earlier effect-owner and line types have separate cases in the shared report; whole-game scheduling is not validated by this record."]
    ),
    "bullet_system": (
        "pool_validation.json", ("bullet_",),
        ["Full Bullet 0x528 and Controller 0x286da8 constructor state; only the compiled controller vtable pointer is normalized.",
         "Float timer including NaN, infinity, conversion overflow, subnormal inputs, and random initialization/mode flags.",
         "Viewport and circle geometry; full-state nine movement operators and player-bearing adapter.",
         "Real ANM label-1 interrupt execution, sound request queue, rectangle/circle cancellation across 32-element active lists.",
         "Advance states 0/2/3/4, actual retirement and Controller iteration with motion/hit excluded by original flags.",
         "Bounce and offscreen-delay with full Bullet state, true sound queue and return values.",
         "Command and ShotMetadata constructors, real PMR allocation and metadata destruction; the original style initializer's complete 17,200 bytes and all 50 radii.",
         "ETEX opcodes 0..33 excluding 24: complete Bullet, 64-animation pool, writable command storage, RNG and sound state; sequential execution, parallel gates and counted jumps.",
         "All 4,000 style remap values; complete restart state for 2,048 valid/missing/not-ready script cases, and 3,200 real style/child-animation lifecycle cases.",
         "ShotParameters constructor and 3,328 complete shoot-one calls across all thirteen patterns, including near-player rejection/retirement, all fifty styles, real shared_ptr reference counts, ANM state and RNG.",
         "512 ETEX-13 native/source allocation, nested shooting and parent cancellation cases; full metadata payloads, writable vectors, shared ownership topology/reference counts and real destruction.",
         "8,192 resolve-angle boundary/NaN/infinity cases including real player/enemy selectors and both return-value and RNG state comparison.",
         "32 original 47b290 shared-metadata factory calls verify the actual vector has size and capacity two, matching source construction.",
         "512 ETEX27 cases for each of Type0 and Type1 create actual beam objects and optionally cancel their parent Bullet; complete beam/commands/ANM/Segment/list/RNG/sound state is compared."],
        ["Original executable is mapped only in the isolated validation executable; no original instructions are patched.",
         "ETEX opcode 24 and style bit-0x8000 enemy-resource selection have not been exercised. Player collision and Item creation remain explicit throwing fixture boundaries.",
         "ETEX-13 independently allocated metadata/vector addresses are normalized; their size, capacity, data, aliasing and owner counts are compared separately.",
         "ETEX-13 independently allocated ShotMetadata tail padding at 0x4a/0x4b has no common initial value and is excluded. All defined fields are compared, and separate full-fill-pattern constructor tests verify that both implementations preserve those two bytes.",
         "The style-table pointer and proven remap callback address are normalized to their independently compiled equivalents; other state bytes are compared exactly.",
         "Original CRT NaN sqrt faulted in the isolated image; offscreen-delay positions are finite. Bounce retains NaN position cases. See unavailable_nan_probe.json.",
         "Cancellation uses drop mode 0 and either flag 0x200 or negative cancel-script; visual spawn-on-cancel and drops are not covered.",
         "ANM input is a bounded synthetic interrupt-label/stop program, not a complete gameplay replay."]
    ),
    "laser_system": (
        "pool_validation.json", ("laser_",),
        ["Full 0x58 Controller constructor state, with compiled vptr normalization only.",
         "Rectangle and circle controller filters, argument forwarding, sum of signed virtual return values, and whole controller/list/entity state for 16 trace entities.",
         "Real Type3 0x1d30 constructor/destructor across 256 initial fill patterns; 2,048 complete initialization and owned PMR command-copy cases, deletion signals and parameter flags.",
         "512 Type3 destruction cases owning nonempty PMR vectors, ANM geometry, and the base allocation; whole state compared after actual deallocation.",
         "8,192 base distance-point outputs and 8,192 position/context whole-base-state comparisons; Type3 initialization covers both context indices.",
         "Real Type1 constructor/destructor over 256 initial fill patterns; 8,192 complete ETEX/advance cases and 8,192 collision-segment cases, including timer phase boundaries and selected NaN scalar inputs.",
         "2,048 Type1 initialization cases with actual two-ANM execution, all fifty styles/sixteen colors, sound requests, owned Segment allocation and PMR command copies.",
         "8,192 base-proximity results and 4,096 Type1 rectangle-measure cases with real Enemy animation-link vectors, registered ANM and sprite geometry.",
         "Full Type0 constructor/destructor over 256 fill patterns; four motion operators over 2,048 cases each, 8,192 actual ETEX/advance calls and 8,192 collision-segment cases.",
         "2,048 Type0 initializations execute actual three-ANM scripts, sound requests, all fifty styles/sixteen colors, both contexts and owned Segment/PMR command allocation.",
         "512 Type1 and 512 Type0 split cases create and destroy actual Type0 child objects. Whole parent/child state, linked-list topology, commands size/capacity/content, Segment values and ANM counters are compared.",
         "128 cases for each of six cancellation/erase operations on Type0 and Type1 execute actual cancellation ANM and actual surviving Type0 spawns; includes NaN circle radii.",
         "Three Type2 path samplers each run 8,192 cases, including angular-threshold neighbors, fractional/negative time, linked-interval boundaries, invalid kinds and unmatched NaN time.",
         "Full Type2 constructor/destructor, five motion operators, complete advance with command cursor past the vector, and collision-segment compaction are compared over whole objects and sample/storage/output buffers.",
         "1,024 Type2 initializations compare actual two-ANM execution, initialized samples/geometry, deep-copied paths, commands, sound and owned allocation presence.",
         "Type2 ETEX 0..33 except 13 each run 128 original-CPU cases, including actual heap path append and all path fields/next/previous links.",
         "Separately labeled source-only checks verify ETEX13 raises its documented checked-copy error after advancing the command cursor once."],
        ["Manager virtual-call tests use trace entities with separate original-slot and C++ virtual tables; the Type3 group separately uses actual recovered Type3 source.",
         "Type3 vptr and allocated PMR vector addresses are normalized; vector size/capacity/content and other bytes are checked exactly.",
         "Type1 tests compare the original advance through its actual native vtable and restored source advance through independently compiled C++.",
         "Split-child object addresses, verified original vtables, ANM self/owner pointers, remap callbacks, Segment allocation and PMR storage addresses are normalized; corresponding data and graph topology are compared separately.",
         "Type2 type=2 initialization requires Enemy-owned animation resources and is excluded. Its uninitialized collision-segment allocation payload is not compared before first use.",
         "Type2 ETEX13 has an original out-of-bounds copy into a two-command allocation; checked source-only behavior is not original-CPU equivalence. See TYPE2_ETEX13.md.",
         "Player collision/graze, complete concrete-beam update/draw, Type2 cancellation, file loading and asynchronous scheduling are not covered here."]
    ),
    "hud_system": (
        "draw_validation.json", ("hud_",),
        ["PlayerFeedback 0x4f8840 executes 8,192 cases over complete Feedback and Renderer storage, including actual formatted text/shadow queue entries, timer restart/blink, alpha, signed hit counts, window scales and full/near-full text queues.",
         "HUD primary numeric rows compare 4,096 full FrontInf, Renderer, Player and continuation-digit states plus return, including signed-low-word high score behavior and all mutating range clamps.",
         "2,048 complete HUD scene cases add actual native CRT formatting, valid/invalid encoded spell best times, missing/present registered popup handles, all 41 numeric threshold entries, real Enemy linked lookup, Boss/pause/game flags, ANM deletion and whole text/FrontInf/Player states.",
         "256 HUD player-wrapper calls execute actual viewport selection and camera calculation, compare full graphics, Feedback and Renderer storage, Sprite offsets, return and COM matrix/viewport traces.",
         "8,192 icon cases compare all fourteen Animation objects and FrontInf bytes for life/bomb selection, 0..7 maxima, -1..7 counts, three fragment values and missing first icon.",
         "3,072 notification cases execute real named animation spawn, registered old-handle deletion, exact digit sprite assignment and hide/enable operations, all handled and unhandled notification kinds and numeric separator boundaries.",
         "4,096 enable cases compare full FrontInf, Player and PlayerTable, all three scheduler nodes, 64-animation pool and three resource counters. This covers first/repeated enable, practice/stage/initial-stage creation, real icon interrupts and field clamps.",
         "4,096 full 4b2c90 frame cases run actual two-boss lookup, health-ring allocation, four marker rotations with original cumulative scratch Z, countdown sprite changes, item/reward ticker, handles, sound requests and timer boundaries; whole FrontInf, PlayerTable, 64-animation pool, counters, sound and returns are compared.",
         "32,768 scoring cases compare complete FrontInf and Session for uint64 boundaries, unsigned wrapped distance/speed, sign-extended low-word comparisons, high-score updates, continuation digit clamp and flags."],
        ["Original instructions and CRT functions are not patched; only actual import/function-boundary fixture state is supplied.",
         "This validates queued text, Renderer state and bounded HUD enable behavior, not GPU pixels or the complete HUD owner lifecycle. StoneMenu.open when Graphics+0xb18 is set remains an explicit throwing boundary and is excluded from enable cases. Active Dialogue update/deletion is also explicitly excluded from frame cases.",
         "Spell, Pause and Enemy owners use their actual accessed storage fields; their full owner lifecycles are outside this draw test. Numeric fragments above the actual 41-entry threshold data span throw in recovered source instead of reading adjacent unrelated native data.",
         "The oracle compiles the production Session storage/constructor and renames only its unused context accessor, because the common fixture already supplies context() as the mapped test-state view; this build choice is bound in the CMake input hash."]
    ),
    "stage_clear": (
        "pool_validation.json", ("stage_clear_",),
        ["256 complete original/source StageClearInf constructors and empty-owned destructors over fill patterns; full 0x98 state and the actual global owner pointer are compared, with only vptr normalization.",
         "8,192 direct cases cover all eight mutating level/phase getters, full Player state and return values.",
         "4,096 full 510b60 calls cover states 0..8 except the self-delete point, signed low-word bonus arithmetic, getter clamping, actual score adder, GameController flags/counter, real named ANM spawning/deletion, input thresholds, timers and sound command queues.",
         "2,048 full 510e80 draws execute actual CP932 formatting and dynamic.cpp::write_text against six existing zero-frame TextJobs. Complete StageClearInf, Player, Renderer, six Job objects, 64-animation pool and return values are compared.",
         "The shared ANM fixture now reads the actual GameController animation_frozen flag, including frozen StageClear spawns; no instructions are patched."],
        ["Resource initialization/factory scheduling and file IO are compiled source but not validated by this group. Destruction tests use no owned scheduler nodes or secondary ANM handle.",
         "State 6 at timer 10 calls game stage completion and deletes itself; that domain remains an explicit throwing test boundary and is excluded.",
         "Dynamic text cases reuse six cached jobs. New-job allocation/layout, GDI rasterization, deferred scheduling and GPU upload are not exercised or claimed, though their actual source is linked.",
         "Native mutex slot 11 is explicitly initialized for actual audio enqueue. A previous fixture abort before this initialization is retained, as is the subsequent frozen-ANM fixture failure record.",
         "Production game globals are represented by isolated test storage. This is module state/call evidence, not a complete gameplay replay."]
    ),
    "card_system": (
        "pool_validation.json", ("card_",),
        ["256 complete CardInf constructors and three-handle destructors; 64 actual scheduler registrations and owned-node destructions compare whole objects, nodes, scheduler state and returns.",
         "32,768 encoded-time inputs and invalid-code predicates cover signed remainder, uint32 wrap and integer extremes.",
         "8,192 full update calls cover two contexts, all relevant flags/timers, signed bonus arithmetic, real Enemy lookup, Bomb state, whole Background and 64-animation pool, including NaN/infinite player Y.",
         "8,192 full draws use actual current-profile selection, all 113 card record indices, count thresholds/negative counts, near-full text queues and whole Renderer/Card state.",
         "2,048 complete finish calls compare Card, Player, HUD, whole SaveManager, Background, 64-animation pool, file counters and actual sound queue. Successful/failed captures, replay modes, record clamping and real HUD digit spawning are included.",
         "1,024 complete start calls execute actual child ANM spawning for scripts 4/5, duration assignment, named title-task enqueue, capture-attempt updates and sound requests; whole state, pool and resource counters are compared.",
         "8,192 full post-frame calls compare timer quantization, source clock arithmetic, original 41cb10 with a deterministic QueryPerformanceCounter import, and all eight actual Replay recording/playback record layouts. Integer-overflow seconds exposed and fixed a signed32 conversion error in production source."],
        ["No original instructions are patched. Heap node and proven callback addresses are normalized only for owned scheduler comparisons; other state is exact.",
         "Start tests queue one real title task but do not execute its asynchronous GDI rasterization/completion. Portrait-file entries are -1, so Enemy-owned portrait resource selection is excluded.",
         "Finish tests leave uncaptured records; the all-113-captures achievement endpoint is excluded from this group. The actual Trophy source is now linked and independently exercised below.",
         "Replay access uses actual source getters over isolated valid test storage; whole Replay file IO/lifecycle and platform clock scheduling are outside this group.",
         "Synthetic ANM programs exercise actual child creation and interrupts, not complete original assets or gameplay replays. This is not a whole-game equivalence claim."]
    ),
    "gameplay": (
        "frame_validation.json", ("game_frame_", "game_scene_", "game_draw_"),
        ["16,384 complete original 4ba4a0 calls compare GameController, full Session, HUD, Graphics, input latch, slowdown counter, 64-animation pool, actual fade object/scheduler graph and integer return.",
         "Actual source score easing, counter wrap/clamp, four ANM executors, nullable input, flags, timer boundaries and scene selection execute without original instruction patches.",
         "Both real ScreenInf allocation triggers, including simultaneous creation at exit counter180 and demo frame3540, execute actual constructor, callback registration and destructor. Whole effects and both scheduler node graphs are compared with only allocated pointers, vptr and identified compiled callback addresses normalized.",
         "4,096 direct scene-selector and draw-reset cases compare full Graphics and Sprite prefix bytes and draw return."],
        ["Initial activation at frame0, background transition at frame30, nonnull secondary-background retirement and Pause replay-finish at exit counter120 are not included. Their test-only unresolved boundary functions throw; no successful result is substituted.",
         "ANM inputs use valid synthetic label/stop programs and several timer/freeze states, not full game assets or gameplay replays.",
         "Screen callbacks are registered and their actual owners destroyed; this group does not dispatch their rendering or compare GPU pixels.",
         "This records main-frame state and call composition in a bounded domain, not complete game lifecycle equivalence."]
    ),
    "pause_system": (
        "draw_validation.json", ("pause_",),
        ["8,192 cases execute six original drawing entries and the full text-state reset, comparing the whole PauseInf, Renderer, 25 Replay user records, cached TextJob, PlayerTable, 64-animation pool and integer return.",
         "Name entry, all 25 replay slots, selected-row interpolation, current-profile score ranking, date/stage formatting, registered background-child color and every main-menu routing branch use actual reconstructed drawing and text queue source.",
         "Stone-count hints deliberately use a Context.current_player different from PlayerTable.players[0]; the forced branch exposed and corrected an original source selector error.",
         "Both original and source execute their full CRT calendar conversion against the same actual non-DST host timezone. The native CRT timezone initialization data is supplied; no CRT code is patched.",
         "The CP932 stone hint executes actual dynamic text lookup against an existing zero-frame Job; native mutex slots 18/20 and metadata checksum are initialized."],
        ["GPU rasterization, new dynamic-text Job allocation, asynchronous upload, file IO and the complete Pause owner lifecycle are outside this group.",
         "Date inputs are valid positive timestamps under the host non-DST timezone; DST transitions and invalid-time CRT error handling are not covered.",
         "Replay slot/header objects and SaveManager profiles are isolated valid storage. Their owning file/thread lifecycles are not claimed by these drawing checks.",
         "The source explicit range check for corrupt character/stage table indices does not reproduce native out-of-range reads."]
    ),
    "title_system": (
        "draw_validation.json", ("music_draw_", "stage_select_draw_", "practice_", "title_replay_draw_"),
        ["2,048 full original520c80 Music-page calls compare whole TitleInf, Renderer, metadata and the complete ordered TextJob ownership graph, including every string and object byte with role-based allocation-pointer normalization.",
         "All32 valid track positions, phase/time gates, unlocked titles, actual formatted locked-track numbering, selected colors and all eight comment/warning rows are covered. Repeated fullwidth-space warnings create actual new TextJob clones and run their real native/source destructors.",
         "4,096 complete529b40 Practice-stage draws compare whole TitleInf, Renderer and selected Profile across five difficulties, all six rows, 64-bit record representations, locked/unlocked score formatting, selected blink, negative/extreme timers and near-full320-line queues.",
         "Source and native window scales are explicitly synchronized over five scale values. Both paths run the real ASCII formatting, shadow-line insertion and dynamic cached-text functions.",
         "4,096 full528a50 spell-practice score draws and52cb30 fallback group queries compare whole Title/Renderer/current and fallback profiles, 64-bit scores, signed capture/attempt counts, selected colors, phase gates and all113 valid cards.",
         "2,048 original51fd80 selections compare actual invalid-handle clearing, interrupt2/3 calls and complete64-ANM state. Another2,048 complete51feb0 refreshes compare the real127..133 ANM deletion/spawning, file counters, entire owner, Renderer and six cached CP932 TextJobs, including long/short names, unavailable hints and Extra grouping. The four entries add65,536 checks.",
         "4,096 complete5240d0 replay-page calls add28,672 checks for entire TitleInf/Renderer/100 metadata owners/100 UserHeaders/eight playback StageRecords/five-byte filename scratch/return. They include 25-row pages, null entries, ordinary and spell records, next-stage score/continue-count clamping, final-score fallback, floating Timer interpolation, whole-label tables, five scales and near-full queues.",
         "The16-case standalone replay_format_probe additionally exercises all four actual supplied demo headers on ordinary/custom pages and labeled slowdown overrides. It records14 successful calls and two original access violations with input hashes; this diagnostic is not added to successful equality counts."],
        ["Music inputs start with matching cached zero-frame jobs; duplicate strings exercise real clone creation. New string measurement, GDI rasterization and asynchronous upload are not part of this page group.",
         "The static music comment parser, file worker and menu input/update are outside this draw record and have separate evidence when available.",
         "TextJob heap addresses, internal ownership pointers and allocated string buffers are normalized by ordered role. All remaining object bytes, text content, list order and Renderer state are compared.",
         "This compares queued rendering state, not GPU output or complete Title/Practice gameplay. Spell-practice refresh starts with matching cached jobs and synthetic ANM label/stop programs; actual file IO, new string rasterization and complete practice-page scheduling are excluded.",
         "Spell names use valid NUL-terminated card storage; source checks reject corrupt card/group indices rather than reproducing original invalid reads. The original52cb30 is callee-cleanup RET8 despite the decompiler cdecl label; the test ABI was corrected without modifying original instructions.",
         "The malformed custom-replay format5751c4 has an extra%s. Native demo inputs and finite null-double-word cases match the observed(null)0.0% display. A nonzero low word explicitly raises domain_error instead of repeating original arbitrary-address reads. That undefined argument/pointer domain is not claimed1:1; REPLAY_DRAW_VALIDATION.md and replay_format_probe.json preserve exact evidence.",
         "The initial Music cleanup probe used the wrong caller-cleanup test ABI for46a100; it was corrected to the real callee-cleanup ABI without modifying original instructions. The initial Stage scale-fixture failures are retained in stage_draw_first_failure.json."]
    ),
    "notice_system": (
        "pool_validation.json", ("notice_",),
        ["256 complete NoticeInf constructors and destructors compare whole0x130 objects, actual owning Cursor proxies and global-owner transitions. File slots14/22 are absent and the real unload function executes its no-resource branch.",
         "256 actual4df6e0 cache-hit worker-body calls compare whole Notice owner, both actual heap callback nodes, full scheduler and shared Worker state; both original/source destructors then remove and free the nodes. The worker has no active thread.",
         "4,096 complete original4decf0 frame calls cover main states-1..4, substates0/1/2/4/5/6/7/8, integer timer boundaries, nullable physical input slot2, Cursor select/move, seven ANM handles, resource counters and sound requests.",
         "2,048 forced text-producing cases cycle every one of the25 valid messages and both outline flag values. Real source constant strings and isolated StoneMenu names drive actual midpoint formatting, GDI fonts7/8, raster/Bitmap ownership and deferred upload.",
         "Every uploaded pixel buffer, rectangle and COM call count is compared. The original/source clear_texture calls also compare the complete memory-backed surface, while whole64-ANM state is checked before text completion."],
        ["Original instructions are never patched. The native message table is initialized with independently owned ABI-compatible PMR strings containing the exact reconstructed constants; this does not test the original static initializer/CRT startup.",
         "Substate3 image replacement, actual asynchronous thread launch/file preload and nonempty-file destruction are excluded. The cache-hit worker callback is called synchronously and does not validate scheduling races. The real source image implementation is linked but not called by this group.",
         "COM endpoints are memory-backed observation fixtures, not physical GPU rendering. Fonts come from this Windows host.",
         "Constructor vptr and independently allocated Cursor proxy addresses are normalized. Proxy contents and the rest of the complete owner storage are compared.",
         "Cursor selection is limited to the actual25-entry text table despite the original count99. Out-of-range text access is deliberately not reproduced.",
         "ANM uses valid synthetic label/stop programs. Full game UI scheduling is not established by this module group."]
    ),
    "ending_scene": (
        "pool_validation.json", ("ending_",),
        ["256 full EndingInf constructors and256 complete Script constructors/destructors compare all state bytes, ten real ANM owners, worker close state and resource counters. Only the owner vptr is normalized.",
         "12,288 complete run_script/update_script/Ending frame calls cover every non-IO opcode4..17, unknown terminators, instruction-time gating, integer/timer edges, null physical input slots0/2, interactive waits, difficulty-gated spawning, metadata music unlocks and sound requests.",
         "Opcodes13/14 create and destroy actual ScreenInf mode0/5 objects; full effect, scheduler node graph and return values are compared. The frame test includes the Graphics event0x200 scene override and signed12-frame fast-forward cadence.",
         "8,192 original held-frame helper calls compare all32 valid bit indices and complete held-frame values.",
         "1,024 opcode3 programs compare encrypted CP932 text/underscore decoding, ordinary and ruby lines, wrap across five text slots, actual queued std::function closures and captured live Script colors. Two queued lines may be separated by a color opcode, and colors are changed again before dispatch.",
         "Real GDI fonts4/18, both outline flag values, outline/raster Bitmap code, actual deferred upload and interrupt2 completion execute. Every upload pixel buffer, rectangle, COM count and complete64-ANM state before and after completion is compared."],
        ["Resource initialization, opcode7 asynchronous file loading and opcode12 credits-file replacement are explicit throwing test boundaries and excluded. Opcode12 gallery early return is covered; actual achievement/credits replacement is not.",
         "Script worker construction/destruction is tested with no active thread. Full EndingInf owned destructor/file unload and initialization metadata/IO are not covered.",
         "COM surfaces are memory-backed observation fixtures; upload pixels are compared, not a physical GPU display. Fonts come from the current Windows GDI host.",
         "ANM inputs are valid synthetic label/stop programs. This is not an end-to-end original ending replay.",
         "held_frames is tested with a valid object and indices0..31; nullable physical input selection is tested through the VM/frame caller.",
         "The former659 sound differences are retained in pool_first_failure.json and corrected by restoring the original dummy command string; the constructor now correctly selects Graphics.surface_animation rather than Renderer.animation_file."]
    ),
    "trophy_system": (
        "pool_validation.json", ("trophy_",),
        ["256 complete TrophyInf constructors, initialization and owned-node destructors compare full owner, queue, scheduler topology and actual PMR allocation/deallocation sequence.",
         "2,048 full update calls exercise states -1 through3, empty/nonempty queues, age thresholds, variable/null timer rate, three actual named ANM spawns, sound requests and real state2 self-destruction.",
         "Both original and source execute real deferred text tasks, CP932 decoding, GDI font rasterization, outline generation, Bitmap ownership and actual upload loops. Every submitted pixel buffer, rectangle and COM call count is compared against a memory-backed surface fixture.",
         "Both outline flag values, ASCII/Japanese/empty titles and the exact original Japanese quotation-mark format are included. Whole64-animation pool, resource counters, scheduler graph, owner/queue contents and PMR event order are checked.",
         "The actual load_animation_file cache-hit path runs during initialization; original executable instructions and CRT/GDI functions are not patched."],
        ["Texture and surface COM endpoints are observation fixtures, so this is an upload-buffer comparison rather than a physical GPU display or driver rendering test.",
         "The animation file is already loaded and contains bounded label/stop programs. Resource preload/file IO, full original animation assets and whole-game scheduling remain outside this group.",
         "Real GDI uses a shared host font handle and native Win32 imports. Other installed fonts, Windows versions and concurrent renderer execution are not covered.",
         "Independently allocated owner, queue map/block/proxy and scheduler pointers are normalized by role. Every defined state field and allocation event remains compared; vptr and verified compiled callback addresses are normalized.",
         "This owner/frame group is separate from the previously recorded queue/codec tests; their counts must not be added twice."]
    ),
    "bomb_system": (
        "cancellation_validation.json", ("marisa_child_cancel_", "reimu_active_cancel_"),
        ["512 Marisa update samples with real script-57 child, full parent position/rotation chain, 32 actual Bullet objects and a real empty Laser controller.",
         "512 Reimu update samples executing the real every-eight-frames circle cancellation against 32 Bullet objects and a real empty Laser controller.",
         "Full Bomb, player, Bullet/Laser controller, 64-animation pool and return values compared."],
        ["Concrete nonempty Laser beam cancellation and Item spawning remain outside this group.",
         "Other Bomb constructor/start/damage/orb evidence remains in the earlier character and core validation records; counts here do not include those."]
    ),
}
for module, (filename, prefixes, coverage, limitations) in modules.items():
    comparisons = {name: value for name, value in report["comparisons"].items() if name.startswith(prefixes)}
    if not comparisons:
        raise SystemExit(f"No comparisons for {module}")
    result = {
        "status": "passed", "failed": 0, "total": sum(comparisons.values()),
        "original_cpu_checks": sum(count for name, count in comparisons.items() if "_source_only_" not in name),
        "source_only_checks": sum(count for name, count in comparisons.items() if "_source_only_" in name),
        "original_sha256": report["original_sha256"],
        "shared_report": "source_reconstruction/sprite_renderer/pool_cpu_validation.json",
        "shared_report_sha256": hashlib.sha256(report_path.read_bytes()).hexdigest(),
        "shared_report_total": report["total"], "comparisons": comparisons,
        "coverage": coverage, "limitations": limitations,
        "bound_source_hashes": hashes,
    }
    destination = root / module / filename
    destination.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(f"{module}: {result['total']} checks; all {len(hashes)} source hashes match")
