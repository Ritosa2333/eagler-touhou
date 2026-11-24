# TH128 runtime compatibility ledger

This is an implementation/evidence ledger, not a full-port completion claim.
The actual source files and generated build manifest define the current build.

| Area | Reuse/adaptation | Evidence and boundary |
| --- | --- | --- |
| Original data | No TH12 data reused | Japanese and Chinese TH128 DATs are independently prepared; 199 unique files per variant. |
| ANM v7 structure | Compatible bounded reader | All 53 Japanese banks load in Python and C++; independent thtk entry/sprite/script counts agree. |
| ANM VM/Scene | MIT generic module adapted to `th128::anm` | Preserved source license; hardened offsets, arguments, budgets, exceptions/diagnostic boundaries. Most execution/scheduler behavior is inherited, not proven TH128 equivalence. |
| ANM 111 | Native handler flag store studied | Original-byte 24-case flag-write evidence; full external draw/update contract not recovered. Still stops explicitly. |
| ANM 113/114 | Not reused/approximated | Native angular-track initialization and geometry allocation investigated; complete update/ownership/rendering remain unsupported. |
| RNG | Independently implemented from TH128 bytes | Original helpers `0x46a3f0` / `0x46a460`; 514 bounded emulator result/seed/counter transitions, plus C++/WASM vectors. No native Windows CPU trace. |
| WebGL rectangle rendering | Independent renderer with explicit ABI | Geometry modes 0/1, anchors, ARGB, UVs and blend modes; unsupported modes/3D rotation stop. Transform/layer/viewport details are not native screenshot-verified. |
| Title construction | TH128 instruction/data evidence | Bank 24 title and 25 version; scripts 97/101, then 0/0; original script children supply actual graphics. Timing/IRQs and enabled-option flow still need complete original verification. |
| ECL program | Independent bounded C++ reader | All 33 real ECLs: 1,459 subroutines and 49,097 indexed instructions. Loading them does not imply gameplay execution. |
| ECL control VM | Bounded typed control core | Numeric/control/host-interface semantics have synthetic tests; gameplay callbacks remain explicit host requirements. Actual Normal root stops at unregistered opcode 402, before 22. |
| Freeze scalar rules | Independently implemented from native evidence | Explicit raw units, gates, cap/cost, duration, float reward branches. Complete propagation and lifecycle are not implemented. |
| Replay resource codec | MIT format/cipher interpretation adapted | Python/browser bounded decoders preserve actual demo bytes, compare recompression/edits across implementations. Original game loading/playback not verified. |
| Stage resource mapping | Extracted from TH128 table | 16 actual table entries, including cross-named backgrounds; no filename-based guess, no dynamic route-progression proof. |
| TH12 UFO/players/score/stages/saves | Rejected | Not copied into TH128 gameplay. |
| Full gameplay/flow | Not implemented | No first-stage acceptance, no complete route, Extra, endings, score saves or native replay simulation. |

## First-stage integration update

`Program::linkIncludes` now explicitly links loaded dependency closures and
preserves snapshots; typed negative-reference providers keep unknown values as
faults. All 16 actual stage include graphs pass. An independent graph-depth
review has a regression for order-independent longest paths.

`StageWorld` uses the actual A1-1 Normal scripts and demo0 seed 34772, creates
the real first GirlA1b at control tick100, records its native animation request
and emitter configuration, retains raw509 extension words and stops at first
unimplemented emission501offset816 at control tick114. This is not continuous
enemy-motion simulation or a completed bullet spawn: snapshots explicitly mark
positionIntegrated:false. The earlier raw control export still stops at402 by
design, and remains a separate diagnostic.

Independent player/shot, enemy and emitter pure components have native-byte
fixtures, including the TH128 enemy1/100 coordinate quantum, actual Cirno raw
speeds/bounds and first-count1 emitter data. Their local proof does not establish
full constructor/frame scheduling, animation metrics, world manager state,
bullet/ETEX lifetime, shooting/hit/freeze/score behavior or first-stage play.

## Toolchain and build boundary

Emscripten 4.0.22 is installed entirely in workspace `work/tools`, with its
bundled Python 3.13.3. Activation is local, not `--permanent` or `--system`.
The prebuilt SDK cache had a Windows path-normalization/readonly-cache clear
failure; the project now uses a fresh isolated `work/emscripten-cache-4.0.22`
with normal sanity checks and compiler concurrency limited to two workers.
No sanity-check bypass or system-security change is used.

`tools/build-runtime.mjs` compiles C++20 into ES-module WASM artifacts, tracks
all source/header/license hashes and both output artifacts, and labels the
result M1-in-progress with `gameplayImplemented: false`. This build does not
launch the original executable or require original assets at compilation.

`site/play.html` is an animation/control diagnostic host, not a playable game
entry. The M0 inspector remains available separately at `/`. Runtime routes
serve only allowed original game resource types, never EXE/DAT/config/scores.
Browser data import/upload does not publish the user's content to a server.
