# TH06 Multiplayer port ledger

> Live ledger for the new TH06MP line. This file belongs only to TH06MP. Do not merge it into the TH07 sbrik ledger.
>
> Engineering upstream = current `../th07-eagler` working tree. Rejected historical TH06 multiplayer code is not upstream.

## Status vocabulary

Use only these labels:

- `TODO` - not audited/implemented.
- `READ` - TH07 reference and TH06 owner have both been read, but no implementation claim.
- `PURGED` - rejected old TH06 multiplayer hunk/file removed without disturbing unrelated current code.
- `COPIED` - generic TH07 implementation copied with only naming/include/build adaptation.
- `ADAPTED` - behavior ported but TH06 ownership/layout required deliberate changes; record them.
- `EQUIVALENT` - current TH06 code already satisfies the TH07 contract; cite exact function/test evidence.
- `EXCLUDED` - intentionally not applicable; give a concrete reason.
- `BUILD-PASS` - build evidence only.
- `FOCUSED-PASS` - focused automated behavior/contract evidence.
- `HUMAN-PASS` - real person/device acceptance actually performed.

Never write `PASS` without the evidence class.

---

# Phase A - purge rejected historical TH06 multiplayer layer

Goal: ordinary/current TH06 Eagler baseline with no legacy `MultiplayerRuntime` architecture left, while preserving unrelated touch/replay/thprac/render/audio/platform work.

| Owner / marker | Current old-MP evidence | Action | Status | Evidence / notes |
| --- | --- | --- | --- | --- |
| `CMakeLists.txt` | `TH_ENABLE_MULTIPLAYER` archived local 2P option | remove only rejected old option/source/defines; later add new netplay separation modeled on TH07 | PURGED | old option/source/define removed; new `TH_ENABLE_NETPLAY` / `TH_ENABLE_MULTIPLAYER_GAMEPLAY` are OFF by default and append sources only in MP build trees |
| `src/MultiplayerRuntime.cpp/.hpp` | flat P1/P2 helper runtime | delete rejected files after consumers are surgically removed | PURGED | files deleted after all consumers were removed; no flat replacement introduced |
| `scripts/test-multiplayer-source-contract.mjs` | contract for rejected implementation | delete after legacy purge | PURGED | rejected source contract deleted; new tests live under `tests/` |
| `Controller.cpp/.hpp` | P2-specific button widening / sampling | remove legacy P2 physical lane only; preserve current ordinary/touch input code | PURGED | restored ordinary `u16` input; new MP capture enters `Netplay::Input::ResolveLocal()` only under `TH_ENABLE_MULTIPLAYER_GAMEPLAY` |
| `AnmIdx.hpp` | old P2 slots/offsets | remove legacy assumptions; later re-audit real TH06 ANM occupied child ranges | PURGED | all old P2 file/script offsets removed; no replacement slot chosen yet |
| `Player.cpp/.hpp` | `g_Player2` and legacy per-player logic | remove legacy-only branches but preserve current Eagler/touch/replay work | PURGED | `g_Player2`, `playerType`, P2 scripts/buttons/resource branches removed; post-purge file returned to current non-MP baseline |
| `GameManager.cpp/.hpp` | `character2`, `shotType2`, legacy startup | remove legacy fields/hooks | PURGED | legacy second-loadout fields/helpers removed |
| `MainMenu.cpp` | legacy P2 auto character/shot selection | remove | PURGED | ordinary menu/replay shottype behavior restored |
| `Enemy.hpp`, `EclManager*`, `EnemyEclInstr.cpp`, `EnemyManager.cpp` | legacy `provokedPlayer` / nearest-player behavior | remove old-MP assumptions, retaining unrelated Eagler fixes | PURGED | old provoked/nearest-player owner removed completely; new targeting remains deliberate future TH06 design |
| `BulletManager.cpp/.hpp` | legacy P2 targeting/collision | remove old-MP-only branches | PURGED | old P2 graze/hit/laser/provoked code removed |
| `ItemManager.cpp` | legacy P2 pickup/auto-collect | remove old-MP-only branches | PURGED | ordinary TH06 pickup/auto-collect restored |
| `BombData.cpp` | P2 script offset helper | remove old legacy mapping | PURGED | old P2 ANM-script remapping removed |
| `Gui.cpp` | old P2 HUD/resource release hooks | remove old legacy mapping | PURGED | old P2 face-bank load/release removed |
| `ReplayData.hpp`, `ReplayManager.cpp`, `ResultScreen.cpp` | old MP replay/result assumptions | remove only rejected MP parts; preserve current ReplayX/touch fixes | PURGED | original 16-bit replay input/shottype semantics restored; current ReplayX/touch lifecycle retained |
| `PracticeRuntime.cpp`, `Touch.cpp/.hpp` | may contain both current valid fixes and old MP hooks | do not bulk revert | PURGED | only old widened input type was removed from PracticeRuntime; current run-level touch/replay resets were retained |
| old `build-*multiplayer*` caches | generated from rejected code | never use as source/evidence | EXCLUDED | can remain on disk; do not infer current contracts from them |

### Phase A exit gate

- [x] no source consumer of `MultiplayerRuntime` remains.
- [x] no rejected P2-only physical-input widening remains.
- [x] no old TH06 MP ANM slot assumption remains.
- [x] ordinary TH06 build still compiles. (`build-web-ordinary-audit` official Web link, 2026-08-29, BUILD-PASS.)
- [x] current TH06 touch/replay/thprac focused checks still pass where relevant. (`test-thprac-source-contract`, `test-thcrap-source-contract`, time-stop, surface-copy; ordinary Emscripten syntax compile PASS.)
- [x] source diff review confirms valid current Eagler platform/render/audio changes were not reverted.

Build note: the earlier MCP limitation is no longer the latest evidence. A current official ordinary Emscripten/Ninja Web link now passes from `build-web-ordinary-audit`. `audit/historical/obsolete-bullet-render-path-contract.mjs` is still an obsolete pre-float-Draw contract which demands `rintf`; it must not be satisfied by reintroducing the user-rejected pixel quantization.

---

# Phase B - copy generic TH07 netplay foundation

Read the complete TH07 implementation before marking anything below.

| TH07 reference | TH06 target | Expected disposition | Status | TH06 differences / evidence |
| --- | --- | --- | --- | --- |
| `src/netplay/NetplayProtocol.*` | `src/netplay/NetplayProtocol.*` | copy nearly verbatim; TH06 game identity is outside generic packet code | ADAPTED | code copied; wire magic intentionally `E6NP`; 32-frame redundancy and separate `senderFrame` preserved; core round-trip FOCUSED-PASS |
| `src/netplay/NetplayCore.*` | same | copy nearly verbatim | COPIED | current TH07 algorithm copied; prediction/rollback/loss/reorder/3P/direct-touch focused suite FOCUSED-PASS |
| `src/netplay/NetplaySession.*` | same | copy nearly verbatim | COPIED | current gate copied; TH06 test uses gameId 6; HELLO-before-READY/mismatch contracts FOCUSED-PASS |
| `src/netplay/NetplayInput.*` | same | copy architecture, adapt TH06 input globals/types | ADAPTED | TH06 adds compile-isolated 3-lane `g_Cur/LastFrameGameInputs`; ordinary `g_Cur/LastFrameInput` stays unchanged; input-lane FOCUSED-PASS |
| `src/netplay/NetplaySideEffects.*` | same | copy | ADAPTED | implementation copied; TH06 audio/MIDI/replay persistence call sites suppress speculative rollback resimulation; MP Emscripten integration compile and final Web link pass |
| `src/netplay/RollbackJournal.*` | same | copy current optimized implementation | COPIED | current two-logical-frame first-write journal copied; second-frame-only touch/restore and eviction tests FOCUSED-PASS |
| `src/netplay/BrowserPeerTransport.*` | same/generic rename only if needed | copy current working-tree implementation | ADAPTED | current dirty TH07 working-tree file copied; only client identity changed to TH06. Shared relay targeted-envelope marker stays `0xe7`; route-race/O(1) queue/ICE recovery contract FOCUSED-PASS |
| `src/netplay/WebSocketTransport.*` | same | copy | COPIED | current fallback/test transport copied unchanged; Emscripten syntax compile PASS |

### Phase B focused tests to port first

- [x] `netplay-core-test.cpp`
- [x] `netplay-input-test.cpp`
- [x] protocol bad-version/round-trip tests
- [x] prediction correction / rollback-budget / duplicate / conflict coverage
- [x] 32-frame redundancy coverage
- [x] direct-touch first-missing-frame-only prediction coverage
- [x] scheduled-input retry cannot call physical sampler (`tests/netplay-production-driver-contract-test.py`, FOCUSED-PASS)
- [x] browser receive queue contract rejects per-packet `Array.shift()`
- [x] control HELLO/READY retry is rate limited (production driver keeps the TH07 15-tick/250ms control cadence; production-driver source contract + inherited transport/core tests, FOCUSED-PASS)

Do not wait for full TH06 gameplay before establishing these generic contracts.

---

# Phase C - TH06 gameplay/session ownership

Create a TH06-specific `src/multiplayer/` layer. Do not copy TH07 gameplay constants.

| Work class | TH07 code to read | TH06 owners to audit | Status | Explicit TH06 decisions required |
| --- | --- | --- | --- | --- |
| stable participant/session slots | `multiplayer/GameplaySession.*` | `GameManager`, `Player`, launcher/runtime options | ADAPTED | `TH06_MULTI_MAX_PLAYERS=3`, stable local slot and lifecycle copied; descriptor restricted to Reimu/Marisa + A/B; TH07 Stage4/Cherry/Border/Spirit fields excluded; focused session test PASS |
| player instance lifecycle | `Player.*`, TH07 ledger player rows | TH06 `Player.*`, GameManager chain registration/destruction | ADAPTED | MP build owns stable `g_Players[3]` + active mask; P1 remains compatibility owner, fresh GameManager registration adds negotiated P2/P3, stage reinit preserves participant resources; ordinary build keeps the original singleton |
| independent input lanes | `NetplayInput.*`, `Controller.*`, `Player.*` | TH06 `Controller.*`, `Player.*`, Supervisor input globals | ADAPTED | synchronized `FrameInput` lanes drive each stable Player; legacy/global consumers receive deterministic union only; Player movement/focus/Bomb consume their own slot; frame-zero/retry source contracts FOCUSED-PASS |
| raw touch ownership | TH07 Player/Touch helpers | TH06 `Touch.*`, `Player.*` | ADAPTED | only negotiated local stable slot can sample physical joystick/direct-touch when no replay override exists; remote Players consume synchronized analog/touch lanes; one scheduled sample per logical frame is enforced by driver contract |
| character/shot resources | TH07 MultiplayerResources / Player resource setup | TH06 `Player`, SHT/resource load, ANM | ADAPTED | only Reimu/Marisa + A/B; stable slots load their own audited player ANM bank; P1 uses original globals while P2/P3 use resource sidecars; `multiplayer-resources-test.cpp` and ANM feature contract FOCUSED-PASS |
| enemy targeting | TH07 enemy/ECL target helpers | TH06 `EclManager`, `EnemyEclInstr`, `EnemyManager` | ADAPTED | gameplay/ECL target reads no longer hardcode P1; target is selected from active logical Player positions with deterministic stable-slot tie behavior, never presentation smoothing |
| bullet/laser collision | TH07 Bullet/Player logic | TH06 `BulletManager`, `Player` | ADAPTED | active stable Players are evaluated through TH06 collision/graze semantics; rollback owns all Player state; no TH07 Cherry/Border behavior imported |
| items/drops | TH07 `ItemManager` patterns | TH06 `ItemManager`, enemy drops | ADAPTED | item target/pickup/auto-collect ownership is stable-player aware; ECL Power-drop selection reads the relevant target resource rather than P1 global; `multiplayer-item-target-contract-test.py` FOCUSED-PASS |
| Bomb | TH07 Bomb ownership patterns | TH06 `BombData`, Player Bomb state/effects | ADAPTED | Bomb availability/decrement and Bomb animation scripts are per stable slot; each Player carries its own TH06 Bomb state/regions and shifted ANM bank; speculative audio side effects suppressed |
| shared vs per-player resources | TH07 gameplay sidecars as pattern only | TH06 score/power/lives/bombs/rank/etc. | ADAPTED | lives/bombs/power are per-player (P1 original owner, guests sidecar); score/rank/deaths/bombsUsed remain TH06 shared run metadata unless later gameplay evidence requires split; no Cherry/Border/Spirit sidecars |
| pause/retry/game over | TH07 Pause-menu Reset/session-generation lifecycle as synchronization reference; TH06 deliberately diverges on Continue policy | TH06 `GameManager`, `Supervisor`, pause/retry menu | ADAPTED | individual zero-life Player remains revivable/terminal without ending the room; only a complete team wipe starts the deterministic 180-logical-frame grace. ABI6 requests `SUPERVISOR_STATE_RESULTSCREEN_FROMGAME` directly when that grace expires, and the ordinary Retry menu is a hard fallback only, so Continue is never exposed. Separately, multiplayer Pause accepts synchronized `R` from any participant and follows a dedicated fresh-attempt state: Story returns to Stage 1, Extra/Practice restart the current stage, the old rollback generation retires, and the existing transport synchronizes a new generation from frame zero. `restart2` reaches generation 1 and F900 with matching hash; `restart3` does the same under 55 ms delay + 10 ms jitter. Pause/Resume and no-Continue 2P/3P gates remain FOCUSED-PASS. |
| stage transition | TH07 history invalidation | TH06 Stage/GameManager transition | ADAPTED | production driver retains stage ownership and synchronized input lifecycle. 2P and 3P production browser runs crossed Stage 1 -> 2 on exactly the same logical frame across all peers and continued thousands of frames with matching canonical hashes; 3P transition frame 10762, F18000 hash `ecdf95fcfd0c3993` (FOCUSED-PASS). |
| ending/result | TH07 ResultScreen/Supervisor | TH06 Ending/ResultScreen | ADAPTED | Final Stage 6 clear keeps the real ECL -> Gui MSG_STAGEEND -> Supervisor transition. After the confirmed gameplay boundary, rollback ownership retires (`generation=1`) and vanilla Ending/ResultScreen run locally. MP ResultScreen deliberately suppresses persistent ordinary `score.dat` progression/high-score writes while retaining the normal Replay-save UI. Full 2P/3P browser lifecycle reached Ending -> Result and saved `./replay/th6_01.rpyx` on every peer (FOCUSED-PASS). |

---

# Phase D - TH06 rollback state inventory

Do not start from `sizeof(manager)` snapshots. Build an owner/mutation ledger.

| TH06 owner/state | Affects future simulation? | Mutation hooks identified? | Rollback strategy | Status / notes |
| --- | --- | --- | --- | --- |
| `GameManager` deterministic fields | yes | partial | fixed-state first-write touch / selected fields | READ - TH06 owns run score/rank/power/lives/bombs/retry/stage/timers directly in `GameManager`; unlike TH07 it has no `globals/defaultCfg` pointees |
| RNG | yes | fixed owner captured at frame checkpoint | fixed state | ADAPTED - `g_Rng` is part of every deterministic checkpoint; TH06 production session seed uses `Rng::Initialize(seed)` |
| Supervisor 60 Hz simulation fields | yes | partial | selected fields only | READ - capture only calc/state/effective-speed fields; never platform pointers, matrices, audio/MIDI or presentation timing |
| Controller/current+previous synchronized input globals | yes | yes for storage | fixed state | READ - ordinary raw lane remains `g_CurFrameInput/g_LastFrameInput`; MP-only game lanes are 3-slot arrays |
| all active Players | yes | stable active mask + every active `g_Players[]` touched at checkpoint | fixed owner per active slot | ADAPTED - full Player is cheap relative to 8 MiB frame cap and includes its 80 reusable shot slots + Bomb state; rollback owner contract protects the active-slot loop |
| player bullets | yes | currently covered inside full Player owner; future active guest Players inherit same policy | fixed with Player owner | ADAPTED - TH06 has 80 `PlayerBullet` slots, not TH07's 96 |
| enemies | yes | `EnemyManager::SpawnEnemy` first-writes `TouchEnemy(newEnemy)` before template overwrite | sparse first-write | ADAPTED - exact TH06 `EnemyManager::enemies[257]`; active slots captured at checkpoint start |
| enemy bullets | yes | `BulletManager::SpawnSingleBullet` first-writes `TouchBullet(bullet)` before mutation | sparse first-write | ADAPTED - exact TH06 `BulletManager::bullets[640]`; manager itself is ~1.24 MiB so full snapshot is deliberately avoided |
| lasers | yes | `BulletManager::SpawnLaserPattern` first-writes `TouchLaser(laser)` before mutation | sparse first-write | ADAPTED - TH06 has 64 laser slots |
| items | yes | `ItemManager::SpawnItem` first-writes `TouchItem(item)` before reuse | sparse first-write | ADAPTED - TH06 `ItemManager::items[513]`; manager is ~205 KiB but still kept sparse |
| gameplay effects | yes for active TH06 `Effect` slots because their update callbacks can consume shared `g_Rng` | `EffectManager::SpawnParticles` first-writes `TouchEffect(effect)` before reuse | sparse first-write | ADAPTED - TH06 `EffectManager::effects[513]`; manager ~238 KiB |
| Bomb state/regions | yes when active | contained in full `Player` owner | fixed with Player owner | ADAPTED - TH06 does not use TH07 `BombEffects`; `Player::bombInfo`/regions are captured with Player |
| ECL globals/context | yes | Enemy contexts live in each `Enemy`; the one confirmed cross-frame out-of-object owner is `ExInsShootStarPattern`'s captured player/enemy positions + six RNG-derived star angles | Enemy owner + small fixed ECL sidecar | ADAPTED - `EnemyEclInstr::RollbackState` is touched at every rollback checkpoint and included in canonical Stage hashing. This fixed a real omission where rollback across the star-pattern initialization frame could retain future player-position/random-angle state. Do not broaden this into a generic ECL blob unless another persistent owner is proven. |
| Stage gameplay state | yes where it changes future ANM/RNG execution | `g_Stage`, dynamic `quadVms`, and each `RawStageObject::flags` captured | fixed + dynamic range | ADAPTED - `Stage::UpdateObjects()` executes ANM; random-sprite ANM consumes shared `g_Rng` |
| GUI fields read by simulation | yes | `g_Gui` + `*g_Gui.impl` captured | fixed owner | ADAPTED - dialogue/wait/stage-end state participates in calc-chain decisions |
| dynamic `ScreenEffect` calc jobs | yes when active | enumerate calc chain, snapshot bounded list, remove/re-register on restore | dynamic owner reconstruction | ADAPTED - Shake consumes shared `g_Rng`; therefore it is not safe to dismiss as presentation-only |
| draw-only interpolation/presentation sidecars | no | n/a | **exclude from deterministic rollback** | TODO |
| decorative Stage/ANM-only visuals | exclude only after proving they cannot alter shared RNG/future calc | Stage background ANM failed this exclusion test | selective | ADAPTED - do not classify by appearance alone; TH06 ANM random opcodes are deterministic-state consumers |

For every new `TouchX()` hook, record the call sites that run before mutation. A state present in the initial capture but mutated later without a first-write hook is not rollback-safe.

### Rollback evidence gates

- [x] generic journal capture -> mutate -> undo restores exact selected state (`rollback-journal-test.cpp`, FOCUSED-PASS).
- [x] integrated TH06 owner capture/restore is exercised by production rollback audit across 2P/3P browser runs; restore audit remained clean under hundreds to thousands of corrections (FOCUSED-PASS).
- [x] production deterministic scripted runs with heavy prediction/rollback converge to the same cross-instance canonical state (2P and 3P WS/RTC, FOCUSED-PASS).
- [x] generic two-frame checkpoint target on second frame restores preceding keyframe and replays correctly (FOCUSED-PASS).
- [x] integrated TH06 two-frame owner replay is exercised by production rollback audit with repeated corrections, including Stage transition and Retry lifecycle (FOCUSED-PASS).
- [x] dense bullets/enemies/items profile stays within mobile budget. (2P/3P forced-WS FOCUSED-PASS using a hidden rollback-only profile that touches every reusable `Enemy[257]`, `Bullet[640]` and `Item[513]` slot each checkpoint without mutating gameplay. Under 20 ms delay + 15 ms jitter + every-7th packet loss, with one Chromium CPU-throttled 4x in 2P / 3x in 3P, peak snapshots were `2679333 B / 1477 blocks` and `2724681 B / 1478 blocks`, below the explicit 4 MiB / 2048-block mobile gate and well below the 8 MiB / 4096 fail-safe. Both runs reached F300 inside the 60 s smoke deadline with matching canonical hashes after heavy rollback.)
- [x] Stage 1 -> 2 transition invalidates old-stage history and continues without freeze in 2P/3P production browser runs (FOCUSED-PASS).
- [x] driver failure now propagates through the real `Chain::RunCalcChain()` integer contract and publishes explicit failure telemetry; the previous bug where `FAIL` could be logged while GameWindow continued was fixed in both TH06 and TH07 (FOCUSED-PASS + static contract).

---

# Phase E - Browser room / transport / Launcher integration

| Work item | TH07 upstream to read/copy | Status | Evidence needed |
| --- | --- | --- | --- |
| runtime room descriptor | TH07 Launcher -> TH07 runtime options path | ADAPTED | Launcher and TH06 shell now validate/install player count, local slot, seed, difficulty 0..4, ICE and Reimu/Marisa A/B loadouts; shell protocol + TH06 Launcher contracts FOCUSED-PASS |
| preconnect signaling/relay | TH07 public network handoff + launcher code | ADAPTED | TH06 production driver starts browser transport before gameplay gate and refuses frame zero until session HELLO/READY + exact first remote input. Real 2P/3P browser runs observed the exact F0 SAMPLE -> WAIT -> remote-confirmed -> SIM barrier (FOCUSED-PASS). |
| RTC full mesh | `BrowserPeerTransport` | COPIED | 2P/3P real multi-browser production Runtime smoke passes; 3P RTC scripted rollback stress reaches the same canonical hash on all peers (FOCUSED-PASS). |
| reliable control channel | current TH07 implementation | COPIED | reliable ordered control path and bounded HELLO/READY resend retained; source/generic tests FOCUSED-PASS; browser stress pending |
| fast input channel | current TH07 implementation | COPIED | unordered `maxRetransmits:0` RTC input path + peer ACK/redundant tail retained; 2P/3P scripted rollback stress plus generic loss/reorder/prediction tests FOCUSED-PASS |
| whole-room WS fallback | current TH07 implementation | COPIED | forced-no-WebRTC 2P/3P production browser smoke passes, including 120 ms fixed-delay rollback stress and deterministic packet-loss stress (FOCUSED-PASS). |
| route skew | current TH07 race test | COPIED | 2P/3P RTC browser skew smoke delays one endpoint's `route=rtc` barrier by 300 ms after all DataChannels open; pre-route control/input packets are retained and every peer reaches F300 with matching canonical state (FOCUSED-PASS). |
| ICE transient recovery | current TH07 implementation | COPIED | progress-aware 2.5s disconnected observation / failed immediate restart / max-two-attempt implementation inherited. 2P/3P RTC browser smoke reports one real peer edge transiently disconnected for 3.2 s while traffic and confirmed frames progress; production recovery reschedules with zero ICE restart requests and all peers converge at F600 (FOCUSED-PASS). The separate failed-state restart path also continues to matching F900 state. |
| signaling reconnect | current TH07 implementation | COPIED | live 2P/3P RTC browser smoke closes the active signaling WebSocket after route selection, observes a different reopened OPEN signaling socket, performs ICE restart, and continues deterministic simulation to F900 (FOCUSED-PASS). |
| spectator admission / frame-zero history | shared relay + confirmed-frame spectator path | ADAPTED | spectators are admitted only if present in the lobby at run start and do not enter player HELLO/READY/ACK/prediction/rollback. Relay keeps frame-zero history only while an admitted spectator remains unclaimed. Unclaimed admission expires after `TH07_SPECTATOR_CONNECT_GRACE_MS` (default 60 s), immediately releasing history; already-connected spectators are unaffected. Relay contract plus TH06/TH07 1.2 s delayed-start browser smokes FOCUSED-PASS. |
| spectator long-backlog bound | shared relay + both `BrowserPeerTransport` spectator paths | ADAPTED | no snapshot/reconnect/rollback was added. A connected spectator that cannot consume for several minutes is terminated instead of accumulating forever: relay outbound `bufferedAmount` defaults to a 1 MiB cap and each spectator Runtime caps its unconsumed JS queue at 16384 packets. `tests/spectator-backlog-contract-test.py`, Emscripten rebuilds, and delayed spectator browser smokes FOCUSED-PASS. |
| diagnostics | current TH07 host/runtime diagnostics | ADAPTED | Launcher reads `__th06PeerTransport` and shared frame/confirmed/rollback/resim/pacing globals. Cross-instance canonical hash was added; expensive sub-hashes/raw rollback hashes run only with rollback audit, while normal MP samples the composite every 300 frames (FOCUSED-PASS). |
| served-build marker | current TH07 marker pattern | ADAPTED | production driver publishes `__eaglerNetplayRuntimeBuild` with TH06MP marker; Launcher surfaces it in runtime diagnostics; served-host hash/device verification still pending |

Do not create a TH06-only networking protocol if the generic TH07 transport can be reused.

Spectator first-version policy remains deliberately simple: P1 is the only publisher, spectator transport is read-only WebSocket, no late join, no reconnect, no snapshot synchronization, no hash repair, no publisher election and no spectator rollback. The bounded history/backlog rules above solve the memory-growth problem without expanding that architecture.

---

# Phase F - Replay / ReplayX / touch replay

| Work item | Current reference | TH06 action | Status |
| --- | --- | --- | --- |
| preserve ordinary TH06 replay base | TH06 Replay audit + current ReplayManager | keep vanilla header/input stream as P1 compatibility lane; EAGX remains optional sidecar | FOCUSED-PASS; the ordinary Web binary loaded a public unmodified v1.02h `.rpy` through the real Replay list/stage picker and played Stage 1 past frame 300 |
| EAGX container version | current TH06 + TH07 ReplayExtension | `VERSION=1`, `DETERMINISM_ABI=1`, base header 96 bytes, MP header 160 bytes | FOCUSED-PASS |
| MP subtype flag/metadata | TH07 `FLAG_MULTIPLAYER_INPUT` | TH06 uses flag 32 plus playerCount/difficulty/gameplayAbi/loadouts without changing v1 identity | FOCUSED-PASS |
| synchronized per-player `FrameInput` lanes | TH07 ReplayExtension/Manager | record every active stable lane from synchronized overrides; P1 remains vanilla compatibility word | FOCUSED-PASS |
| local touch gesture lane | TH07 + user requirement | local touch events/state remain in the EAGX stream alongside multiplayer FrameInput samples | FOCUSED-PASS |
| no duplicate frames on rollback resim | TH07 ReplayManager | `Netplay::SideEffects::IsSpeculative()` exits before any ReplayExtension/frame append | FOCUSED-PASS |
| playback per-player overrides | TH07 NetplayInput/ReplayManager | `GetMultiplayerPlaybackFrame()` restores all lanes and installs `SetPlayerInputOverrides()` | FOCUSED-PASS |
| replay touch finger state machine | current TH06/TH07 Touch fixes | DOWN alone acquires a playback finger; orphan MOTION/UP require an already-active finger | FOCUSED-PASS |
| fresh run/restart touch reset | current TH06 fix + TH07 equivalent | playback start/delete reset Replay touch state; stage progression keeps the active stream | FOCUSED-PASS |
| result replay save prompt | TH07 ResultScreen incident | ResultScreen still calls normal `ReplayManager::SaveReplay`; SaveReplay appends the multiplayer EAGX recording rather than suppressing MP | FOCUSED-PASS |
| single-player persistence isolation | TH07 ResultScreen pattern | ordinary and MP binaries use independent IDBFS roots; MP Result suppresses ordinary score progression but retains MP Replay save | FOCUSED-PASS |
| direct later-stage MP Replay resource restore | TH07/TH06 EAGX sidecar audit | EAGX v1 appends optional per-stage P1/P2/P3 lives/bombs/power plus contribution snapshots after the established input body; playback restores them before Player registration | FOCUSED-PASS; browser/selftest coverage includes Stage1/Stage4 resource+contribution round-trip while old v1 files without the optional tails remain readable |

Evidence gates for the focused rows above: `python tests/multiplayer-replay-contract-test.py` -> `TH06 multiplayer replay contract: PASS`; current native `--replay-extension-selftest` -> PASS; 2P/3P `replay2`/`replay3` browser gates traverse Result save -> real Replay list/stage picker -> per-player playback; the clean ordinary Web build is also exercised by `tests/ordinary-browser-smoke.py` against the public unmodified v1.02h sample `th6_ud002677.rpy`, temporarily staged outside source control, and reaches ordinary Stage 1 Replay past frame 300 through the real ReplayManager path. These are automated runtime gates, not human visual/touch Replay acceptance.

2026-08-31 compatibility note: TH06 multiplayer gameplay ABI has one shared owner, `TH06_MULTI_GAMEPLAY_ABI`, used by the live session gate and new EAGX recordings. ABI4 introduced rollback ownership of the cross-frame `ExInsShootStarPattern` ECL sidecar. ABI5 changed the terminal team-wipe lifecycle so multiplayer goes directly to Result and Continue is forbidden. ABI6 adds the synchronized Pause-menu `R` quick-Restart/new-generation lifecycle. Old ABI5 peers are therefore rejected by the live room gate, but ABI5 EAGX Replay is explicitly still readable because Pause-menu Restart is not replayed gameplay evolution; early TH06 EAGX v1 recordings that historically hard-coded `gameplayAbi=1` also remain readable. Other correctly versioned non-current gameplay ABIs are rejected rather than silently replayed under different deterministic rules. Current `tests/replay-extension-browser-selftest.py` passes on the ABI6 Web runtime.

---

# Phase G - presentation / interpolation / multiplayer visuals

| Concern | Contract | Status |
| --- | --- | --- |
| local ship | authoritative logical `prevPositionCenter -> positionCenter` interpolation only; no presentation state feeds simulation | FOCUSED-PASS |
| remote ship | ordinary logical prev/current interpolation + bounded draw-only correction; presentation lag capped at 12 game pixels and never written back to Player state | FOCUSED-PASS |
| remote Options/attachments | TH06 orbs receive exactly the same current-frame `remoteDrawOffset` as their owning ship, then temporary positions are restored | FOCUSED-PASS |
| hitbox/labels/prompts | the Eagler visible hitbox uses the same corrected draw position; no separate TH06 MP player-name/prompt presentation owner is currently required here | FOCUSED-PASS |
| simulation targeting/collision | gameplay owners never read `g_RemoteDrawOffsets`/presentation state; targeting/collision continue to use authoritative logical Player coordinates | FOCUSED-PASS |
| teammate transparency | draw only; clamp and restore TH06's actual interpolation endpoints `color/prevColor`. **TH06 `AnmVm` has no `color2/prevColor2`; do not invent TH07 fields in this layout.** | FOCUSED-PASS |
| temporary VM transforms | Player/orb draw positions and alpha endpoints are saved/restored around draw calls; no temporary presentation transform becomes future simulation/ANM state | FOCUSED-PASS |
| broad TH06 scale/rotation/UV/color interpolation | **forbidden historical failed experiment** | EXCLUDED |
| local locator, if reused | no separate local-player locator was introduced for TH06MP; avoid adding one unless a real usability need appears | EXCLUDED |
| decorative desync | prefer no rollback overhead if gameplay irrelevant | TODO |

Evidence gate for the focused rows above: `python tests/multiplayer-presentation-contract-test.py` -> `TH06 multiplayer presentation contract: PASS`. This proves source/presentation isolation contracts only; final opacity/smoothing appearance still requires human visual acceptance.

---

# Phase H - resource/ANM audit

Before adding any MP visual bank:

- [x] enumerate actual TH06 archive/ANM file children.
- [x] enumerate every occupied player-related ANM file/script range relevant to guest player allocation.
- [x] enumerate script offsets/ranges.
- [x] choose sidecar player slots/ranges outside original ranges.
- [x] focused automated contract protects the chosen ranges (`tests/player-resources-feature-test.py`, FOCUSED-PASS). The contract also locks the complete original Title/Select/Replay/Result/Music/Staff file-slot block to `21..46`, proves guest files remain `47/48`, checks both guest script banks stay below `ANM_OFFSET_FRONT=0x600`, verifies Player releases the exact slot it loaded, and protects the real post-game/menu loaders from drifting into guest slots.
- [ ] normal Title / Replay / Music Room / Result / Staff menus are visually checked after MP resources are loaded/unloaded.

Do not trust old TH06 MP `AnmIdx.hpp` slot numbers.

### 2026-08-29 original player ANM audit

The audit used the current project's own `Pbg3Archive`/`Pbg3Parser` against the local original DATs, not filenames inferred from source alone.

- both gameplay player ANMs are archive entries in `紅魔郷CM.DAT`.
- `player00.anm` (Reimu): 28 sprites, raw sprite IDs `0..132`; 24 scripts, raw script IDs `0..140`; no chained `nextOffset` child.
- `player01.anm` (Marisa): 27 sprites, raw sprite IDs `0..129`; 27 scripts, raw script IDs `0..129`; no chained `nextOffset` child.
- vanilla `ANM_OFFSET_PLAYER=0x400`; therefore the real worst-case vanilla player script extent is `0x400 + 140 = 0x48c`, matching `ANM_SCRIPT_PLAYER_END`.
- `face00a.anm` and `face01a.anm` each use only raw sprite `0..1` and script `0..3`; the source-defined original face banks end at `0x4ab` before `ANM_OFFSET_FRONT=0x600`.
- `AnmManager` has 128 ANM file slots and 2048 script/sprite indices. File-slot capacity is not the limiting factor; global script/sprite index collision is.

Candidate fresh MP sidecar layout to protect with a source contract before activation:

- guest P2 file slot `47`, script/sprite offset `0x4c0` -> worst case `0x54c`.
- guest P3 file slot `48`, script/sprite offset `0x550` -> worst case `0x5dc`.
- both fit entirely after original face end `0x4ab` and before original front/staff bank at `0x600`, with no overlap with each other.

These are **new audited sidecar choices**, not a revival of the rejected historical TH06 P2 ANM slots.

### 2026-08-29 rollback owner size audit

Native `sizeof` audit of the current TH06 source (temporary audit artifact removed afterwards):

- `Player=48,032`
- `Enemy=4,664`, `EnemyManager=1,203,456`
- `Bullet=1,816`, `Laser=776`, `BulletManager=1,239,072`
- `Item=400`, `ItemManager=205,208`
- `Effect=464`, `EffectManager=238,040`
- `GuiImpl=13,984`, `AsciiManager=50,608`, `Stage=944`, `AnmVm=336`

Decision: capture Player whole, but keep Enemy/Bullet/Item/Effect slot storage sparse with first-write hooks. This is a TH06 layout decision, not a copied TH07 capacity assumption.

---

# Phase I - acceptance matrix

## Automated / stress

- [x] ordinary TH06 build after legacy purge. (official Web link BUILD-PASS)
- [x] TH06 netplay-enabled desktop build. (Fresh `build-desktop-netplay-th06` MinGW tree configured with `TH_ENABLE_NETPLAY=ON`, `TH_EXTERNAL_ASSETS=ON`, THPRAC/THCRAP OFF and linked `th06.exe` successfully. The build compiles the current isolated multiplayer/rollback/netplay source set; native `BrowserPeerTransport` remains its intentional Web-only stub, so this is a desktop compile/link gate rather than a native online-play claim.)
- [x] TH06 netplay Web build. (official CMake/Ninja link BUILD-PASS)
- [x] generic netplay core focused suite.
- [x] rollback state focused suite.
- [x] 2P two-runtime deterministic run without network loss.
- [x] latency/jitter/loss/reorder/duplicate run. (2P/3P WS scripted stress; every 7th relay game packet loss also converges)
- [x] frame-zero packet drop recovery. (2P/3P forced-WS FOCUSED-PASS with a test-only shared-relay switch dropping exactly the first `PacketType::Input` on every directed peer edge. The production driver remained behind the exact frame-zero barrier, retried the already-scheduled logical input without re-sampling, and all peers reached F300 with matching canonical hashes.)
- [x] prediction-window boundary packet drop recovery. (2P/3P forced-WS FOCUSED-PASS. The test relay drops the first transmission of every input packet whose `latestFrame` is 120..132. With last confirmed 119, the first diagnostic run stopped exactly at `sim=132/confirmed=119`, proving the 12-frame prediction ceiling. Allowing the production three-driver-tick retry of frame 132 then restored the full gap from the 32-frame redundancy tail; all peers continued to F300 with matching canonical hashes.)
- [x] pause/resume. (2P + 3P production browser FOCUSED-PASS)
- [x] **Pause-menu R quick Restart**. This is distinct from Continue and is only recognized while the synchronized Pause UI owns input. P1 opens Pause at frame 600 and presses R at frame 620; `restart2` retires generation 0, re-synchronizes generation 1, and reaches F900 with matching canonical hash. `restart3` repeats the same production lifecycle for 3P under 55 ms delay + 10 ms jitter and all three peers converge at F900. Normal gameplay does not have a global R Restart path.
- [x] **TH06MP no-Continue team wipe**. Complete team wipe keeps the deterministic 180-logical-frame grace, then requests Result directly without opening Retry. 2P/3P forced-WS browser elimination smokes both hand off on frame 781 and retire gameplay ownership with `generation=1` on every peer.
- [x] adversarial old Retry-Yes input cannot revive the room. The `retry2`/`retry3` browser probes still inject the old Up/Shoot Continue-selection pattern around the terminal handoff; ABI6 nevertheless reaches direct Result at frame 781 on all peers with no Retry-Continue or resource-reset path. `retry2` was re-run after Pause-R landed and still passes.
- [x] browser receive/control backlog remains bounded. (2P/3P forced-WS FOCUSED-PASS under 20 ms delay + 15 ms jitter + every-7th forwarded packet loss with one Chromium CPU-throttled. 2P peak receive backlog `3/26`, pending signals `0/0`, send buffered bytes `250/534`; 3P `5/6/14`, `0/0/0`, `344/320/1780`. Both runs drained back near zero and converged at F900, so no extra production queue cap was added.)
- [x] forced WebSocket fallback. (2P/3P FOCUSED-PASS)
- [x] RTC route skew. (2P/3P RTC FOCUSED-PASS with the shared signaling/relay server delaying one endpoint's `route=rtc` barrier by 300 ms after all DataChannels are already open. Earlier peers begin the real session gate and send control/input before the delayed endpoint receives its route; TH06 `BrowserPeerTransport` retains those pre-route RTC packets and all peers cross frame zero, reach F300 and agree on canonical hashes.)
- [x] transient disconnect no false ICE restart. (2P/3P RTC FOCUSED-PASS. Each endpoint drives the production `schedulePeerRecovery()` path with one real RTC peer temporarily reporting `connectionState`/`iceConnectionState=disconnected` for 3.2 s while DataChannel traffic and confirmed frames continue. The first 2.5 s observation window sees progress and reschedules instead of restarting; every edge settles with zero `requestPeerIceRestart()` calls and all peers converge at F600.)
- [x] production ICE restart recovery path. (2P/3P live RTC browser FOCUSED-PASS; production `restartPeerIce()` invoked mid-run while signaling is also forced to reconnect. A synthetic browser `connectionState=failed` event itself is not claimed.)
- [x] signaling WebSocket reconnect while RTC gameplay remains active. (2P/3P FOCUSED-PASS)
- [x] synchronized gameplay Quit retires rollback ownership. (2P/3P Pause -> Q -> vanilla Quit Yes; all peers end with `generation=1`, `active=false`, after prior rollback activity.)
- [x] stage transition. (2P/3P Stage 1 -> 2 FOCUSED-PASS; peers transition on the same logical frame and later canonical hashes match)
- [x] final Stage 6 clear -> Ending -> Result -> Replay-save lifecycle. (2P/3P FOCUSED-PASS; real Stage 6/ECL/Gui/Supervisor path, rollback retires with `generation=1`, every peer saves `./replay/th6_01.rpyx`. Current storage telemetry confirms all 3P peer paths resolve below `/savesth06-multiplayer/replay/`. The test-only accelerator satisfies active boss HP/timer thresholds inside rollback but never writes scene/result state directly.)
- [x] Replay save/load with synchronized lanes. (2P/3P FOCUSED-PASS through the real Result save UI, MainMenu Replay list, stage picker and ReplayManager playback. Saved `./replay/th6_01.rpyx` files reconfigure the recorded MP player count/loadouts and feed distinct recorded P2/P3 input lanes back into gameplay; observed 2P playback inputs include `[321,33,0]`, and 3P includes `[17,133,321]`.)
- [x] touch Replay restart/two-finger ghost-cross regression. (`multiplayer-replay-contract-test.py` FOCUSED-PASS plus a current THPRAC+THCRAP `TH_DEV_TOOLS=ON` native rebuild and real `th06.exe --touch-selftest` PASS. The executable state-machine test creates two playback fingers, clears them at the Restart boundary, then proves stale MOTION/UP cannot recreate either cross. The combined MP+touch EAGX body order is also locked to the TH07 v1 contract and `--replay-extension-selftest`, `replay2`, and `replay3` all pass after the correction. This does not claim human visual/touch Replay acceptance.)
- [x] ANM slot/resource regression. (`tests/player-resources-feature-test.py` FOCUSED-PASS with exact original menu/staff file ranges, guest file/script-bank separation and load/release ownership. Existing 2P/3P Stage6 -> Ending/Staff -> Result -> MainMenu Replay browser lifecycles complete without ANM/resource failure, and the ordinary `music-room` browser gate enters the real Music Room with all 17 descriptors loaded. This is an automated ownership/collision gate only; the separate human Title/Replay/Music Room/Result/Staff visual check remains open.)
- [x] ordinary `.rpy` Replay regression. (The clean ordinary Web build uses `/savesth06`, not the MP save root; Replay Viewer explicitly loads the title archive and cached transition surfaces when entering directly. Current `tests/ordinary-browser-smoke.py` checks the public sample's TH06 Replay magic, drives the real Replay list/stage picker, and observes ordinary Stage 1 playback past frame 300; the companion Music Room gate also enters the real ordinary Music Room with 17 descriptors. The temporary sample is absent from source control and the clean artifact. Automated browser evidence only; the human ordinary-single-player/thprac/Replay item remains open.)

## Human / real device - never infer from automation

- [ ] PC + phone actual 2P start and sustained play.
- [ ] keyboard/controller ownership.
- [ ] phone direct touch ownership.
- [ ] simultaneous movement + Focus + Shoot + Bomb.
- [ ] rollback correction visually smooth without changing collision.
- [ ] close overlapping players remain readable/non-obstructive if transparency feature is enabled.
- [ ] pause/restart repeatedly without page-unresponsive behavior.
- [ ] BGM/SFX remain correct around rollback/pause/restart.
- [ ] death/game-over/retry behavior.
- [ ] stage transition/clear/result/ending chain.
- [ ] Replay actually reproduces play and local touch gesture visualization.
- [ ] ordinary single-player/thprac/replay still behave normally after MP work.

---

# Decision log

Append dated entries here instead of silently changing architecture.

## 2026-08-29 - new TH06MP line

- Historical external TH06 multiplayer upstream and all old TH06 MP progress are rejected by user.
- Current tested/fixed TH07MP working-tree implementation becomes the engineering upstream.
- Agent must read/copy actual TH07 code and tests, not merely borrow concepts.
- TH06 original source remains the authority for TH06-specific owner/game semantics.
- TH06MP documentation is isolated in `th06-eagler/docs/th06-mp/` and must remain separate from TH07MP ledgers/handoffs.

## 2026-08-29 - ordinary/MP binary isolation and generic foundation

- User explicitly requires the TH07 publication model: ordinary TH06 and TH06MP are separate binaries/build trees. Do not ship one fat runtime and toggle multiplayer at runtime.
- Writable state follows that binary boundary: every Web MP build uses `/savesth06-multiplayer` for config, score/save data, Replay and other IDBFS files even when no LAN room is active; it never falls back to ordinary `/savesth06`. Read-only DAT/package, font, language and BGM URLs remain shared between variants so browser/CDN cache and transfer traffic are reused rather than duplicated.
- `TH_ENABLE_NETPLAY` and `TH_ENABLE_MULTIPLAYER_GAMEPLAY` default OFF. Ordinary TH06 therefore does not compile/link the new multiplayer/netplay source tree or WebSocket support.
- The rejected `TH_ENABLE_MULTIPLAYER` flag and `MultiplayerRuntime.*` architecture remain deleted and must not return under another name.
- Current TH07 working-tree `NetplayCore`, `NetplaySession`, `RollbackJournal`, `NetplaySideEffects`, `BrowserPeerTransport`, and `WebSocketTransport` are the copied engineering foundation.
- TH06 packet identity is deliberately `E6NP`; TH07 relay byte `0xe7` is not a game id. It is the existing server-side targeted-envelope marker and remains unchanged so TH06 reuses the mature relay dialect instead of inventing a second one.
- Browser client symbols/DataChannel labels are TH06-specific (`__th06PeerTransport`, `th06-control`, `th06-input`) so separate game runtimes do not masquerade as TH07 internally.
- Stable gameplay capacity is initially three slots to preserve the proven 2P/3P generic architecture, but TH06 loadouts are strictly Reimu/Marisa + A/B. TH07-only third-character/Cherry/Border/Spirit/Stage-4-chain semantics are excluded.
## 2026-08-30 - Ending/Result ownership and lifecycle gate

- TH06 does not keep rollback ownership through Ending/Result. The deterministic gameplay session owns the final Stage 6 clear and MSG_STAGEEND boundary, then retires before vanilla Ending/ResultScreen take over.
- Multiplayer ResultScreen keeps the original Replay-save UI but skips persistent ordinary `score.dat` progression/high-score writes so a room result cannot contaminate single-player progression.
- The lifecycle smoke must not fake `Supervisor::curState`, `Gui::finishedStage`, Result state, or call `SaveReplay()` directly. Its only acceleration is deterministic boss HP/timer-threshold completion after real Stage 6 registration; all ECL callbacks, Gui stage-end logic, Supervisor scene transitions, Ending parsing, Result UI and ReplayManager save calls remain production paths.
- 2P and 3P forced-WS browser runs both completed the full path and saved `.rpyx` on every endpoint.

## 2026-08-30 - Multiplayer Replay save/load gate

- Multiplayer `.rpyx` acceptance is not file-format-only: the browser smoke saves through the real Result UI, returns to MainMenu, enters the real Replay list/stage picker, and starts normal ReplayManager playback from that saved file.
- Playback restores the recorded multiplayer player count/loadouts before GameManager starts, then `ReplayExtension::GetMultiplayerPlaybackFrame()` supplies the recorded per-player lanes to ReplayManager. 2P and 3P runs both observed playback beyond frame 300 with independent nonzero P2/P3 inputs, proving guest lanes are consumed rather than merely present in the extension payload.
- The hidden Ending/Replay lifecycle harness may use a 60-second remote-input watchdog because three headless Chromium renderers on one test host can be descheduled for more than the production 15-second watchdog. The production watchdog remains 15 seconds; ordinary rollback/loss tests do not receive this relaxation.
