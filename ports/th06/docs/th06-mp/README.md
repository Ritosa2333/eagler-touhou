# TH06 Multiplayer - START HERE

> Status: new TH06 multiplayer line. This document set is intentionally separate from every TH07 multiplayer document.
>
> Primary engineering upstream: the current tested TH07 Eagler multiplayer implementation in `../th07-eagler`, including its dirty working-tree fixes. The rejected historical TH06 multiplayer branch/progress is **not** an upstream and must not be used as a gameplay or architecture oracle.

## 0. User decision - binding

The previous TH06 multiplayer upstream (`mp-source/th06-coop` / SnowStair LCConnect) and the existing TH06 `TH_ENABLE_MULTIPLAYER` implementation are rejected. Treat all of that multiplayer-specific work as disposable legacy contamination.

Do **not** continue `src/MultiplayerRuntime.*`. Do **not** preserve a behavior merely because the old TH06 multiplayer code implemented it. Do **not** port from the old TH06 multiplayer build directories.

For network, rollback, session lifecycle, browser transport, prediction, time synchronization, touch ownership, Replay extension, presentation smoothing and regression strategy, treat our current TH07MP code as the main engineering upstream and read the code itself in detail.

TH06 original/portable source remains authoritative only for TH06-specific game ownership and semantics: Player/Enemy/Bullet/Item/Bomb/Stage/Replay structures, original lifecycle, resource layout, original replay format and the 60 Hz calc-chain order. Adapt the proven TH07MP system to those owners rather than importing TH07 gameplay constants.

## 1. Current source anchors

At creation of this document set:

- TH06 repository: `D:/workspace/eagler/th06-eagler`
- TH06 HEAD: `c571bca4da18c591aba4ab0ab8ff8b8dadef503c`
- TH06 branch: `eagler`
- TH06 working tree is intentionally dirty and currently still contains the rejected old multiplayer implementation.
- TH07 repository: `D:/workspace/eagler/th07-eagler`
- TH07 HEAD: `a193b4cbd952e71689a064ddb6a449cfd33c1e2b`
- TH07 branch: `eagler`
- TH07 working tree is intentionally dirty. The **working-tree source**, not HEAD alone, is the reference because recent accepted/focused fixes remain uncommitted.

Never use `git reset`, `git checkout`, `git restore` or `git clean` to obtain a multiplayer baseline. Old TH06 multiplayer hunks are interleaved with valid Eagler, touch, thprac, replay and runtime changes.

## 2. First task before new implementation

Create a semantic purge ledger of the rejected TH06 multiplayer layer and remove only those hunks/files.

Known rejected markers include at least:

- `TH_ENABLE_MULTIPLAYER`
- `src/MultiplayerRuntime.cpp`
- `src/MultiplayerRuntime.hpp`
- `scripts/test-multiplayer-source-contract.mjs`
- old `g_Player2` / player-2-only branches introduced by that line
- old P2 button-bit widening in `Controller.*`
- old multiplayer ANM slots/offsets in `AnmIdx.hpp`
- old `character2` / `shotType2` fields and menu hooks
- old `provokedPlayer` additions and old nearest-player wrappers where they exist only for the rejected implementation
- old multiplayer build caches are evidence of the rejected line, not source-of-truth

Before removing a mixed hunk, compare it with the current ordinary TH06 owner and current Eagler/touch/replay changes. Do not delete unrelated fixes just because the file also contains multiplayer code.

A clean starting point means: no rejected `MultiplayerRuntime` architecture remains, while current ordinary TH06 Eagler behavior still builds and its existing touch/replay/thprac fixes remain present.

## 3. Mandatory reading order

Read code before writing new TH06MP code.

1. `docs/th06-mp/TH07-CODE-MAP.md`
2. `docs/th06-mp/PITFALLS.md`
3. `docs/th06-mp/PORT-LEDGER.md`
4. `../th07-eagler/src/netplay/` - read all production files, not just headers.
5. `../th07-eagler/src/multiplayer/GameplaySession.*`
6. TH07 integration owners named in `TH07-CODE-MAP.md` (`GameWindow`, `Player`, `Controller`, `ReplayManager`, `ReplayExtension`, `ResultScreen`, `EffectManager`, etc.).
7. TH07 focused tests named in the code map. Tests are part of the upstream contract.
8. `../th07-eagler/docs/th06-th07-multiplayer-feature-rebase-notes.md`
9. `../th07-eagler/docs/th07-sbrik-feature-rebase-handoff.md`
10. `../th07-eagler/docs/th07-sbrik-feature-rebase-ledger.md` - use mainly to discover owner coverage and test discipline, not to copy TH07 gameplay constants.
11. `../th07-eagler/docs/th07-public-network-deploy-handoff.md` - current RTC/TURN/relay deployment behavior and known transport races.
12. `../docs/replay-porting-guide.md`
13. `../docs/replay-determinism-audit.md`
14. `PORTING.md` in this TH06 repository.
15. History entries listed below.

### History entries that matter

- `../docs/history-session/31.md`: most important recent TH07MP runtime incidents - Restart generation, control-channel flood, O(1) receive queue, Replay v1 multiplayer subtype, touch Replay lifecycle, ANM alpha endpoint bug, menu ANM slot collision, local locator, and the final decision to remove decorative Stage rollback overhead.
- `../docs/history-session/17.md`: mandatory TH06 interpolation accident record. A broad TH06 ANM presentation interpolation of scale/rotation/UV/color caused severe real-device problems and was fully reverted; the reverted state was user-confirmed OK.
- `../docs/history-session/29.md` / `30.md`: public-network/WebRTC and surrounding integration history when more detail is needed.
- `../docs/history-session/33.md`: later local verification snapshot, but note that its Stage-pointee rollback statement was subsequently superseded by the later tail of history-session 31: decorative 3D Stage rollback was removed by user choice to avoid extra rollback workload.

Do not assume the numerically largest history file is chronologically authoritative for every detail. Compare timestamps/checkpoints when statements conflict.

## 4. What may be copied almost verbatim vs adapted

The goal is maximal reuse of tested code, not a conceptual rewrite.

### Strong reuse candidates

Start by copying/renaming from TH07 and preserving behavior/tests wherever type dependencies permit:

- `NetplayProtocol.*`
- `NetplayCore.*`
- `NetplaySession.*`
- `NetplayInput.*` architecture
- `NetplaySideEffects.*`
- `RollbackJournal.*`
- `BrowserPeerTransport.*`
- `WebSocketTransport.*`
- relay/signaling protocol behavior from `th07-eagler/tools/netplay/lan-relay.cjs`
- input redundancy / prediction semantics
- Restart session-generation model
- frame-zero exact-input barrier
- scheduler-only time-sync behavior
- bounded post-stall catch-up
- browser diagnostics and transport failure behavior

Do not casually simplify these just because TH06 has fewer gameplay features. Most bugs fixed here were browser/network/lifecycle bugs independent of Touhou 7 gameplay.

### Must be TH06-adapted

- rollback state owner list and every `Touch*` mutation hook
- player arrays/resources/loadout ownership
- enemy targeting/damage semantics
- bullet/laser ownership
- item/drop/resource rules
- Bomb rules
- GUI/HUD layout
- stage/retry/game-over lifecycle
- ANM file slot allocation and resource offsets
- Replay base-file integration
- result/score persistence rules
- any TH07 Cherry/Border/Spirit/Stage-4-specific behavior: these are not TH06 semantics

## 5. Required project structure

Do not recreate the old flat `MultiplayerRuntime` design. Prefer the same separation as TH07:

- `src/netplay/` - transport-neutral protocol/core, rollback/session/transport and TH06 driver/state adapter
- `src/multiplayer/` - TH06 multiplayer gameplay/session sidecars
- narrow hooks in original TH06 owners
- compile isolation similar to TH07: ordinary build must not depend on active netplay state

### Ordinary vs multiplayer binaries

The user explicitly requires the same publication split as TH07: build and publish an ordinary TH06 runtime and a separate TH06 multiplayer/netplay runtime. Do not turn the ordinary executable into a fat binary that contains the netplay stack and is merely disabled by a runtime flag.

Current build switches:

- ordinary TH06: `TH_ENABLE_NETPLAY=OFF`, `TH_ENABLE_MULTIPLAYER_GAMEPLAY=OFF`;
- gameplay-only multiplayer audit builds may enable `TH_ENABLE_MULTIPLAYER_GAMEPLAY` without transport;
- full TH06MP enables `TH_ENABLE_NETPLAY`, which also exposes the multiplayer gameplay compile contract.

This isolation is intentional regression protection for the already-verified ordinary TH06 touch, ReplayX, thprac, rendering and audio paths.

Use new TH06-specific names (`Th06...`) where a file contains game-specific ownership. Keep generic files generic if copied without TH07 dependencies.

## 6. Definition of progress

Do not call a feature complete because it compiles. For each work class, record in `PORT-LEDGER.md`:

- TH07 reference files/functions/tests actually read
- TH06 owner and semantic differences
- copied behavior vs intentional adaptation
- rollback/presentation/touch impact
- ordinary-build regression evidence
- multiplayer focused evidence
- real-device/human evidence still missing

The user values real-device behavior over green compile output. Never turn a build PASS into a human-acceptance claim.

## 7. Safety / scope

- Preserve current TH06 float draw, atlas edge-extrusion, mobile VBO performance path, audio, touch, ReplayX and thprac work.
- Rendering/presentation additions must remain out of deterministic simulation unless they truly alter gameplay.
- Decorative state does not need rollback merely to look identical if doing so creates measurable work; the user explicitly prefers allowing trivial decorative desync over extra rollback burden.
- No hash-based room admission, compatibility rejection, automatic repair or resync behavior may be added without asking the user. Existing diagnostic hashing is not authorization for new hash gates.
- Do not commit, push or deploy unless separately authorized.
