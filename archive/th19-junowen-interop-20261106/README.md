# Ju.N.Owen static interoperability audit

Scope: no game process launched. Reference commit534f26466c10a5ebab1fac222b1a410b6b053424; original EXE SHA2568918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9.

The offline codec in ports/th19/tools/replay/junowen-format.mjs implements the upstream17-byte writer layout, both16-bit seat streams, and all four matchup variants. Input mapping preserves bits0..7 and maps web pause1024 to native pause256. Unknown input flags fail explicitly. Native records have no magic, checksum or engine build field; decoding their layout is not proof of provenance or deterministic compatibility.

Correction to earlier investigation: upstream names rand_seed1/2 refer to RVA1c6624 and1c6634. For this EXE these are current state(+4) and shadow(+0x14) of the same object at5c6620. 402540 computes state*48271 mod2147483647, stores both state and shadow, and returns state modulo object+0x10. A separate object starts at5c6638. Do not label the upstream two header words as two independent random streams. 550dd0 restores native replay seed by normalizing the shadow modulo2147483647, substituting1 for zero.

Remaining engine alignment: web ANM random uses an independent LCG; per-world wave/bullet/spirit random streams are not yet tied to the native object's full call order. Changing the RNG formula alone cannot guarantee playback parity. The web transport records a monotonic match-frame counter; Ju.N.Owen indexes by RoundFrame.frame. Round transitions must be audited before combining multi-round timelines. The web played flag is cumulative (whether any gameplay has occurred), not a per-row combat/round marker, so filtering rows by played cannot establish native timing.

Validation: offline tests cover byte-exact decode/encode,17-byte truncation boundaries, incomplete input rows, all four matchup layouts,512 key/pause mappings, and a known Park-Miller reference sequence. The codec is not connected to the production replay UI and does not claim old web recordings are native-playable. No game/CI runtime deployment changed in this audit.

## Native clock evidence

The GameInf update at 0x516450 calls the timer at 0x4026c0 from 0x516cad with ECX=GameInf+0x10. That timer copies current(+4) into previous(+0), then advances the integer/floating clock according to the speed pointer at 0x596da0. Thus GameInf+0x10/+0x14 are previous/current clock values. The raw instructions are preserved in clock-disassembly.txt; the float storage in the decompiled C has imperfect inferred types and must be read against the instructions.

The call is conditional: loading bit4, flag0x10, flags0x60, the countdown branches for flags0x40000 and0x10000, and several early returns bypass it. The function also returns before it when the object at global0x5d1c10 has a nonzero +0x20 and flag0x10 is absent. These are observed branch conditions, not a complete naming of every game state.

At 0x515a10, when GameInf flags include0x1000, the function clears that flag, resets several gameplay objects, and writes current=0, floating clock=0, previous=0xffffffff (0x515d3f–0x515d4d). Its caller invokes it when current==30. This proves that the clock can reset within the existing GameInf lifetime; identifying every round transition requires further state tracing. It does not justify assuming each network row maps to one native tick.

## File inspection

Run `node ports/th19/tools/replay/inspect-replay.mjs FILE` for a web recording, or add `--format junowen` for the signatureless native tool format. The command reads the file, validates its contents, and emits JSON without launching a game or writing the input. Web reports identify missing native RNG/clock data, direct movement, and unsupported input bits. Ju.N.Owen reports include packed settings and a byte-exact re-encode check. The 600-row idle fixture passes that check; it is a synthetic format fixture, not captured gameplay.

Upstream source inconsistency: read_header_from_memory selects a single-seat vector for CpuVsCpu, but read_from_reader selects two seats for that same matchup. An even-sized single-seat payload can be misread without a length error. The inspector reports this ambiguity instead of choosing a layout from payload size. Existing codec roundtrip tests follow the reader layout; they do not establish recorder compatibility for this matchup.

Validation command: `node --test ports/th19/tests/replay-interop-inspect.test.mjs ports/th19/tests/junowen-format.test.mjs ports/th19/tests/recording.test.mjs` — 17 tests pass. No native game/player was launched during this continuation; no GitHub or deployed runtime was changed.

## Isolated original-instruction comparison

`ports/th19/tools/replay/audit-native-primitives.py` reads the hash-verified EXE and runs only two selected machine-code ranges in Unicorn 2.1.4: 0x4026c0–0x402751 (timer) and 0x402540–0x402616 (RNG). It does not start the EXE, execute its entry point, load imports, or access game saves. RNG mutex calls are substituted with successful no-ops. Each invocation must return to the sentinel within its instruction budget.

`native-primitives.json` contains 120 timer cases and 156 RNG cases produced by those instructions. `ports/th19/tests/native-primitives.test.mjs` compares the JavaScript models against these outputs. Cases include exact float32 speed thresholds0.99/1.01, a null speed pointer, fractional/repeated/skipped frame indices, selector normalization, integer wrap/overflow, uint32 RNG boundary states and four output ranges. All276 vectors match. This validates primitive arithmetic; scheduler timing, random call order, and whole-game equivalence remain separate requirements.

The timer's approximate-one fast path increments the integer frame while accumulating a float32 value. Other speeds calculate the integer using CVTTSS2SI on the accumulated float32 value. At speed0.5 the first four frame values are0,1,1,2. `native-timer.mjs` preserves this behavior, including integer-indefinite0x80000000 on conversion overflow.

## Round transition and live protocol

The object referenced by0x5d1a38 has BackGroundInf's vtable (see th19-round-transition-native.c). The reset in0x515a10 is gated by the background-related flag0x1000; it should not be named a universal next-round reset.

The current Ju.N.Owen event dispatcher hooks RVA0x132cf0+0x29f=VA0x532f8f. At that call site the EXE invokes0x516cf0 before creating a new GameInf through0x516d60 in transition case0x16. Ju.N.Owen calls its original function first, then on_round_over. GameInf construction initializes its clock to zero, so the object's lifetime and the index can restart while a larger match continues.

The live code in junowen/src/state/battle_session/utils.rs exchanges four u32 fields per RoundInitial; spectator_game.rs restores all four on round-over. Their addresses are0x5c6624/0x5c6634 (current/shadow of the first RNG object) and0x5c663c/0x5c664c (current/shadow of the second). The live battle/spectator implementations explicitly zero player inputs and avoid dequeuing at frame<1. Their comment discusses initialization repetitions; the actual condition is frame<1 on a u32, not a test for0xffffffff.

The archived replay tool built in the CI has a different contract: one17-byte header stores the first two fields, and its recorder's put() overwrites vec[frame] for any existing index. It has no round record or four-field synchronization. The inspector now exposes that structure and flags multiple-round settings. Format parsing and primitive arithmetic tests cannot recover previously overwritten inputs from an old file or infer absent RNG states.

## ANM random arithmetic and rollback implementation

NativeRandom.hpp now implements the default0xffff00 range, integer remainder, unsigned normalization `float(draw)/(float(range)-1)`, and signed normalization `float(draw)/(float(range)/2-1)-1`, with multiplication by the caller's scale after normalization. The audit emulator additionally executes0x404bd0–0x404c3c and0x404c90–0x404ce9 and records234 float outputs as raw bits. The compiled C++ test checks those bits through both NativeRandom and AnmVm; this is independent original-machine-code evidence rather than expected values generated by the new C++ formula.

AnmVm removes its1664525/1013904223 LCG for these draws. The native jump table gives10010/11/12 the second object's stream and10030/31/32 the first object's stream; the two ANM streams now advance independently. WorldCheckpoint captures and restores both. Float10022 returns the raw ranged draw; integer10022 uses bounded remainder and consumes no draw when its bound is zero. Integer reads of float-only random variables pass the literal ID through without consuming a random value, as the native integer dispatch table specifies.10038 is outside the native variable interval and likewise no longer draws a random value. The address-level dispatch evidence is in anm-rng-dispatch.txt.

Validation: `node ports/th19/tests/anm-native-random.mjs` compiles the production ANM source and checks234 bit patterns per stream, stream independence, rollback reproducibility and bounded draws. `node ports/th19/tests/world-limits.mjs` checks snapshot construction/recapture/restoration of both random states alongside the existing capacity tests. The20 JavaScript replay/primitive tests pass. The same C++ source serves normal and multiplayer modes.

Integration boundary: these are the ANM consumers. ECL, bullets, waves and other consumers still have separately owned web streams. Initializing both ANM states from the web match seed preserves web determinism but does not reconstruct two original RNG objects or their full shared call order. Whole-game native replay compatibility is not established by this change. Existing recordings remain build-specific; changing this random sequence requires the new runtime build identity.
