# TH07 sbrik feature-rebase ledger

## Frozen scope

- After every context compaction, read this ledger completely before any other workspace inspection or modification.
- Gameplay oracle: `5b9ebe892914ff5666ef68c0cd02719dde7d4ee9..022c533` from `mp-source-sbrik/main`.
- Port gameplay semantics hunk by hunk onto the current Eagler TH07 tree.
- Keep Eagler WebSocket transport, Web/mobile touch input, presentation smoothing, audio adaptation, and browser lifecycle handling.
- Exclude sbrik WinSock, Windows threads, LowLatency, and native UDP shell.
- The only permitted hash use is runtime partitioned state hashing for desync diagnosis. It must never gate joining, reject peers, decide compatibility, repair state, or alter simulation.
- No upload, publication, deployment, or commit is part of this work.
- Reusable Eagler adaptations and audit lessons are maintained in `docs/th06-th07-multiplayer-feature-rebase-notes.md` for the likely TH06 sister port. That document is not a substitute for TH06's own upstream oracle.

## Upstream file ledger

The read-only three-way preview reports 31 conflicted files and six upstream-only files. The hunk count comes from the frozen upstream range with zero context. For an upstream-only file, one Git hunk may contain many functions, so it is only an inventory marker. `clean` merge status would never be sufficient evidence: every retained hunk needs a semantic disposition and test reference before its status can become `Verified`.

| Work class | File | Upstream hunks | Three-way conflicts | Required disposition | Status |
| --- | --- | ---: | ---: | --- | --- |
| Assets/ANM | `AnmIdx.hpp` | 4 | 3 | Port player asset slots and offsets | Hunk-complete: 4/4 ported; mixed-loadout visual acceptance pending |
| Assets/ANM | `AnmManager.cpp` | 16 | 12 | Port multiplayer asset ownership; preserve Eagler renderer | Hunk-complete: 16/16 disposed; renderer/Web cache retained |
| Assets/ANM | `AnmManager.hpp` | 4 | 3 | Port slot declarations without changing normal layout | Hunk-complete: 4/4 disposed; portable layout adaptation retained |
| HUD | `AsciiManager.cpp` | 11 | 6 | Port multiplayer display semantics; preserve interpolation fixes | Hunk-complete: 11/11 disposed; pause/HUD visual acceptance pending |
| Bomb | `BombData.cpp` | 27 | 27 | Port per-player bomb ownership and damage | Hunk-complete: 27/27 ported; all-loadout Bomb acceptance pending |
| Bullet | `BulletManager.cpp` | 11 | 15 | Port multi-target gameplay and deterministic ownership | Hunk-complete: 11/11 ported; real bullet/laser collision acceptance pending |
| Input | `Controller.cpp` | 2 | 2 | Port logical lanes; preserve Web/touch sampling | Hunk-complete: 2/2 disposed; real two-end touch acceptance pending |
| Input | `Controller.hpp` | 3 | 2 | Port three lanes; retain Eagler analog payload | Hunk-complete: 3/3 disposed; real two-end touch acceptance pending |
| ECL | `EclManager.cpp` | 30 | 20 | Port multi-player targeting and Stage 4 rules | Hunk-complete: 30/30 disposed; real Stage 4/targeting acceptance pending |
| ECL | `EclManager.hpp` | 1 | 3 | Port required multiplayer state | Hunk-complete: 1/1 ported; rollback restore structurally verified |
| Effects | `EffectManager.cpp` | 14 | 3 | Port player attachment/ownership; preserve presentation-only offsets | Hunk-complete: 14/14 disposed; attached-effect visual acceptance pending |
| Effects | `EffectManager.hpp` | 2 | 2 | Port effect capacity/ownership without breaking rollback journal | Hunk-complete: 2/2 disposed; portable layout retained |
| ECL | `EnemyEclInstr.cpp` | 1 | 1 | Port multiplayer ECL behavior | Hunk-complete: 1/1 ported; real Youmu pattern acceptance pending |
| Enemy | `EnemyManager.cpp` | 33 | 17 | Port targeting, damage scaling, drops and contribution ownership | Hunk-complete: 33/33 disposed; real multi-player damage/drop acceptance pending |
| Diagnostics | `GameErrorContext.cpp` | 3 | 1 | Port non-hash diagnostics only | Hunk-complete: 3/3 excluded as inseparable native hash/resync log policy |
| Game rules | `GameManager.cpp` | 20 | 9 | Port initialization, shared state, rank and unlock semantics | Hunk-complete: 20/20 disposed; real startup/Border/unlock acceptance pending |
| Game rules | `GameManager.hpp` | 3 | 2 | Port resource APIs without altering normal data contracts | Hunk-complete: 3/3 disposed; ordinary layout guard retained |
| Runtime | `GameWindow.cpp` | 10 | 6 | Reconcile rollback behavior; preserve Web pacing | Hunk-complete: 10/10 native display hunks disposed; Web pacing verified structurally |
| Runtime | `GameWindow.hpp` | 2 | 1 | Retain only transport-neutral state | Hunk-complete: 2/2 native render-wrapper hunks excluded |
| HUD | `Gui.cpp` | 54 | 25 | Port per-player resources and portraits; preserve stable compact HUD | Hunk-complete: 54/54 disposed; 2P/3P visual matrix pending |
| Items | `ItemManager.cpp` | 49 | 28 | Port rotation, targeting, collection and shared extends | Hunk-complete: 49/49 disposed; real two-end acceptance pending |
| Items | `ItemManager.hpp` | 2 | 2 | Port targeted item APIs | Hunk-complete: 2/2 disposed; real two-end acceptance pending |
| Native shell | `LowLatency.cpp` | 1 | New file | Exclude; do not port Windows low-latency loop | Excluded |
| Native shell | `LowLatency.hpp` | 1 | New file | Exclude | Excluded |
| Menu | `MainMenu.cpp` | 17 | 4 | Port deterministic menu/unlock/loadout behavior | Hunk-complete: 17/17 disposed; real menu/session acceptance pending |
| Player/resources | `Multiplayer.hpp` | 1 | Local file exists | Reconcile constants with sbrik | Hunk-complete: constants equivalent; transport-neutral adaptation retained |
| Player/resources | `MultiplayerResources.cpp` | 1 | Local file exists | Audit every upstream function body | Hunk-complete: final file equivalent except Eagler GUI API rename; real acceptance pending |
| Native network | `Netplay.cpp` | 1 | New file | Do not port shell; separately map mature rollback behavior | Function-class complete: portable rollback mapped; live 2P/3P loss and physical-keyboard paths pass, physical touch matrix pending |
| Native network | `Netplay.hpp` | 1 | New file | Do not port WinSock protocol/API | Excluded direct port; transport-neutral contracts mapped locally |
| Player/resources | `Player.cpp` | 126 | 69 | Port every gameplay hunk; preserve touch/presentation adaptations | Hunk-complete: 126/126 disposed; real two-end acceptance pending |
| Player/resources | `Player.hpp` | 4 | 6 | Port states/sidecars without breaking normal layout | Hunk-complete: 4/4 disposed; build/layout checks pass |
| Replay | `ReplayManager.cpp` | 8 | 6 | Port multiplayer Replay policy; preserve EAGX compatibility | Hunk-complete: 8/8 disposed; real ordinary/EAGX playback acceptance pending |
| Result | `ResultScreen.cpp` | 28 | 4 | Port multiplayer result semantics | Hunk-complete: 28/28 disposed; real result/replay/save acceptance pending |
| Audio | `SoundPlayer.cpp` | 2 | 2 | Reconcile side effects; preserve Web audio commit rules | Hunk-complete: 2/2 disposed; real two-end BGM/SFX acceptance pending |
| Runtime | `Supervisor.cpp` | 48 | 21 | Port only gameplay/runtime semantics; preserve portable host | Hunk-complete: 48/48 disposed; real lifecycle/input/audio acceptance pending |
| Runtime | `Supervisor.hpp` | 2 | 2 | Reconcile runtime state without Windows dependencies | Hunk-complete: 2/2 disposed; lane/rollback contract verified |
| Entry | `main.cpp` | 11 | 6 | Reconcile session setup with Eagler launcher | Hunk-complete: 11/11 disposed; real room launch/shutdown acceptance pending |

Inventory totals: 37 changed upstream files, 31 conflicted existing files, six upstream-only/local-collision files, and 554 upstream diff hunks. Directly excluded native-shell files remain represented so their gameplay-neutral exclusion is reviewable rather than implicit.

## Eagler adaptation preservation ledger

These are not optional deviations. They are known regressions already observed on real desktop/mobile endpoints and must survive every upstream hunk application.

| Area | Observed symptom | Root cause | Preserved solution | Regression evidence required |
| --- | --- | --- | --- | --- |
| Touch ownership | One phone touch controlled both players; the other endpoint did not reproduce it | Raw host touch leaked through global input while synchronized players shared the same logical update path | Carry buttons, analog mode, coordinates, unlimited flag, touch-used, and touch-bomb per player; each `Player` consumes the lane selected by `initParam`; raw touch is sampled only for the local slot | Two endpoints: touching one device moves only its player and is visible identically on the peer |
| Direct-touch prediction | Remote touch movement raced in one direction and snapped back | A touch displacement is a one-frame delta, but held-input prediction repeated it on every missing frame | Repeat direct-touch displacement for only the first missing frame; zero the displacement on the second and later missing frames while retaining held buttons/flags | Inject delayed input and verify bounded motion with no repeated delta |
| Touch rollback/deathbomb | Hit/death diverged after several frames | Deathbomb logic read local-global touch state rather than synchronized per-player touch flags | Use the synchronized player lane for touch-used and touch-bomb during normal simulation and resimulation | Two-end hit/deathbomb run with touch and rollback |
| Remote presentation | The peer player moved in visible bursts despite synchronized logic | Rollback corrections were drawn directly from authoritative logical coordinates | Apply bounded presentation-only smoothing to remote player, options, hitbox, and attached effects; never write the smoothed position into simulation or snapshots | Remote keyboard and touch movement remain smooth while local control stays immediate |
| Attached Cherry/focus/border effect | The player sprite was smooth but its following Cherry formation still jittered | Attached effects continued drawing the logical position | Add the same remote presentation offset only at draw time and leave effect simulation state unchanged | Remote attached effects remain aligned during movement and rollback |
| HUD life/Bomb icons | Heart and Bomb icons continuously scaled up and down while text stayed stable | Compact HUD changed `AnmVm::scale` temporarily but left interpolated `prevScale` at another value | Set and restore both `scale` and `prevScale` around compact icon draws | Both resource rows remain visually stable at non-integer render cadence |
| Independent shot power | P2's power bar rose but its emitted pattern still followed P1; P1 death changed both players' practical firepower | `Player::SpawnBullets` selected the SHT level from P1's global `currentPower` | Match sbrik: select the SHT level with `GetPlayerPower(player->initParam)` | Give P1/P2 different power values, then kill P1; P2 bullet pattern must follow only P2's power |
| Audio side effects | Pre-boss BGM continued into the boss instead of switching | All synchronized forward frames were treated as speculative, suppressing normal audio commands | Suppress side effects only during rollback resimulation; normal forward simulation commits BGM/audio commands | Real run must switch stage/boss music on both endpoints |
| Network stall presentation | Packet variation stalled visible animation and left the game in slow time | Stalled logical ticks discarded or leaked wall-clock backlog into presentation pacing | Keep a bounded logical backlog, catch up a limited number of fixed ticks after recovery, and continue presentation independently | Inject jitter and verify no permanent slow time or unbounded catch-up burst |
| Deterministic RNG | Boss phase desynced quickly | RNG consumers and rollback state needed one synchronized seed and complete restoration | Keep gameplay RNG session-seeded and rollback-owned; audit every new upstream RNG consumer | Boss-entry and sustained boss-pattern two-end run |
| Item interpolation and touch | Ported transfer/auto-collect motion could bypass Eagler interpolation or read one endpoint's raw touch state | Upstream item motion predates the current per-player touch lanes and `prev/current` render path | Item simulation targets synchronized logical `Player::positionCenter`; spawn and each logical tick publish `prevPosition`, and `OnDraw` alone applies `Lerp(..., g_RenderAlpha)` | Two endpoints: direct-touch collection and life/power transfer remain smooth; delayed input/rollback must not change ownership or make the remote item snap |

## Items hunk disposition

The identifiers below refer to the zero-context final diff `5b9ebe892914ff5666ef68c0cd02719dde7d4ee9..022c533`, in file order. `C01..C49` are `src/th07/ItemManager.cpp`; `H01..H02` are `src/th07/ItemManager.hpp`. Except for `C05` from `22b4bcb8e75a0366f1fb99fc55697b302fa85220`, every Items hunk comes from `d93bc947b01a97633ac0e29db789093979e2c112`.

| Upstream hunk | Upstream lines | Local landing/disposition | Focused evidence |
| --- | --- | --- | --- |
| C01-C02 | `+8`, `+11` | Excluded `GameErrorContext.hpp` diagnostics and native `Netplay.hpp`; Eagler uses guarded `GameplaySession.hpp` and rollback journal only | Contract rejects native include and upstream diagnostic callback |
| C03 | `+34..248` | Ported transfer/auto-collect markers, active-target rules, deterministic eligible-slot distribution, nearest logical target, round-robin Power ownership, and separated resource drops; native log-once flags excluded | Contract: mapping, distribution, active-slot round robin, no raw touch |
| C04 | `+298..299` | Equivalent local temporaries/factoring; no data-layout change | Ordinary and multiplayer compile |
| C05 | `+302..308` | Ported `22b4bcb` transfer Power exemption before conversion | Contract checks transfer states and targeted Power calls |
| C06-C07 | `+310..331` | Ported normal-vs-multiplayer Power-to-Cherry decision and fixed active-slot round robin; guarded to preserve vanilla normal build | Deterministic round-robin model plus both desktop builds |
| C08-C10 | `+370..386` | Ported states 3/4/5, 60-pixel rise target and fixed P2/P1/P3 marker; preserved `prevPosition` and sprite interpolation publication | Transfer mapping/easing/collision-lock contract |
| C11 | `+394..439` | Ported enemy-only LIFE/BOMB duplication, active-slot count and 32-pixel separated positions; native diagnostic logging excluded | 2P/3P/edge position model and call-site audit |
| C12-C13 | `+455..459` | Ported target locals and per-target collection radius | Source contract and builds |
| C14 | `+473..480` | Ported exact inactive fixed-target release: clear marker/state permanently, then select closest active logical player | Source contract; this corrected a prior local retarget-on-return deviation |
| C15 | `+497..514` | Ported 20-frame eased transfer rise, collision suppression and transition to homing | Monotonic frame 0..19 model and source contract |
| C16-C20 | `+517..565` | Ported shared-Border redistribution, fixed-target protection outside Border, per-slot auto-collect markers and homing; native verification logs excluded | Source contract; this corrected prior fixed-target behavior during shared Border |
| C21 | `+597..603` | Ported logical target radius/collision and transfer collision lock | Source contract and builds |
| C22-C24, C26 | `+609..629`, `+642` | Ported small-Power read/add/cap/level behavior to the collecting slot | Target-specific Power contract |
| C25 | `+632` | Ported per-slot cap setter. Upstream removal of the pre-existing vanilla integrity checksum is deliberately not applied because every hash-related change must be skipped pending user direction; no new hash behavior was added | Ordinary build preserves baseline behavior; multiplayer setter remains per-slot |
| C27-C28 | `+660..687` | Ported target-specific POC score and generic auto-collect award/color behavior | Source contract and builds |
| C29 | `+734` | Ported shared point-threshold extends in multiplayer; vanilla normal path retained | Source contract and both desktop builds |
| C30-C32, C34 | `+743..755`, `+768` | Ported big-Power read/add/cap/level behavior to the collecting slot | Target-specific Power contract |
| C33 | `+758` | Same hash disposition as C25: per-slot setter ported; checksum deletion skipped and not replaced with any new hash | Both desktop builds |
| C35-C36 | `+784..786` | Ported Bomb cap/add to the collecting slot | Target-specific Bomb contract |
| C37-C38 | `+792..804` | Ported LIFE ownership to the collecting slot; native `ReportLifeTransferTestResult` diagnostic excluded | Target-specific life contract |
| C39 | `+806` | Ported full-Power eligibility to the collecting slot | Source contract and builds |
| C40 | `+814` | Same hash disposition as C25/C33: per-slot full-Power setter ported; checksum deletion skipped | Both desktop builds |
| C41-C45 | `+820..851` | Ported owner Bomb state and per-player Cherry/Border contribution for point-bullet and small-Cherry items | Shared Cherry owner contract |
| C46-C47 | `+857..873` | Ported target-specific POC and auto-collect scoring/color for Cherry items | Source contract and builds |
| C48-C49 | `+882`, `+901` | Ported Cherry and Star contribution through the collecting slot | Shared Cherry owner contract |
| H01 | `+9` | Ported guarded `GetLifeTransferSpawnState` API without changing normal build symbols | Header contract and both desktop builds |
| H02 | `+71..74` | Ported narrow `SpawnEnemyDrop` API; no native network dependency | Header contract, call-site audit and builds |

Items hunk accounting: `49/49` C++ hunks and `2/2` header hunks have a disposition. Gameplay code is ported or adapted; native logs/network calls are explicitly excluded; the three checksum-deletion slices are skipped under the user's hash boundary. No hash implementation was added or expanded.

### Items real-person acceptance

- Give P1/P2/P3 distinct Power and Bomb counts, collect small Power, big Power, full Power, Bomb and LIFE with each slot, and confirm only the logical collector's private resource changes.
- Put two then three active players under the same enemy LIFE/BOMB drop. Confirm one visibly separated copy per active slot, with no uncollectible reserved copy.
- Trigger top-of-screen auto-collect with two eligible players and then shared Border. Confirm deterministic even distribution on both endpoints and no item ownership divergence after rollback.
- Start a life or Power transfer, make the receiver temporarily absent before frame 20, then return it. Confirm the released item stays reassigned instead of snapping back to the old receiver.
- On a touch endpoint, move through items and perform life/Power transfer while the peer uses keyboard. Confirm touch controls only its own player, collection/ownership agree on both peers, and item motion uses smooth render interpolation without feeding smoothed coordinates back into simulation.
- Inject delayed input during transfer and auto-collect. Confirm the same recipient and resource totals after rollback, without repeated direct-touch displacement or remote item snapping.

## Player/resources hunk checklist

Each row must end as `ported`, `already equivalent`, `Eagler adaptation`, or `excluded with reason` and cite a focused test.

| Semantic area | Upstream expectation | Current disposition | Status |
| --- | --- | --- | --- |
| Slots and lifecycle | P1/P2/P3 active slots, temporary absence, spirit, eliminated | Ported slot array/mask, rollback identity repair, absent/spirit/eliminated guards and persistent projectile lifetime | Hunk-complete; real leave/rejoin run pending |
| Loadouts | Independent character, shot, SHT, ANM, Bomb callback | Ported per-slot SHT/ANM/Bomb selection and post-SHT sidecar initialization | Hunk-complete; mixed-loadout visual run pending |
| Input | `Player::initParam` selects a logical lane | Eagler adaptation: synchronized lane macros retained; every raw touch/joystick/deathbomb read is restricted to the local slot | Hunk-complete; two-end touch run pending |
| Shooting | SHT power level reads `GetPlayerPower(initParam)` | Ported per-slot Power, ANM offsets, missile normalization and persistent eliminated-player projectiles; Eagler bullet interpolation retained | Hunk-complete; unequal-Power firing run pending |
| Collision and targeting | Every active player participates where sbrik changed P1-only paths | Ported active/absent state guards, per-player Border/death and logical nearest-target behavior; diagnostic invincibility cheat excluded | Hunk-complete; collision runtime matrix pending |
| Death/respawn | Per-player lives, bombs, power loss, spirit/revival | Ported fixed Bomb=3, drift, receiver choice, rank penalty and revival preservation; thprac options remain parameter-gated | Hunk-complete; death/Spirit/revival run pending |
| Life transfer | 20-pixel selection, release-shot plus 90-frame focus hold, deterministic receiver | Ported Spirit priority, low-life/slot tie breaks, charge reset, targeted item trajectory and debit-after-spawn | Hunk-complete; real transfer run pending |
| Power transfer | 20-pixel selection, eight shot taps, transfer 20 power as targeted items | Ported lowest-Power/slot tie break, 24-frame tap window and exact two-big/four-small handoff | Hunk-complete; real transfer run pending |
| Shared Border | One shared activation/break transition for active participants | Ported shared owner, threshold, activation and natural/forced break propagation without duplicate gauge writes | Hunk-complete; real Border run pending |
| Draw/UI helpers | Per-player assets, overlap alpha, prompts and stage labels | Ported final tint/absence/overlap rules; Eagler adaptation makes prompts/names follow interpolated remote presentation only at draw time | Hunk-complete; desktop/mobile visual run pending |
| Registration/cleanup | Per-player chains and resources without touching normal build | Ported per-slot registration, loadout-aware resource reset, contribution reset and null-safe cleanup; vanilla path stays separate | Hunk-complete; stage/retry transition run pending |

### Confirmed Player/resources findings

| Upstream behavior | Prior local behavior | Disposition | Evidence |
| --- | --- | --- | --- |
| Life/Power transfers use states 3/4/5: rise 60 px for 20 frames with collision disabled, then home to P2/P1/P3 | `SpawnItemForPlayer(..., state=0)` only pinned collection and skipped the transfer trajectory | Port exact state mapping and state-machine semantics; preserve Eagler `prevPosition`/VM interpolation publication | Final `ItemManager.cpp` `GetLifeTransferSpawnState`, `SpawnItem`, and `OnUpdate` hunks |
| Transfer power is exempt from unrelated round-robin MAX-power conversion | Helper spawned generically, then repaired the item type afterward | Skip conversion before item initialization when state is 3..5 | Final `ItemManager::SpawnItem` hunk |
| Spirit entry resets Bomb to exactly 3 | Used the selected SHT's `initialBombs` | Port exact fixed value 3 | Final `Player::UpdateDeath` hunk |
| Spirit revival preserves Focus and drift-speed fields | Cleared Focus and both prior speed fields | Remove the undocumented state resets | Final `UpdateLifeTransfer` hunk |
| Transfer prompts force ASCII non-selected mode and restore it | Saved scale/color/GUI only | Save, set to 0, and restore `isSelected`; retain Eagler playfield-to-window coordinate adaptation | Final transfer prompt hunk plus current portable renderer coordinates |
| PracticeRuntime infinite-life/power/auto-bomb switches remain parameter-gated | Eagler/thprac integration around death processing | Eagler adaptation retained pending focused ON/OFF checks | Current `UpdateMultiplayerDeath`; no native sbrik equivalent |
| Stage intro draws slot-colored player names for the first 240 frames when enabled | Session retained `showStagePlayerNames`, but no draw path consumed it | Port draw hunk with Eagler playfield-to-window coordinates and upstream default names | Final `IsStageIntroActive`/`DrawStageIntroPlayerName` hunks |
| Native handshake exchanges custom player names | Browser WebSocket session currently carries no nickname field | Keep default `Player`/`Player2`/`Player3` now; audit optional nickname transport in the transport work class without adding a compatibility gate | Excluded native handshake; pending browser transport disposition |
| `PlayerLoadoutIndex` validates character/Shot before indexing SHT/Bomb tables | Current code computes the same per-slot index inline; `GameplaySession::Configure` rejects character >2 and Shot >1 | Already equivalent under the gameplay-session invariant; retain normal-build globals outside multiplayer | Final `PlayerLoadoutIndex`/`AddedCallback` hunks and current `GameplaySession::Configure` |
| Spirit drift is deterministic and bounded in the lower playfield | Logic is factored into local `UpdateSpiritState` rather than inline in `UpdateState` | Already equivalent factoring; no input or RNG is consumed after Spirit entry | Final `UpdateState` Spirit hunk and current helper body |
| `CutChain` cuts every active player's three chains and resets the active mask | Current loop tests each chain pointer for all three slots, then resets the mask | Already equivalent; broader null-safe iteration has no simulation effect | Final `CutPlayerChains`/`CutChain` hunks and current `Player::CutChain` |
| `OnMissileHit` normalizes non-P1 ANM indices before matching missile explosion types | Switched directly on the slot-offset script index, so non-P1 missiles skipped the 1089–1096 hitbox/speed cases | Port the final upstream expression exactly; do not infer a different P3 formula here | Final `ShtData::OnMissileHit` hunk |
| Existing player bullets/bomb boxes remain damage-capable after their owner leaves an active ship state | `CalcDamageToEnemy` returned zero for Dead/Spirit/Eliminated/temporarily absent owners | Remove the extra local activity guard; lifetime is governed by the upstream bullet/bomb state | Final `Player::CalcDamageToEnemy` hunk |
| Spirit, eliminated, and temporarily absent players cannot graze | `CheckGraze` lacked the multiplayer state guard even though adjacent collision functions had it | Add the same active-state/temporary-absence contract used by final sbrik | Final `Player::CheckGraze` hunk |
| Rollback restore keeps stable player/input identity | Local `OnUpdate` trusted the restored `initParam`, so a mismatched snapshot could route a slot through another player's input lane | Port final pointer-index repair before rollback-sensitive updates; omit native log-once diagnostic | Final `OnUpdate` identity-repair hunk; focused order check |
| Eliminated players' existing bullets remain visible as well as damaging | Local draw returned before `DrawBullets`, although update intentionally kept those projectiles alive | Match final ordering: reject inactive slots, draw bullets, then suppress eliminated ship/Bomb/options | Final `OnDrawHighPrio` ordering hunk; focused order check |
| Same-character and temporary-absence presentation | Local draw had overlap alpha but omitted final tint, absent alpha and Options suppression | Port final draw-only tint/absence behavior and restore the logical sprite color after draw | Final `OnDrawHighPrio` presentation hunks |
| Raw touch belongs only to the local slot | Gameplay-only multiplayer builds could fall through to global `Touch::*` for every `Player`, even though Web netplay normally supplied lane overrides | Gate joystick, direct-touch and deathbomb global reads by `GetLocalPlayerSlot`; remote players consume only synchronized lanes | Eagler adaptation contract and ordinary/multiplayer/Web builds |
| Transfer prompts and stage labels follow remote smoothing | Prompts used authoritative `positionCenter` while the remote ship used bounded presentation smoothing | Draw from `prevPositionCenter.Lerp(..., g_RenderAlpha) + GetPlayerPresentationOffset`; simulation and snapshots remain untouched | Eagler interpolation contract; visual acceptance pending |

## Player/resources hunk disposition

`P01..P126` below are the zero-context final diff hunks for `src/th07/Player.cpp`; `H01..H04` are `Player.hpp`. The helper/body additions come from `d93bc947b01a97633ac0e29db789093979e2c112`, the Power-transfer delta from `22b4bcb8e75a0366f1fb99fc55697b302fa85220`, and final proximity/Bomb-damage/registration corrections from `022c533e0c4205e07c94047a0523cb267ae46447`. `be2c35a` contributes release packaging/diagnostic adjustments only.

| Upstream hunk | Semantic disposition and local landing | Evidence |
| --- | --- | --- |
| P01-P02 | Exclude `GameErrorContext`/native `Netplay.hpp`; port `ItemManager` dependency and map gameplay/session/input behavior to Eagler modules | Native-shell exclusion contract and builds |
| P03 | Port slot storage, shared Border, 2P/3P resource transfer, Spirit selection/drift, proximity rules, stage labels, ANM/effect helpers and active-slot utilities; exclude native handshake, log-once tests and network status UI | 46-check Player/resources contract; prompt/name interpolation adaptation |
| P04-P06 | Port per-slot bullet ANM, final missile normalization and SHT Power selection; retain `prevPos`/`prevAngle` Eagler interpolation | Source contract and three builds |
| P07-P08 | Port per-slot character/Shot fire-timer rules | Mixed-loadout source contract |
| P09-P12 | Port per-player shot/Bomb damage, final 3P Bomb multiplier and surviving projectile lifetime; no owner-state early-zero guard | Damage source contract |
| P13-P21 | Port absent/active-state guards and `this` ownership for Bomb graze, killbox, graze, item and laser collision; exclude native diagnostic invincibility switch | Collision source audit and builds |
| P22-P25 | Port per-player graze source and contribution growth while retaining the one shared score/Cherry totals | Exact `MultiplayerResources` body plus source contract |
| P26-P27 | Port per-player death effect/state; retain existing practice invincibility adaptation. Existing vanilla integrity checksum call is unchanged, not expanded | Builds; hash behavior deliberately untouched |
| P28-P47 | Port every directional/focus/Sakuya/ANM/effect/input-lane hunk; retain analog/direct-touch payloads and gate raw touch to the local slot | Touch ownership contract and three builds |
| P48-P54 | Port per-player Border/Bomb ownership, private Bomb stock and shared rank penalty | Source contract and resource body equivalence |
| P55-P67 | Port per-player death resources, rank/Cherry scaling, Spirit Bomb=3, deterministic drift, transfer item and correct spawn positions; preserve thprac parameter switches | Death/Spirit contracts and builds |
| P68-P76 | Port respawn, Spirit update and single-owner shared-Border gauge countdown | Source contract |
| P77-P84 | Port shared natural/forced Border break and activation propagation, per-slot effect ownership and contribution accounting | Shared-Border contract |
| P85 | Port per-player UI hit-position reset while keeping shared GUI ownership | Build/source audit |
| P86-P90 | Port active/eliminated lifecycle, rollback identity repair, transfer updates and state guards; exclude native damage-injection/proximity smoke tests | Update ordering contract |
| P91-P99 | Port final ship tint, absence, overlap, prompt/name and per-player invulnerability draw behavior; draw eliminated bullets before hiding ship; preserve bounded remote interpolation | Draw ordering/interpolation contract |
| P100 | Port inactive-slot low-priority draw guard; retain Eagler hitbox draw path | Source contract |
| P101 | Port ANM/effect slot helpers; loadout helper is equivalent inline under validated `GameplaySession::Configure` | Loadout contract |
| P102-P117 | Port SHT/ANM/Bomb callbacks, spawn offsets, per-player sizes/effects, shared Border activation and independent resource initialization; preserve normal single-player path | Exact resource body comparison and builds |
| P118-P120 | Port per-slot ANM/SHT cleanup and P1-only shared UI teardown | Build/source audit |
| P121-P125 | Port per-slot chain registration, active/departed mask, post-load resource reset and contribution reset; native diagnostics excluded | Registration contract |
| P126 | Port all-slot null-safe chain cleanup and active-mask reset | Source contract |
| H01-H04 | Port guarded multiplayer include, Spirit/Eliminated states, reuse of two unused layout words and complete slot/helper declarations; ordinary build retains original surface | Ordinary/multiplayer builds and header contract |
| `Multiplayer.hpp` new-file hunk | Constants exactly match final upstream; use `constexpr` and transport-neutral comments only | Constant contract |
| `MultiplayerResources.cpp` new-file hunk | Every final upstream function body is byte-equivalent after the required `ShowFullPowerMode` to Eagler `ShowStatusPopup` API adaptation | Automated full-file comparison |

Player/resources accounting: `126/126` Player C++ hunks, `4/4` Player header hunks and both upstream-only resource files have a disposition. Pre-existing calls that maintain vanilla P1 `GameIntegrityCsum` were not changed during this audit; this is not authorization to add or expand hashing elsewhere.

### Player/resources real-person acceptance

- Run 2P then 3P with different character/Shot combinations and unequal Power. Verify each slot's ANM, Bomb, SHT pattern, starting Bomb count and practical firepower remain independent through death/retry.
- Use touch on one endpoint and keyboard on another. Verify raw touch, joystick, Focus and deathbomb affect only the local slot; inject missing frames and confirm direct-touch delta is not repeated.
- Kill a player while its bullets are active, then eliminate it. Verify existing bullets remain visible and damaging until their normal lifetime ends, while the eliminated ship/options stay hidden.
- Exercise normal hit, laser, graze, Bomb graze, Border hit, temporary absence and return for every slot; verify both peers agree on state and item ownership after rollback.
- Complete life transfers to a Spirit, to the lowest-life player and across a slot tie; complete eight-tap Power transfers with receiver changes during the window.
- Activate and naturally/forcibly break shared Border in 2P and 3P. Verify one gauge, one transition, correct threshold and no duplicate break reward.
- Observe remote ship, options, hitbox, transfer prompts and stage labels under keyboard/touch movement and injected jitter. They must remain aligned and smooth while simulation coordinates remain authoritative.

## Input hunk disposition

`C01..C02` below are the zero-context final diff hunks for `src/th07/Controller.cpp`; `H01..H03` are `Controller.hpp`. `C01`, `C02`, `H01`, and `H03` originate in `d93bc947b01a97633ac0e29db789093979e2c112`; `H02` is the original-input compile wrapper exposed by `22b4bcb8e75a0366f1fb99fc55697b302fa85220`.

| Upstream hunk | Semantic disposition and local landing | Evidence |
| --- | --- | --- |
| C01 | Excluded native DirectInput controller-detection diagnostic. SDL already owns controller discovery, and the hunk has no gameplay/input-lane semantics | Focused contract rejects `GameErrorContext` and the native diagnostic string |
| C02 | Excluded Windows `GetAsyncKeyState` local-co-op sampler. Browser/WebSocket sessions sample one endpoint-local physical lane and map synchronized `FrameInput` records to stable player slots; adding a second process-global keyboard sampler would bypass touch ownership and rollback capture | Focused contract rejects `GetInput2`/`GetAsyncKeyState`; three builds |
| H01 | Ported current/previous per-player logical lanes and edge detection, guarded so the ordinary build retains vanilla `IS_PRESSED_GAME` behavior | Focused lane/edge/fallback contracts |
| H02 | Excluded `TH07_COMPILE_ORIGINAL_INPUT`/`OriginalGetInput` native reccmp wrapper; it is build plumbing rather than multiplayer gameplay and does not exist in the portable SDL host | Header contract and ordinary build |
| H03 | `GetInput2` declaration excluded with C02 | Native sampler exclusion contract |

Input accounting: `2/2` implementation hunks and `3/3` header hunks have a disposition. The Eagler adaptation composes browser keyboard, browser gamepad, SDL controller, touch joystick and touch buttons once, captures the complete local logical frame, then installs synchronized per-player button/analog/touch lanes before simulation. Rollback replay returns the captured frame before sampling SDL/DOM/touch again.

### Input real-person acceptance

- Run two browser endpoints with one touch player and one keyboard/controller player. Touch movement, Focus, Bomb and deathbomb must affect only the local slot and appear identically on both peers.
- Exercise both touch movement modes. Joystick mode must keep the configured deadzone; direct-touch movement must stay bounded when input frames are missing and must not repeat one-frame displacement after the first predicted frame.
- Hold and release Shoot/Focus/Bomb across injected rollback. Per-player pressed edges must occur once on the same logical frame on both peers.
- Pause, resume and cross a stage transition while holding input. The current/previous lane pair must resume without a stuck button or a lost edge.
- Observe remote player/options/attached effects during touch movement and correction. Presentation smoothing may change drawing only; it must never feed interpolated coordinates back into the synchronized input or simulation state.

## Audio hunk disposition

`C01..C02` are the two zero-context final diff hunks for `src/th07/SoundPlayer.cpp`, both from `d93bc947b01a97633ac0e29db789093979e2c112`.

| Upstream hunk | Semantic disposition and local landing | Evidence |
| --- | --- | --- |
| C01 | Excluded native `Netplay.hpp` include. The portable host uses the isolated Eagler side-effect adapter and local browser/desktop audio configuration | Focused native-shell exclusion contract; three builds |
| C02 | Upstream `IsBgmEnabled` is a per-peer native handshake preference, not synchronized gameplay. Eagler already keeps each endpoint's `musicMode` and Web audio lifecycle locally. Retain that path; suppress SFX only during rollback resimulation, not ordinary predicted-forward frames | 9-check audio contract, `SimulateFrame(resimulation)` audit, GameWindow logical-tick queue audit |

Audio accounting: `2/2` hunks have a disposition. No native networking/audio-preference protocol was ported. The Eagler adaptation keeps normal forward-frame BGM commands and SFX audible, prevents rollback resimulation from replaying SFX, advances queues only after a logical tick, and preserves the paired Web audio transition guard.

### Audio real-person acceptance

- Start a stage on two endpoints and enter the Boss transition. Both endpoints must change from stage BGM to Boss BGM; neither may remain on the pre-Boss track.
- Trigger Bomb, Border, item, graze and spell-card sounds while injecting rollback. A sound should be heard for the committed action without a resimulation echo burst.
- Set music/SFX preferences differently on the two endpoints. Each endpoint must honor its own preference without changing session compatibility or synchronized simulation.
- On mobile/browser, begin only after a user gesture, background/foreground the page, and cross one BGM transition. Audio must resume without duplicated streams or a permanently muted engine.

## Enemy hunk disposition

`C01..C33` are the zero-context final diff hunks for `src/th07/EnemyManager.cpp`, from `d93bc947b01a97633ac0e29db789093979e2c112`, with release/final adjustments from `be2c35a` and `022c533e0c4205e07c94047a0523cb267ae46447`.

| Upstream hunk | Semantic disposition and local landing | Evidence |
| --- | --- | --- |
| C01 | Excluded native `Netplay.hpp`; Eagler uses `GameplaySession` and its rollback state | Native-shell exclusion and builds |
| C02-C03 | Ported Stage 4 phase restart and all-active-slot projectile graze/killbox collision; ordinary build retains P1 path | 16-check contract and three builds |
| C04-C10 | Ported per-slot damage/collision collection with deterministic lower-slot tie ownership, focus/Bomb gating and loadout-specific Cherry rules | Damage-owner source contract |
| C11-C15 | Ported active-count Boss damage scaling, Stage 4 chain exemption, proportional damage contribution with integer remainder, score and enemy-defeat ownership | Boss/contribution contract |
| C16-C21 | Ported independent per-player Sakuya/non-Sakuya targeting state and logical enemy positions; no presentation interpolation enters targeting | Target-state contract; touch/interpolation exclusion |
| C22-C23 | Ported both enemy-drop call sites through `SpawnEnemyDrop` so LIFE/Bomb/Power ownership uses the ItemManager distribution rules | Items call-site audit |
| C24-C32 | Upstream diagnostic/trace and native session-seed slices are excluded. Eagler instead seeds RNG from the room seed before gameplay initialization, so `AddedCallback`'s existing deterministic draws remain identical across peers without adding a new protocol or hash path | Startup seed audit and builds |
| C33 | Ported Stage 4 coordinator reset at EnemyManager registration | Lifecycle/reset contract |

Enemy accounting: `33/33` implementation hunks have a disposition. Active players are evaluated from authoritative logical positions; no raw touch, `g_RenderAlpha` or presentation offset is read in `OnUpdate`. The Eagler startup path seeds gameplay RNG before GameManager/EnemyManager registration, preserving the upstream purpose of synchronized random drop selection without importing native Netplay APIs. No hash behavior was added, removed or expanded.

### Enemy real-person acceptance

- Run 2P and 3P with distinct Power, character and Shot. Hit the same ordinary enemy and Boss from different positions; damage, Cherry attribution, contribution totals, score and enemy-defeat counters must agree on both endpoints, including exact damage ties resolving to the lower slot.
- Use Reimu/Sakuya targeting with active, Spirit, eliminated and temporarily absent slots. Each slot's target marker must update independently from logical enemy/player coordinates and never from the smoothed remote sprite.
- Exercise bullet graze/killbox collision with multiple active ships and overlapping trails. Every active slot may graze or kill, and the enemy must not disappear or lose life solely because one player is absent.
- Trigger LIFE/Bomb/Power drops from ordinary and Boss enemies with two then three players. Confirm `SpawnEnemyDrop` produces the correct separated ownership and no copy is reserved for an inactive slot.
- Run Boss damage at 2P/3P with Stage 4 chain OFF and ON. Normal Boss phases use active-count scaling; chained character phases retain single-player life/damage semantics and reset on stage registration.
- Use direct touch on one endpoint while another uses keyboard and inject delayed input/rollback. Targeting and damage must remain equal across peers; interpolation may affect drawing only.
- Repeat representative enemy patterns in ordinary single-player to confirm vanilla P1 damage, targeting, Cherry and drop behavior remains unchanged.

Remaining Enemy risk: focused tests and builds do not replace a real two-end damage/drop run, especially Stage 4 chained cards and touch/rollback collisions.

## MainMenu hunk disposition

`C01..C17` are the zero-context final diff hunks for `src/th07/MainMenu.cpp`, from `d93bc947b01a97633ac0e29db789093979e2c112`.

| Upstream hunk | Semantic disposition and local landing | Evidence |
| --- | --- | --- |
| C01 | Excluded native `Netplay.hpp`; browser menu reads the transport-neutral gameplay session only | Native-shell exclusion and builds |
| C02 | Ported a portable `StartSelectedGame` helper for stage/replay/audio cleanup; native quick-start entry is excluded because the browser launcher already dispatches negotiated gameplay | Start-helper and ordinary fallback contract |
| C03-C04 | Excluded native quick-start/demo-disable APIs. Existing Eagler stage harness/launcher owns this entry; ordinary demo behavior remains unchanged | Source exclusion and build contract |
| C05-C07 | Adapted networked option-menu suppression to `MultiplayerGameplay::IsMultiplayer`; ordinary/local menu retains all option controls | Menu guard contract |
| C08-C09 | Existing difficulty cursor follows Eagler supervisor configuration; native initial-difficulty handshake helper is excluded | Ordinary build and configuration audit |
| C10 | Ported selected-shot launch through the common helper while preserving Normal/Extra/Phantasm stage mapping | Launch mapping contract |
| C11 | Ported runtime-only multiplayer unlocks in practice update and draw paths; save data remains untouched | Two unlock call-site checks |
| C12-C13 | Excluded native demo-disable state; existing THPRAC parameter-gated idle behavior remains | Demo behavior audit |
| C14-C15 | Ported cursor wrap while skipping the endpoint-local options row in multiplayer; sound feedback and ordinary wrap remain | Cursor contract |
| C16-C17 | Existing menu renderer keeps Eagler `prevPos`/current interpolation and does not feed presentation back into gameplay | Interpolation/raw-touch exclusion and Web build |

MainMenu accounting: `17/17` implementation hunks have a disposition. Native network launcher/UI APIs are excluded rather than faked; browser runtime quick-start remains outside the menu state machine. Multiplayer practice visibility uses the existing runtime-only unlock facade, ordinary score/unlock behavior stays vanilla, and menu drawing keeps the current interpolation path. No hash behavior was touched.

### MainMenu real-person acceptance

- Enter ordinary Start/Extra/Practice/Replay/Options and verify cursor wrap, launch stage mapping, audio queue drain and save/unlock behavior remain vanilla.
- Enter a 2P/3P browser room and confirm the local options row is skipped, stage/character/Shot selection comes from the room descriptor, and no peer can alter another endpoint's local display/audio preference through menu state.
- Open practice under peers with different local `score.dat` unlocks. Both peers must see the runtime-forced content without either save file being modified.
- Use keyboard on one endpoint and touch on another while navigating menus and launching a stage. Each input lane must remain local; menu transitions must not sample or replay raw touch during rollback.
- Observe selected rows and title/menu sprites at non-integer render cadence. Their draw interpolation may smooth presentation but must not alter cursor, difficulty, character or Shot simulation values.

Remaining MainMenu risk: the browser launcher currently bypasses the native connection UI and quick-start APIs; a real hosted menu-to-stage run is still needed to verify the room descriptor and loadout handoff end to end.

## ResultScreen hunk disposition

`C01..C28` are the zero-context final diff hunks for `src/th07/ResultScreen.cpp`, from `d93bc947b01a97633ac0e29db789093979e2c112` with final adjustments in `022c533e0c4205e07c94047a0523cb267ae46447`.

| Upstream hunk | Semantic disposition and local landing | Evidence |
| --- | --- | --- |
| C01 | Excluded native `Netplay.hpp`; result behavior uses `GameplaySession` | Native-shell exclusion and builds |
| C02-C09, C11-C21, C24-C25 | Upstream encoding-only text corrections are represented by Eagler's UTF-8/localization strings; no result gameplay branch is lost | Text/source audit |
| C10 | Ported multiplayer result lifecycle: skip the unusable P1-only replay-save prompt and leave through state 18 | Prompt-state contract |
| C22-C23 | Ported the same skip at result initialization/name-entry transition; ordinary result names remain available | AddedCallback contract |
| C26-C27 | Ported runtime-only persistent-result suppression for multiplayer. Score data is still parsed for display but never overwritten by a multiplayer economy | WriteScore/DeletedCallback contract |
| C28 | Kept the portable result cleanup chain and replay/save behavior; native NoSave flag is excluded, while ordinary persistence and EAGX replay handling remain | Three builds and source audit |

ResultScreen accounting: `28/28` implementation hunks have a disposition. Multiplayer results read local score data for display, do not write single-player score/life tables, and do not ask for a replay that cannot represent multiple synchronized lanes. Ordinary results retain score writes and replay naming. Existing vanilla score checksums were not modified; no hash or compatibility behavior was added.

### ResultScreen real-person acceptance

- Finish an ordinary stage and verify score, spell history, replay-name prompt, replay save and `score.dat` persistence remain vanilla.
- Finish 2P and 3P runs, including a full-stage result and a death/result path. Confirm no P1-only replay-save prompt blocks the room and neither endpoint overwrites its single-player score/life tables.
- Open result statistics with peers whose local score files differ. Display parsing may remain endpoint-local, but no result transition may alter the other's synchronized gameplay state or room settings.
- Use touch on one endpoint during result/menu transitions and keyboard on another. Touch must not leak into result navigation or replay/save decisions.
- Verify result screens and transitions remain smooth at high-refresh interpolation; no draw smoothing may affect score, replay or persistence decisions.

Remaining ResultScreen risk: focused contracts and builds do not replace a real ordinary replay round-trip or hosted multiplayer result transition.

## Supervisor hunk disposition

`C01..C48` are the zero-context final diff hunks for `src/th07/Supervisor.cpp`; `H01..H02` are `Supervisor.hpp`, from `d93bc947b01a97633ac0e29db789093979e2c112`.

| Upstream hunk | Semantic disposition and local landing | Evidence |
| --- | --- | --- |
| C01 | Excluded native `Netplay.hpp`; browser rollback/input adapters own transport and synchronized lane installation | Native-shell exclusion and input/rollback contracts |
| C02 | Ported/retained per-slot logical game-input arrays while keeping raw physical sampling endpoint-local for Eagler touch/keyboard ownership | Header/input contract |
| C03-C05 | Native local-coop sampler/auto-input behavior is excluded; `Supervisor::OnUpdate` retains vanilla P1 raw capture and `NetplayInput` commits synchronized P2/P3 lanes before gameplay | Input lane contract and builds |
| C06-C08 | Encoding-only timing/error strings are represented by portable UTF-8 diagnostics; fixed-tick/Web pacing remains in Eagler GameWindow | Three builds and runtime audit |
| C09-C11 | Native DirectInput setup and diagnostics are excluded; SDL/browser controller and touch setup remains the host path | Native API exclusion |
| C12-C18 | Portable data/config/audio error paths are already equivalent after localization; no native wrapper is imported | Source/build audit |
| C19-C22 | Ported deterministic RNG lifecycle through Eagler's room-seeded startup path: Supervisor may initialize wall-clock state, but the session launcher overwrites the seed immediately before GameManager/EnemyManager registration | Session-seed ordering test |
| C23-C44 | Portable lifecycle, BGM, Web audio, render setup and config parsing remain local; network-only preference/NoSave APIs are not faked in Supervisor | Audio/Web lifecycle contract |
| C45-C47 | Multiplayer life/slow-mode normalization is retained in GameManager's final registration/update path, where the gameplay session exists; config presentation remains endpoint-local | GameManager and Supervisor tests |
| C48 | Native `ApplyAudioPreferences` and NoSave config-write gate are excluded; Eagler audio preference and save boundaries are handled by local Web/ResultScreen adapters | Audio/ResultScreen contracts |
| H01-H02 | Port stable P1/P2/P3 game-input lane declarations and compatibility macros; retain scalar raw P1 fields because raw touch/controller sampling is intentionally local-only | Header/input/rollback contracts |

Supervisor accounting: `48/48` implementation hunks and `2/2` header hunks have a disposition. The mature Eagler rollback path commits synchronized game lanes in `NetplayInput` before simulation, while Supervisor owns only the endpoint-local raw P1 sample and fixed host lifecycle. The session seed is installed before online stage dispatch; no WinSock/thread/LowLatency shell or hash behavior was added.

### Supervisor real-person acceptance

- Start ordinary desktop, gameplay-only multiplayer desktop and browser Web sessions. Verify 60 Hz simulation, menu/stage/result transitions, audio startup and clean shutdown.
- Use one touch endpoint and one keyboard/controller endpoint. Raw input must affect only the local lane; synchronized P2/P3 game lanes must be installed before Player/GameManager calculations, including rollback and pause edges.
- Start sessions with different local life/slow-mode settings. Both peers must use the multiplayer two-life/no-slow baseline; ordinary local play must retain its configured settings.
- Inject packet delay and a short stall around stage/menu transitions. Recovery must preserve logical input edges, RNG sequence and BGM transitions while presentation remains smooth and bounded.
- Background/foreground a browser page and resume audio after a gesture. Audio queue behavior may differ by endpoint preference, but synchronized gameplay and session seed must not.
- Run ordinary and multiplayer result/config paths and verify local config/score persistence boundaries remain as documented.

Remaining Supervisor risk: focused contracts and builds do not replace a live two-end touch/keyboard run through menu, stage, rollback, audio and result transitions.

## Entry/main hunk disposition

`C01..C11` are the zero-context final diff hunks for `src/th07/main.cpp`. The native entry shell originates in `d93bc947b01a97633ac0e29db789093979e2c112`; `022c533e0c4205e07c94047a0523cb267ae46447` adds the early-failure compatibility-log flush inside C02.

| Upstream hunk | Semantic disposition and local landing | Evidence |
| --- | --- | --- |
| C01 | Excluded native `Netplay.hpp`; Eagler startup uses guarded `GameplaySession`, browser options and the isolated WebSocket/rollback adapters | Native-shell exclusion contract and three builds |
| C02 | Excluded Win32 command-line connection initialization, modal error UI and compatibility-failure logging. Browser room setup occurs before the gameplay iframe launch; no EXE/resource/protocol/session hash or connection-rejection gate is imported | Browser room-mode/session contract; hash-boundary check |
| C03 | Excluded the Win32 single-instance override. Browser tabs/iframes and SDL host lifetime are endpoint-local and may coexist without a native mutex/window probe | SDL callback lifecycle and native-API exclusion |
| C04 | Excluded the native `SetWindowTextA` branding hunk. Canvas/window title is host presentation and not gameplay/session state | Portable SDL/Web host contract |
| C05, C09 | Excluded peer/test-selected fullscreen and forced-windowed behavior. SDL/browser fullscreen remains local, including deferred Alt+Enter handling, and never enters the room descriptor | Fullscreen/event source contract and builds |
| C06-C07 | Excluded the native automated-test timer. Existing Eagler harnesses dispatch through SDL callbacks and external probes own their timeout; no timer advances or terminates synchronized gameplay | SDL iterate/harness contract |
| C08, C10 | Upstream `NoSave` is a local/test safety option, not blanket multiplayer persistence semantics. Eagler dev self-tests already avoid config writes where required; multiplayer score/replay writes are suppressed in `ResultScreen`/`ReplayManager`, while endpoint-local display/audio config remains writable | Entry and prior Result/Replay contracts |
| C11 | Adapted shutdown ordering to the SDL callback/Web lifecycle: release chains/audio/rendering, write allowed local config, flush diagnostics, then notify `EaglerTouhouGameExited`. Native Netplay measurement shutdown is excluded with its shell | SDL quit-order and Web-exit contract |

Entry accounting: `11/11` hunks have a disposition. Before Stage 1 dispatch, the browser production path installs the room RNG seed, player count, stable local slot, every active character/Shot loadout and the host-authored Stage 4 option, then validates the transport-neutral gameplay session. The existing vanilla `GameWindow::ChecksumExecutable()` call predates this work and was deliberately left untouched; no hash behavior was added, removed or expanded.

### Entry/main real-person acceptance

- Launch ordinary desktop and exit from title, gameplay and result. Confirm local fullscreen/audio configuration, score/replay behavior and clean SDL teardown remain vanilla.
- Launch a real 2P then 3P browser room with mixed loadouts. Confirm each iframe receives the same seed, player count, loadout vector and Stage 4 option before frame zero, while each endpoint receives only its own stable local slot.
- Use touch on one endpoint and keyboard/controller on another from room launch through Stage 1. Startup must not sample touch for a remote lane; smoothing begins only after logical players exist and never feeds the session descriptor or RNG.
- Toggle fullscreen locally, background/foreground one browser, resume audio and close one endpoint. These endpoint-local lifecycle actions must not alter the peer's gameplay configuration or leave duplicated audio/render loops.
- Exercise a multiplayer result/exit and an ordinary result/exit. Multiplayer must not write incomplete replay or single-player score progress; ordinary play must still persist them. Endpoint-local `th07.cfg` preferences may remain writable.
- Reject an invalid room descriptor through the browser launcher/session validator and confirm the failure is reported without adding any EXE/resource/protocol/session hash comparison or automatic repair.

Remaining Entry risk: source contracts and builds do not prove the actual host-to-iframe room descriptor, user-gesture audio activation or one-peer-close behavior. Those remain part of the final two-end browser acceptance run.

## Netplay mature rollback function disposition

The final `src/th07/Netplay.cpp` is one 15,000-line upstream-only Git hunk, so its hunk count is not meaningful function evidence. The table below instead freezes final-file function/line groups from `022c533`. Its history is `d93bc94`, `be2c35a`, `43d0b61`, `5645bee`, `22b4bcb`, `4beceb8`, `ecd4483`, and `022c533`. Direct file import is forbidden because those same bodies combine gameplay rollback with WinSock, Windows UI/thread/timing, LowLatency, native UDP, compatibility hashes, state hashes and automatic resynchronization.

| Final upstream function group | Local landing/disposition | Focused evidence |
| --- | --- | --- |
| `ResetInputRings`, `StoreRemoteInputs`, `SendInputs` (`4790`, `6064`, `10437`) | Mapped to `RollbackCore`: per-slot input/used rings, duplicate/conflict handling, earliest mismatch, peer ACK shrink and final upstream 32-frame application-level redundant tail. Browser production constructs one input packet per remote endpoint so 3P ACK shrink and frame advantage remain peer-relative; RTC sends it directly to that endpoint and emergency WebSocket relay uses a transport-only targeted envelope which the relay strips before delivery. While a missing remote frame stalls simulation, the browser driver rebuilds and retries the already-scheduled current-frame packet every three driver ticks; it never calls the physical keyboard/controller/touch sampler again | Core loss/reorder/duplicate/ACK/32-tail tests; targeted relay routing test; focused RTC/fallback/3P browser transport tests |
| `TryGetRemoteInput`, prediction helpers (`10054`, `10114`, `12474`) | Predict before waiting, cap at twelve recoverable frames, then stall. Final TH07 held mask is direction/Focus/Shot/Skip; Bomb/Menu and synchronized touch-Bomb predict released. Keyboard direction continues for at most three missing frames, then becomes neutral while the full rollback window and other safe held controls remain active. Eagler direct-touch behavior is unchanged: consume displacement once on the first missing frame and zero later missing-frame deltas | Direction-horizon, edge and touch contracts plus core tests |
| gameplay readiness and shared UI (`9648..9716`, `12474..12723`) | Browser session HELLO/READY and exact frame-zero input form the gameplay barrier. Pause/retry/score transition frames continue through the rollback owner but require exact remote input rather than prediction | Session/core tests and driver contract |
| rollback snapshot save/restore and BombEffects (`9526..10053`) | Replaced 16 MiB full copies with the existing sparse first-write journal, while retaining all deterministic manager/player/RNG/input/Stage4 state, heap BombEffect reconstruction and stage invalidation. Draw-only render alpha/remote smoothing clocks remain endpoint-local; logical previous/current interpolation fields rewind with their owning objects | Journal tests, state capture contract and three builds |
| final 3P BombEffect capacity (`9718..9783`) | Corrected local production cap from 256 to final upstream 1024. Overflow remains a hard capture failure rather than silently dropping a partial snapshot | Capacity contract and Web rebuild |
| `PerformPendingRollback` (`10250..10436`) | Restore from the earliest mismatch, rebuild every affected logical frame and its future journal entries, install captured synchronized lanes and prevent device re-sampling. An expected unavailable old-stage snapshot is reported and abandoned so it cannot freeze the room; journal corruption, no-progress and failed resimulation remain fatal | Core rollback, journal restore, failure-policy and driver contracts |
| normal-forward vs replay side effects | Eagler `SideEffects::SetSpeculative` is true only during rollback resimulation. Replay writes, SFX/BGM requests and one-shot player feedback are suppressed/deduplicated there, while ordinary predicted-forward frames keep audio and presentation behavior | Audio/Replay/Player tests plus rollback contract |
| network stall pacing and time sync | A stalled logical tick does not consume the fixed-step accumulator. Presentation continues with a six-tick bounded backlog/catch-up and WebAudio pumping. Wall-clock time sync keeps a trimmed sample window per remote endpoint and, following GGPO/GGRS multi-endpoint policy, uses the largest local lead as the room pacing recommendation instead of averaging unrelated links | GameWindow prior test, rollback contract and short live 3P max-endpoint assertion |
| three-player routing | Stable P1/P2/P3 lanes use a browser full-mesh RTC transport in production; all three edges must have reliable control plus unordered input DataChannels before frame zero, otherwise the whole room uses the emergency WebSocket relay. Each peer maintains independent confirmed/predicted history, ACK and timing state per remote slot; tie-breaking and session player count remain deterministic | Three-player core test, LAN stage driver, targeted relay test and focused 3P RTC full-mesh PASS |
| native connection/UI/quick-start/display/test/FPU/LowLatency functions | Excluded. Browser launcher, room descriptor, SDL callbacks, BrowserPeerTransport + signaling/emergency relay, local display/audio/touch and external test harness own those responsibilities | Entry/Supervisor/GameWindow dispositions |
| peer absence/resume/departure (`5341..5917`) | Excluded with the native Host-authoritative UDP lifecycle/relay shell. The current broadcast WebSocket room has no equivalent authority protocol, so inventing local absence timing would diverge. A remote close or 15-second non-advancing required lane now ends the room visibly instead of silently freezing; 3P healthy-pair continuation is not claimed | Close/timeout contract; relay restart ended both live 2P endpoints visibly, production host/mobile background-close check remains |
| executable/resource/protocol/session compatibility identity (`2290..2853`) | Excluded in full. No compatibility hash, join rejection or compatibility repair was added | Hash-boundary source contract |
| logical/detailed state hashes and RNG resync (`7337..9525`, `12404..12473`) | Skipped. Hash comparison, mismatch gating, RNG correction and automatic resync are not part of the portable rollback mapping. Pre-existing runtime partition diagnostic capture in the local tree was not changed or expanded in this work | Core/state exclusion contract; user authorization still required for any change |

Rollback accounting: the upstream-only `Netplay.cpp`/`.hpp` hunks are not copied, but every rollback-relevant final function class now has a concrete disposition. The new corrections in this audit are the TH07 predictable-button mask, touch-Bomb edge clearing, 32-frame redundant tail, stalled-current-frame retransmission without device resampling, 1024-effect 3P capacity, non-freezing unavailable-snapshot policy and explicit remote-close failure. No hash behavior was changed.

### Netplay mature rollback real-person acceptance

- Run 2P and 3P browser rooms at delay 0 with relay jitter and deliberate dropped forwarded messages. Confirm frame zero waits for every real peer, gameplay predicts immediately afterward, input history recovers loss, rollback completes and the fixed 60 Hz game does not remain slow.
- Hold direction/Shot/Focus/Skip remotely while dropping frames. They may predict held. Tap Bomb/Menu/touch-Bomb during the same loss; no edge may repeat, and the exact late edge must rewind to one logical activation.
- Use direct-touch movement on one phone while a peer uses keyboard. Drop two or more consecutive frames: only the first missing frame may reuse the displacement, later missing frames must be zero, and remote draw smoothing must remain bounded without entering snapshots.
- Pause/resume and cross a stage transition under jitter. Shared UI frames must wait for exact input, old-stage journal history must be discarded, and an impossible stale correction must report/continue rather than freeze.
- Overlap three Bombs and dense effects. Rollback must reconstruct every live BombEffect within the 1024 ceiling, without duplicated SFX, BGM loss or a presentation/simulation position mismatch.
- Cleanly close and abruptly kill a peer. A direct remote WebSocket close or a required lane that stops advancing for 15 seconds must end the browser room visibly, never leave a silently frozen canvas. This WebSocket architecture intentionally does not claim native 3P healthy-pair continuation.

Remaining rollback risk: a real in-app browser run now covers browser scheduling, 2P/3P loss, frame-zero recovery, pause/resume, physical keyboard capture and rollback. It does not prove phone touch gestures, audible side effects, remote visual smoothing, stage-transition journal invalidation or production host-overlay integration. Native Host-authoritative UDP absence/resume/departure is an explicit shell exclusion; visible whole-room termination has been observed after relay restart, but abrupt phone/background disconnect still needs the production host path.

## Diagnostics hunk disposition

`C01..C03` are the zero-context final diff hunks for `src/th07/GameErrorContext.cpp`, originating in `d93bc947b01a97633ac0e29db789093979e2c112` with release adjustments in `be2c35a`.

| Upstream hunk | Semantic disposition and local landing | Evidence |
| --- | --- | --- |
| C01 | Excluded in full. It adds a native long-run log recycling policy whose priority/discard lists explicitly include hash traces, state verification, RNG mismatch/resync, rollback-native timing and automatic-repair diagnostics. Splitting out a guessed subset would no longer reproduce an upstream semantic unit | 8-check exclusion contract; frozen diff inspection |
| C02 | Excluded because `Log` depends on C01's hash/resync-aware room-making helpers. Retain Eagler's bounded `vsnprintf`, localization lookup and fixed-buffer behavior | Formatter/localization/buffer contracts; three builds |
| C03 | Excluded because `Fatal` depends on the same C01 policy. Retain the portable bounded formatter and error-dialog flag | Fatal-dialog and formatter contracts |

Diagnostics accounting: `3/3` hunks have an explicit exclusion disposition. No hash logging, state comparison, RNG resync or automatic repair was added or expanded. Existing non-netplay portable diagnostics remain intact and are still bounded and localized.

### Diagnostics real-person acceptance

- Trigger a known missing/invalid resource in a disposable local run. Confirm the localized error is written to `log.txt` and a fatal path still requests the SDL error dialog.
- Run an ordinary stage long enough to produce normal diagnostics and exit cleanly. Confirm the existing log header/footer and fixed-buffer behavior remain intact.
- This work class intentionally has no hash/desync acceptance item: adding such behavior requires separate user authorization.

## Enemy ECL instruction hunk disposition

`C01` is the only zero-context final diff hunk for `src/th07/EnemyEclInstr.cpp`, from `d93bc947b01a97633ac0e29db789093979e2c112`.

| Upstream hunk | Semantic disposition and local landing | Evidence |
| --- | --- | --- |
| C01 | Ported exactly for multiplayer: Youmu's redirected bullets aim at the closest active player to each bullet rather than always P1. The ordinary build retains vanilla P1 targeting. Selection uses authoritative logical bullet/player coordinates; presentation interpolation is deliberately absent | 6-check source/model contract and three builds |

Enemy ECL instruction accounting: `1/1` hunk is ported. The current helper also excludes inactive/temporarily absent player slots according to the Player/resources activity contract, while stable slot order resolves equal-distance ties deterministically.

### Enemy ECL instruction real-person acceptance

- Reach the Youmu pattern that redirects curved bullets. Place P1 and P2 on opposite sides and verify each redirected bullet aims at the closest active ship on both endpoints.
- Temporarily remove or eliminate the nearer player before redirect. The bullet must choose the remaining active player without targeting the absent slot.
- Move the nearer player with direct touch while the peer uses keyboard and inject rollback. Both peers must choose the same target from logical coordinates; remote presentation smoothing must not change the chosen angle.
- Run the same pattern in ordinary single-player mode and confirm vanilla P1 targeting is unchanged.

## ECL hunk disposition

`C01..C30` are the zero-context final diff hunks for `src/th07/EclManager.cpp`; `H01` is the single `EclManager.hpp` hunk. Gameplay originates in `d93bc947b01a97633ac0e29db789093979e2c112`; `5645bee` contributes the host-authoritative option context, and `022c533e0c4205e07c94047a0523cb267ae46447` contains the final Stage 4 and diagnostic form.

| Upstream hunk | Semantic disposition and local landing | Evidence |
| --- | --- | --- |
| C01 | Excluded native `Netplay.hpp`; ECL uses the transport-neutral `GameplaySession` facade | Native-shell exclusion and three builds |
| C02 | Upstream encoding repair is already represented by Eagler's UTF-8 error text | Load/error-path audit |
| C03, C21 diagnostic portions, C22 diagnostic portion, C23, C27 | Excluded native ECL/Stage 4 trace writers and log counters. They change no simulation behavior and depend on the excluded native trace shell | Focused native-trace exclusion contract |
| C04-C05, C11-C12 | Ported Stage 4 life-read transition checks in integer/float ECL paths. Shot-type reads retain the actual Eagler/thprac effective P1 selection where parameter-gated; native trace-only calls are excluded | Source contract and three builds |
| C06-C10, C13-C20 | Ported every integer, float and writable-pointer player position/angle/distance access through the closest active logical player, with vanilla P1 fallback in ordinary builds | 19-check focused contract; exact call-site audit |
| C21 gameplay portion | Ported the exact character/difficulty table, distinct-character stable-slot queue, coordinator life, spell index, phase restart and abort guards. Corrected a prior local deviation: the feature is host-authored and OFF by default, rather than enabled for every multiplayer run | Table/queue/restart/default contracts |
| C22 gameplay portion | Ported `NoteStage4ChainedSpellcard` after the ECL-declared spell index is installed | BeginSpellcard ordering contract |
| C24-C25 | Ported closest-active logical targeting for aimed movement and laser angle | Source contract |
| C26, C29 | Ported both side/exit decisions through the same closest-active logical target | Exact two-call contract |
| C28 | Ported ECL item creation through `SpawnEnemyDrop`, preserving multi-recipient LIFE/Bomb drop rules | Source contract and prior Items evidence |
| C30 | Ported the late spell condition from P1-only Bomb state to any active player Bombing | Source contract |
| H01 | Ported all Stage 4 declarations and seven simulation-state fields behind the multiplayer gameplay guard; every field is included in rollback capture/restore | Header and rollback-journal contracts |

ECL accounting: `30/30` implementation hunks and `1/1` header hunk have a disposition. Browser room configuration exposes `netplayStage4BossChain`, validates it as a boolean and defaults it to false. The room/host descriptor must supply one value to all endpoints before frame zero; it is not a hash, connection rejection rule or automatic repair mechanism. ECL consumes authoritative logical coordinates only: it does not sample raw touch, consume `g_RenderAlpha`, or use remote presentation offsets. No hash behavior was added, removed or expanded.

### ECL real-person acceptance

- Run representative aimed movement, laser, angle, distance and side-exit scripts with P1/P2/P3 at distinct distances. Each event must choose the closest active logical player, exclude absent/Spirit/eliminated slots and resolve equal distances by stable slot order on both peers.
- Repeat those patterns while the nearest player uses direct touch and the peers receive delayed inputs. Target selection must agree after rollback; only drawing may be smoothed, so no target may flip merely because a remote sprite is visually interpolating.
- Run Stage 4 with the room option OFF. The naturally selected character card must occur once, matching upstream default behavior.
- Run Stage 4 with the host option ON using two then three distinct characters, and then duplicate characters. Each distinct active character's second Boss card must run once in natural-card-first then slot order; shared later cards must remain intact.
- During the Stage 4 card, force a phase timeout/defeat, Bomb overlap and rollback correction. Coordinator life, timer, active spell index and queue position must restore identically on every endpoint.
- Exercise an ECL LIFE/Bomb drop and a late-stage Bomb-sensitive spell with P2/P3. Drops must use the multiplayer enemy-drop distribution and any active player's Bomb must trigger the intended branch.
- Repeat representative scripts in ordinary single-player. All player-variable reads and Bomb checks must retain vanilla P1 semantics.

Remaining ECL risk: the current browser room descriptor is responsible for delivering the host's Stage 4 option to every iframe. A real hosted 2P/3P run must verify that propagation; this work deliberately did not add a protocol/hash compatibility gate.

## Assets/ANM hunk disposition

`I01..I04` are the zero-context final diff hunks for `src/th07/AnmIdx.hpp`, `C01..C16` for `AnmManager.cpp`, and `H01..H04` for `AnmManager.hpp`. All originate in `d93bc947b01a97633ac0e29db789093979e2c112`.

| Upstream hunk | Semantic disposition and local landing | Evidence |
| --- | --- | --- |
| I01-I04 | Ported exact P2/P3 player and face file ids and script offsets: files 47/48 and 40/50, player blocks `0x500/0xa00`, face blocks `0x5a0/0xaa0`. FACE3 stays beyond FACE2's child chain and each face block remains `0xa0` after its player block | Exact constant and offset-distance contracts |
| C01 | Ported expanded 2816-slot initialization via shared `ANM_SPRITE_SLOT_COUNT` | Constructor-capacity contract |
| C02-C04, C06-C10, C12, C14, C16 | Already semantically equivalent portable diagnostics. Eagler keeps readable Japanese strings, localization-compatible `\n`, SDL resources and its existing error path rather than Direct3D-specific byte text | Source contract and three builds |
| C05, C15 | Ported file load/release bounds via shared 56-slot `ANM_FILE_SLOT_COUNT` | Load/release bound contracts |
| C11, C13 | Ported sprite/script load bounds via shared 2816-slot `ANM_SPRITE_SLOT_COUNT` | Two load-bound contracts |
| H01-H03 | Ported 56 file slots and 2816 sprite/script/index slots using `constexpr`; all arrays and bounds consume the same constants | Header/source capacity contracts |
| H04 | Excluded the native byte-size `C_ASSERT`. Eagler's renderer adds portable texture handles, interpolation fields, extrusion atlases and Web caches, so claiming the sbrik Direct3D layout size would be false; the semantic capacities are independently checked | Portable-layout exclusion and builds |

Assets/ANM accounting: `4/4` index hunks, `16/16` implementation hunks and `4/4` header hunks have a disposition. Player registration loads and releases independent P1/P2/P3 file slots and offsets. Eagler's SDL/GLES renderer, texture extrusion, Web transition caches, `prevShakeOffset` and draw-only interpolation remain unchanged.

### Assets/ANM real-person acceptance

- Run 2P and 3P with all characters and both Shot types represented. Verify each ship, options, shots and hitbox use the correct player's ANM without borrowing another slot's texture.
- Bomb with P2 and P3 for every represented character/Shot. Confirm the correct face/cut-in appears, especially P3 scripts in the `0xaa0` face block, with no negative source-file access or crash.
- Die, revive, retry and cross a stage transition. Confirm each player's ANM file is released/reloaded independently and P1 assets remain intact.
- On Web/mobile, exercise title/result/music transitions before and after multiplayer play. Persistent Web transition caches must not overwrite player/face blocks.
- Observe remote ship/options/cut-in under touch movement and rollback. ANM ownership remains logical; presentation interpolation changes only the final draw position.

## Effects hunk disposition

`C01..C14` are the zero-context final diff hunks for `src/th07/EffectManager.cpp`; `H01..H02` are `EffectManager.hpp`. All originate in `d93bc947b01a97633ac0e29db789093979e2c112`.

| Upstream hunk | Semantic disposition and local landing | Evidence |
| --- | --- | --- |
| C01-C03 | Ported all-slot ownership for normal, Focus and Border attached effects. Simulation follows each owner's authoritative `positionCenter`; the upstream one-shot `GameErrorContext` verification log and static log flags are excluded | Attachment/ordinary-fallback/native-log contracts |
| C04-C05 | Ported allocation-failure sentinel from slot 408 to 413 for both particle spawn paths | Exact sentinel contracts |
| C06 | Ported update range `0..412`, covering the 400-particle pool plus all 13 fixed P1/P2/P3 effect slots while leaving 413 as sentinel | Update-range contract |
| C07-C14 | Ported per-player Focus overlap alpha semantically into Eagler's texture-sorted draw loop. Color is saved/restored for every layer; attached effects additionally receive the owner's bounded presentation offset after `prevPos.Lerp`, only during draw | Draw-alpha/restore/interpolation/offset contracts |
| H01 | Ported the exact 414-effect storage: pool 0..399, fixed player effects 400..412, sentinel 413 | Header-capacity contract |
| H02 | Excluded the native byte-size assert. Eagler's `Effect` carries `prevPos` and portable renderer state, so the Direct3D structure size is not a valid contract | Portable-layout exclusion and builds |

Effects accounting: `14/14` implementation hunks and `2/2` header hunks have a disposition. New effects publish `prevPos`/VM previous state, each logical tick advances them, and draw interpolation plus remote presentation offset never changes `pos1`. The rollback journal touches every live slot through 412; no hash behavior was modified during this work class.

### Effects real-person acceptance

- Focus and activate/break Border independently with P1/P2/P3. Each ring/attached effect must follow its owner, never P1 by default, and survive overlapping players without slot corruption.
- Overlap the local and remote players. Only the remote Focus ring should fade according to overlap alpha; separating them must restore full opacity without permanently mutating VM color.
- Move a remote player with touch during jitter/rollback. Ship, Focus ring, Border/Cherry attachment and options must remain aligned under the same presentation offset, while collision and later snapshots still use logical positions.
- Saturate ordinary particles during multiple Bombs and deaths. Allocation failure must use sentinel 413 without overwriting fixed player effects.
- Die, enter Spirit, revive, temporarily leave/return and retry. Fixed effects must be removed/recreated for the correct stable slot on both endpoints.

## Replay hunk disposition

`C01..C08` are the zero-context final diff hunks for `src/th07/ReplayManager.cpp`, all from `d93bc947b01a97633ac0e29db789093979e2c112`.

| Upstream hunk | Semantic disposition and local landing | Evidence |
| --- | --- | --- |
| C01 | Excluded native `Netplay.hpp`; Eagler uses isolated input/side-effect adapters and transport-neutral gameplay session state | Native-shell exclusion contract |
| C02 | Ported/adapted three logical lanes. WebSocket netplay installs current/previous synchronized lanes before simulation and ReplayManager must not advance them twice; non-netplay multiplayer advances all previous lanes, samples P1 and zeros unsupplied P2/P3 | Input-edge/fallback contracts and three builds |
| C03 | Ported exact vanilla replay policy: playback supplies P1 and clears P2/P3 current/previous lanes. Existing EAGX touch frame application occurs before the vanilla button word | Demo/EAGX ordering contracts |
| C04-C05 | Ported shared Border threshold for replay initial state. A prior local fixed 50000 cap was corrected to `GetSharedBorderThreshold()` in multiplayer builds, with exact vanilla 50000 retained in ordinary builds | Focused threshold contract; source rebuilt in all configurations |
| C06 | Ported all-lane reset during Replay chain registration, guarded for the ordinary one-lane layout | Registration-reset contract |
| C07-C08 | Ported multiplayer no-save policy at both save entries using `MultiplayerGameplay::IsMultiplayer`. Native `NoSave` test-shell flag is excluded. Ordinary saves/rewrites retain EAGX touch/analog extension handling | Two-entry save contract and EAGX append contracts |

Replay accounting: `8/8` hunks have a disposition. Multiplayer sessions do not write incomplete P1-only replay files, while ordinary `.rpy` and optional EAGX touch extensions remain supported. Pre-existing vanilla replay checksum and pre-existing EAGX trailer validation were not changed in this work class; this is not authorization to add or expand any hash or compatibility gate.

> **Current-tree replay addendum (2026-08-29):** the paragraph above records the
> historical frozen-rebase disposition and is no longer a complete description
> of the working tree. Current TH07 multiplayer replay uses the EAGX v1
> `FLAG_MULTIPLAYER_INPUT` sidecar to save 2P/3P loadouts and synchronized
> `FrameInput` lanes, and the Replay menu restores a `MultiplayerGameplay`
> session before playback. A real playback bug was found where
> `Player::RegisterChain()` additionally required `!g_GameManager.replay`, which
> collapsed an otherwise valid multiplayer Replay back to the single vanilla
> `g_Player`; the replay therefore displayed only P1 even though P2/P3 input was
> present. The guard has been removed. ReplayManager already restores all
> recorded player lanes, and multiplayer Replay now registers every active
> Player slot. Static Replay/Player contracts and the official Web netplay build
> pass after this fix. Human visual Replay playback remains a separate
> acceptance gate. Directly starting a later replay stage still needs a separate
> audit for guest per-stage resource snapshots; do not conflate that with the
> fixed missing-player registration bug.

### Replay real-person acceptance

- Record and play an ordinary keyboard replay. Verify vanilla `.rpy` behavior, stage transitions, held-button repeat and Border activation remain unchanged.
- Record and play ordinary joystick and direct-touch runs. Confirm EAGX analog/touch events reproduce movement and touch visualization frame by frame without changing a vanilla replay that has no EAGX trailer.
- In a 2P then 3P session, reach a replay-save/result path. No incomplete multiplayer replay should be written by either save entry.
- Load replay/demo initial state under the multiplayer build with 2P and 3P active counts. Border must cap/activate at 50000 for 2P and 75000 for 3P, identically on both endpoints.
- Inject rollback during recording-capable ordinary play. Speculative/resimulation passes must not append duplicate replay frames or consume touch events twice.

## GameWindow runtime hunk disposition

`C01..C10` are the zero-context final diff hunks for `src/th07/GameWindow.cpp`; `H01..H02` are `GameWindow.hpp`. All originate in `d93bc947b01a97633ac0e29db789093979e2c112`.

| Upstream hunk | Semantic disposition and local landing | Evidence |
| --- | --- | --- |
| C01-C02 | Excluded native `Netplay.hpp` and its peer-selected fullscreen/windowed helper. Eagler's launcher, SDL window and browser lifecycle own display mode locally | Native display-shell exclusion contracts |
| C03-C05 | Native Win32 cursor and original frame-loop window-mode substitutions are not portable gameplay and are excluded. Eagler retains SDL cursor/fullscreen behavior and its fixed-tick render loop | SDL-host and native-API exclusion; three builds |
| C06-C10 | Native Win32 rectangle/style/client-size and Direct3D presentation substitutions are excluded. Browser canvas/SDL window size is endpoint-local and must not become a session compatibility or gameplay field | Peer-dimension exclusion and portable host contract |
| H01-H02 | Excluded `TH07_COMPILE_ORIGINAL_RENDER` and `OriginalRender`, which are native reccmp/build wrappers. The portable `Render` method remains the sole host loop | Header exclusion contract |

GameWindow accounting: `10/10` implementation hunks and `2/2` header hunks have a disposition. These upstream hunks contain display-shell behavior, not sbrik's rollback algorithm. Eagler retains fixed 60 Hz simulation, a six-tick bounded recovery backlog, independent presentation, accumulator-clamped `g_RenderAlpha`, draw-only ANM suppression, one physical touch/controller sample per scheduled frame and presentation-cadenced WebAudio pumping.

The mature gameplay-neutral rollback semantics embedded in upstream `Netplay.cpp` are disposed separately in the function-class table above. `GameWindow` remains only the pacing/presentation owner; it does not absorb WinSock/thread/UDP shell or hash/repair behavior.

### GameWindow real-person acceptance

- Run desktop, desktop multiplayer and browser netplay at 60 Hz and high-refresh presentation. Simulation speed must remain 60 Hz while high-refresh drawing interpolates smoothly.
- Inject a short packet stall then recovery. Presentation must continue, accumulated simulation debt must remain bounded, recovery must run no more than six fixed ticks before a draw, and the game must not remain permanently slow afterward.
- Move with direct touch during the stall. The physical displacement is sampled once for its scheduled frame; recovery/resimulation must not read the device again or multiply that delta.
- Toggle the optional 60 FPS presentation mode. Retained frames must draw current state (`g_RenderAlpha=1`) without alternating interpolation step sizes.
- Pause/retry, background/foreground Web/mobile, toggle fullscreen locally and resume. Display choice must stay endpoint-local and must not affect session compatibility or logical state.
- Cross a BGM transition during jitter. Audio queues advance only on completed logical ticks while WebAudio pumping continues at presentation cadence.

## Bullet hunk disposition

`C01..C11` are the zero-context final diff hunks for `src/th07/BulletManager.cpp`, all from `d93bc947b01a97633ac0e29db789093979e2c112`.

| Upstream hunk | Semantic disposition and local landing | Evidence |
| --- | --- | --- |
| C01-C03 | Ported closest-active-player aiming for initial bullet patterns, initial aimed lasers and delayed direction-change re-aiming. Ordinary builds retain vanilla P1 aiming | Three exact helper-call contracts and ordinary fallback |
| C04 | Ported collision owner temporaries under the multiplayer compile guard | Build/source contract |
| C05 | Ported stable-slot active-player graze search, stopping at the first upstream result | Graze-loop contract |
| C06 | Ported graze-conversion item type from the actual collision player | First ownership contract |
| C07 | Ported stable-slot active-player killbox search | Killbox-loop contract |
| C08 | Ported killbox-conversion item type from the actual collision player | Second ownership contract |
| C09-C11 | Ported all-active-player laser collision for spawning, active and despawning/hitbox-tail states | Three-call laser contract |

Bullet accounting: `11/11` hunks are ported. Nearest-player aiming and every collision use authoritative bullet/laser and player positions; stable slot order resolves ties. Eagler's `prevPos`/`prevAngle`/laser-length publication remains in logical update, while position, angle and length interpolation occur only during draw.

### Bullet real-person acceptance

- Spawn aimed bullet patterns with P1/P2/P3 at distinct distances. The pattern must choose the closest active ship; equal-distance ties must resolve by stable lower slot on both endpoints.
- Start an aimed laser and trigger a delayed re-aim after players cross sides. Each aim event must use the closest active logical position at that event, not the visually smoothed position.
- Graze and collide with ordinary bullets using every slot. Conversion/despawn behavior and any spawned item type must belong to the actual first collision player and agree on both peers.
- Exercise laser spawning, active and despawning-tail hitboxes against all three players, including simultaneous overlaps, Border, Spirit/eliminated and temporary absence.
- Move the nearest player with direct touch under jitter/rollback. Target choice and hit results must remain deterministic while bullet/laser rendering stays smooth and never feeds interpolation into collision.
- Repeat representative patterns in ordinary single-player and confirm vanilla P1 aiming/collision is unchanged.

## Game rules hunk disposition

`C01..C20` are the zero-context final diff hunks for `src/th07/GameManager.cpp`; `H01..H03` are `GameManager.hpp`. The gameplay changes originate in `d93bc947b01a97633ac0e29db789093979e2c112`; `022c533e0c4205e07c94047a0523cb267ae46447` adds the final P1-resource diagnostic and P3/contribution/resource declarations. The release commit `be2c35a` only exposes the already-accounted Boss multiplier declaration in the header history.

| Upstream hunk | Semantic disposition and local landing | Evidence |
| --- | --- | --- |
| C01 | Excluded native `Netplay.hpp`. Runtime multiplayer state comes from the transport-neutral `MultiplayerGameplay` session facade | Native-shell exclusion contract and three builds |
| C02 | Excluded one-shot `g_cherryRangeBreachLogged` diagnostic. It changes no gameplay state and importing a native verification log is not required for the browser host | Focused diagnostic-exclusion contract |
| C03 | Ported multiplayer forced `slowMode=0` and `slowModeSlowActive=0`, preventing one endpoint from skipping simulation ticks because of its local bullet-load option | Exact update-body contract and builds |
| C04, C06, C08-C14 | Upstream release/encoding-only rewrites of existing error strings are represented by the portable UTF-8 Japanese strings already in Eagler; no gameplay branch changes | Current localized error paths and builds |
| C05 | Ported deterministic startup options: two starting lives and no slow-mode frame skipping. Difficulty/Extra and practice overrides deliberately remain afterward in upstream order. Native configuration logging is excluded | Startup/order contract and builds |
| C07 | Excluded P1-resource initialization log only. P1 initializes through original globals; P2/P3 initialize from their loaded SHT through `ResetMultiplayerPlayerResources` during player registration | Registration call-site audit and Player/resources evidence |
| C15 | Adapted `AddCherryPlus` so multiplayer routes through `AddCherryPlusForPlayer`, while ordinary builds retain the original P1/50000/`ActivateBorder` body rather than pulling multiplayer sidecars into the vanilla layout | Shared-helper and ordinary-fallback contracts |
| C16 | Excluded the same non-gameplay Cherry-range diagnostic as C02; the upstream cap itself was already vanilla behavior and remains intact | Focused exclusion/cap contract |
| C17-C20 | Ported runtime-only forced content unlocks for every relevant query. `ShouldForceContentUnlocks` returns the session state and never writes `score.dat`, `clrd` or compatibility metadata | Four-function contract and session-body audit |
| H01 | Adapted the `Multiplayer.hpp` include behind `TH_ENABLE_MULTIPLAYER_GAMEPLAY`, preserving the ordinary header surface | Header guard and ordinary build |
| H02 | Ported complete P2/P3 sidecar resources, contribution counters, shared Cherry/Border, Boss/Bomb scaling, rank scaling and active-count APIs. Definitions were already fully compared against the final upstream helper file in the Player/resources class | Complete API contract and prior full-file equivalence evidence |
| H03 | Ported `AddCherryPlusForPlayer`; guarded so ordinary code retains its original class API/layout contract | Header/source contract and both desktop builds |

Game rules accounting: `20/20` implementation hunks and `3/3` header hunks have a disposition. Shared gameplay uses authoritative counts and integer state only; this class has no draw position and therefore introduces no interpolation path or raw touch read. Existing vanilla integrity/checksum code was not changed, removed, added or expanded during this work class.

### Game rules real-person acceptance

- Start 2P and 3P rooms from endpoints whose local starting-life and slow-mode settings differ. Both peers must begin with the same two-life multiplayer baseline, and high bullet counts must never make only one endpoint skip gameplay ticks.
- Repeat Extra/Phantasm and practice startup. Their upstream post-normalization overrides must remain deterministic and agree on both peers.
- Fill shared Cherry/Border in 2P then 3P. Confirm thresholds 50000/75000, one activation, and one shared gauge; joining/leaving the third slot must scale the Cherry range once and contraction must not activate Border merely by lowering the threshold.
- Cause deaths/Bombs with two then three active players. Confirm the shared rank penalty and Boss/Bomb damage scaling match active-count rules on both endpoints.
- Enter Extra/Phantasm-capable menus with peers whose local `score.dat` unlocks differ. Multiplayer queries must expose the same content without modifying either endpoint's save data; ordinary single-player must still honor local unlock progress.
- Perform the startup and Border checks with one direct-touch endpoint. Touch must continue to affect only its logical player; GameManager must consume no raw touch or presentation-smoothed coordinates, and rollback must restore identical shared values.

## HUD hunk disposition

`A01..A11` are the zero-context final diff hunks for `src/th07/AsciiManager.cpp`; `G01..G54` are the hunks for `Gui.cpp`. Gameplay/HUD changes originate in `d93bc947b01a97633ac0e29db789093979e2c112`, with the paused-screen repair in `ecd4483` and final resource/contribution additions in `022c533e0c4205e07c94047a0523cb267ae46447`.

| Upstream hunk | Semantic disposition and local landing | Evidence |
| --- | --- | --- |
| A01 | Eagler already uses standard C++ string/format facilities for the same operations; no native C include dependency is required | Existing formatter and three builds |
| A02 | Already equivalent: `AsciiManager::OnUpdate` no longer manipulates `renderSkipFrames`; `Gui::DrawGameScene` asks pause/retry state directly | Update/scene focused contracts |
| A03 | Ported with the portable renderer: flush pause/retry sprites, then restore the full 640×480 viewport through `gfxDevice` | Exact ordering/viewport contract and builds |
| A04-A05 | Excluded prefix filters for native `P2 L` and `NET ` diagnostic strings because the browser architecture emits neither producer. Adding broad text suppression could hide ordinary/localized strings | Whole-file producer-exclusion checks |
| A06 | Ported the `curState != 0` guard so the opening synchronized Menu edge cannot immediately close an uninitialized pause menu | Pause-body contract |
| A07 | Ported active P2/P3 sidecar-resource reset on retry, behind the multiplayer guard | Retry-body contract |
| A08-A11 | Ported/adapted shared Border activity and 2P/3P threshold rendering. Ordinary mode retains P1/50000; Cherry digit scale/color publishes matching previous-state fields for Eagler interpolation | Shared/ordinary/prev-state contracts |
| G01 | Ported `AnmIdx` dependency through the existing guarded asset surface | Portrait constant contracts |
| G02 | Excluded native `Netplay.hpp`; all gameplay/UI queries use `MultiplayerGameplay` and browser-owned session state | Native-shell exclusion contract |
| G03 | Ported loadout labels using transport-neutral per-slot character/Shot queries | Loadout contract |
| G04 | Ported P2/P3 Bomb portrait script blocks and unloaded-sprite fallback. Native one-shot verification logs are excluded; Eagler additionally bounds-checks the sprite index | Portrait/fallback contract |
| G05 | Ported P2/P3 face ANM lifetime to GUI registration, once per slot, through browser session loadouts | Face-load ownership contract |
| G06 | Upstream encoding-only error-string repair is represented by Eagler's portable UTF-8 Japanese message | Build/error-path audit |
| G07-G09 | Ported dialogue-time Border break through the first active player; shared propagation performs one deterministic break. Ordinary P1 fallback remains | RunMsg contract |
| G10 | Excluded stage-end Cherry diagnostic log; it changes no gameplay state | Frozen hunk inspection |
| G11 | Excluded native stage-bonus/growth logging, but ported the final all-slot reset of per-stage Cherry growth counters | Update-body reset contract |
| G12-G18 | Ported compact-HUD locals, pause/retry direct redraw, shifted score labels and multiplayer hiding/repositioning of original labels | Geometry/redraw contracts |
| G19-G25 | Ported exact compact score/resource label positions and ordinary fallbacks | Geometry and ordinary-build contracts |
| G26 | Ported per-row clearing, half-width Graze/Point value backgrounds, slot labels and lower Graze/Point labels. Eagler adaptation synchronizes every temporary VM scale with `prevScale` | Background/label/interpolation contracts |
| G27-G35 | Ported independent life/Bomb icons, active/temporarily-absent filtering, 0.65 scale and exact 11-pixel step. Both `scale` and `prevScale` are saved/restored | Resource/scale contracts |
| G36-G39 | Ported shifted score values, per-slot name/loadout/AWAY rows and local presentation-only contribution totals. Browser has no nickname transport yet, so stable `P1`/`P2`/`P3` labels remain; contribution display defaults on like upstream and is never negotiated | Session preference and row-content contracts |
| G40 | Excluded native input-delay/RTT HUD because it depends on the excluded native network shell. Existing browser network presentation remains transport-owned | Native diagnostics exclusion |
| G41-G53 | Ported per-slot Power bars/text, exact 0.25 width scale, active/absence filtering and full ASCII/renderer state restoration through portable `gfxDevice`; Graze/Point text now uses/restores upstream 0.70 scale | Power/renderer/state contracts |
| G54 | Ported final removal of the single global Bomb portrait/decor draw. This avoids presenting one shared cut-in as if it belonged to simultaneous independent Bomb owners; Bomb name animation remains | DrawStageElements absence contract |

HUD accounting: `11/11` AsciiManager hunks and `54/54` Gui hunks have a disposition. The compact HUD reads synchronized logical resources/contribution counters only; it never reads raw touch or presentation-smoothed player coordinates. All temporary icon, label, row-background and Cherry scales update the matching Eagler interpolation endpoints. No hash behavior was touched.

### HUD real-person acceptance

- Run 2P and 3P with different character/Shot, lives, Bombs and Power. Confirm each row shows the correct stable slot, loadout, contribution totals and exact resource values, with no stale icons after a value decreases.
- Temporarily absent and return each guest. Resource labels/icons/Power must hide while absent, `AWAY` must remain visible without overlapping contributions, and the same row must return without inheriting another player's values.
- Observe life/Bomb icons, labels, Graze/Point background and Cherry digits at non-integer/high-refresh render cadence. No element may pulse between original and compact scale; `prevScale` changes are draw-local and must not enter snapshots.
- Pause and retry repeatedly during long 2P/3P play. The menu must not immediately close on its opening edge, the right HUD must not clip or flicker, and retry must reset every active slot's resources.
- Enter dialogue while shared Border is active. It must break once through stable active-slot order and agree on both endpoints; ordinary single-player dialogue must retain vanilla P1 behavior.
- Bomb with each character/Shot and slot. Face assets must load without negative indexing; final sbrik semantics intentionally show no single shared Bomb portrait/decor, while the Bomb name/other feedback remains.
- Use direct touch on one endpoint while resource values and contributions change under rollback. HUD values must agree on both peers, touch must control only its slot, and HUD drawing must not feed coordinates or scale into gameplay.
- Run ordinary single-player through pause, retry, dialogue, Bomb and Border. Original score/resource placement, 50000 threshold and P1-only values must remain unchanged.

## Bomb hunk disposition

`C01..C27` are the zero-context final diff hunks for `src/th07/BombData.cpp`, all from `d93bc947b01a97633ac0e29db789093979e2c112`.

| Upstream hunk | Semantic disposition and local landing | Evidence |
| --- | --- | --- |
| C01 | Ported the Bomb invulnerability effect to the owner's fixed effect slot. `ResolveBombEffectSlot` returns the original P1 slot in ordinary builds | Effect-slot/fallback contracts |
| C02, C04, C06, C08, C10, C12, C14, C16, C18, C21, C24, C26 | Ported all twelve character/Shot/focus Bomb portrait scripts through the actual `Player*` owner. Eagler localization wraps only the name string and does not change the script index | Exact call-count and all-variant contracts |
| C03, C05, C07, C09, C11, C13, C15, C17, C19-C20, C22-C23, C25, C27 | Ported all fourteen Bomb VM initialization sites through the owner's ANM block, including both Sakuya `1120` auxiliary scripts. `ResolveBombAnmScript` is an Eagler guard that returns the unchanged vanilla index in ordinary builds | Exact call-count, owner and fallback contracts |

Bomb accounting: `27/27` hunks are ported. Each of the twelve calc/draw pairs consumes its passed `Player*`, so timer, sub-info, damage/clear boxes, movement multipliers and effects remain independently owned. Final 3P Bomb-damage scaling is applied in the already-audited `Player::CalcDamageToEnemy` path. Logical Bomb updates publish `prevBombRegionPositions`; draw callbacks interpolate those positions only for presentation and consume neither raw touch nor remote presentation offsets. No hash behavior was touched.

### Bomb real-person acceptance

- In 2P and 3P, execute focused and unfocused Bombs for Reimu A/B, Marisa A/B and Sakuya A/B from every represented slot. Confirm the correct per-slot ANM scripts, Bomb name and effects, with no P1 asset borrowing or negative sprite access.
- Bomb simultaneously or with overlapping durations. Each player's timer, movement multiplier, invulnerability effect, damage/clear boxes and existing projectiles must remain independent; one player's Bomb ending must not end another's.
- Hit the same Boss with P1/P2/P3 Bombs and confirm owner-specific damage plus the final three-player Bomb multiplier agree on both endpoints.
- Observe moving/trailing Bombs at high-refresh presentation and under injected rollback. Region/trail drawing must stay smooth from logical previous/current endpoints without changing hitbox coordinates or later snapshots.
- Use direct-touch movement and touch Bomb on one endpoint while another uses keyboard. Only the synchronized owner lane may trigger/move that player's Bomb; BombData itself must never resample device touch during forward simulation or resimulation.
- Repeat representative Bombs in ordinary single-player. Script indices, effect slot, Cherry drain, shared item clearing and damage behavior must remain vanilla.

### Verification log

| Date | Scope | Evidence | Result | Remaining limitation |
| --- | --- | --- | --- | --- |
| 2026-08-25 | Transfer-state source contract | Checked P1/P2/P3 state mapping 4/3/5, 60 px rise, 20-frame transition, collision suppression, removal of `SpawnItemForPlayer`, three upstream transfer callers, Spirit Bomb=3, and prompt selection restoration | PASS, 10/10 checks | Structural evidence only; does not prove in-game trajectory or collection |
| 2026-08-25 | Ordinary desktop build | CMake `TH_ENABLE_MULTIPLAYER_GAMEPLAY=OFF`, `TH_ENABLE_NETPLAY=OFF`; rebuilt and linked `th07.exe` | PASS | Build evidence only |
| 2026-08-25 | Multiplayer-gameplay desktop build | CMake `TH_ENABLE_MULTIPLAYER_GAMEPLAY=ON`, `TH_ENABLE_NETPLAY=OFF`; compiled and linked `Player.cpp`, `ItemManager.cpp`, and `MultiplayerResources.cpp` | PASS after adding the required standard `<algorithm>` include for existing `std::clamp` calls | No Web build or two-end runtime acceptance yet |
| 2026-08-25 | Stage-name source contract | Checked 240-frame duration, 0.48 scale, display-setting guard, per-slot vertical stagger, ASCII selection restoration, draw-call wiring, and upstream default names | PASS, 7/7 checks; ordinary and multiplayer desktop rebuilds also PASS | Browser nickname exchange is not implemented; visual placement needs a real stage run |
| 2026-08-25 | Items final hunk accounting | Enumerated final zero-context diff and disposed C01-C49/H01-H02 against `d93bc94` and `22b4bcb`; corrected inactive-target release and shared-Border fixed-item redistribution | PASS, 51/51 hunks disposed | C25/C33/C40 checksum deletions intentionally skipped under the hash boundary |
| 2026-08-25 | Items focused contract/model test | `python audit/feature-rebase/item-manager-feature-rebase-test.py`: upstream hunk counts, target APIs, transfer mapping/easing/lock, 2P/3P drop positions, active-slot round robin, interpolation endpoints, logical targeting, raw-touch/native-shell exclusion | PASS, 28/28 checks | Structural/deterministic model evidence; not a real two-end stage run |
| 2026-08-25 | Items ordinary desktop rebuild | Existing Release cache with `TH_ENABLE_MULTIPLAYER_GAMEPLAY=OFF`, `TH_ENABLE_NETPLAY=OFF`; recompiled `ItemManager.cpp` and linked `th07.exe` | PASS | Build evidence only |
| 2026-08-25 | Items multiplayer desktop rebuild | Existing Release cache with `TH_ENABLE_MULTIPLAYER_GAMEPLAY=ON`, `TH_ENABLE_NETPLAY=OFF`; recompiled `ItemManager.cpp` and linked `th07.exe` | PASS | Build evidence only |
| 2026-08-25 | Items WebSocket/rollback Web rebuild | Existing Emscripten cache with `TH_ENABLE_MULTIPLAYER_GAMEPLAY=ON`, `TH_ENABLE_NETPLAY=ON`; rebuilt `ItemManager.cpp` and linked `th07.html` | PASS | No live browser/two-end interaction in this work class |
| 2026-08-26 | Player/resources final hunk accounting | Enumerated and disposed `Player.cpp` P01-P126, `Player.hpp` H01-H04, and the two upstream-only resource-file hunks against `d93bc94`, `22b4bcb`, `be2c35a`, and `022c533` | PASS, 132/132 changed-file hunks plus both new-file hunks disposed | Native network/diagnostic slices excluded; pre-existing vanilla integrity calls left untouched under the hash boundary |
| 2026-08-26 | Player/resources focused contract/model test | `python audit/feature-rebase/player-resources-feature-rebase-test.py`: final upstream resource-body equivalence, slot identity, loadouts, SHT Power, missile normalization, raw-touch ownership, transfer selection, Border/Bomb scaling, projectile draw lifetime, and presentation-only interpolation | PASS, 46/46 checks | Structural/deterministic model evidence; not a real two-end stage run |
| 2026-08-26 | Player/resources ordinary desktop rebuild | Existing Release cache with `TH_ENABLE_MULTIPLAYER_GAMEPLAY=OFF`, `TH_ENABLE_NETPLAY=OFF`; rebuilt and linked `th07.exe` | PASS | Build evidence only |
| 2026-08-26 | Player/resources multiplayer desktop rebuild | Existing Release cache with `TH_ENABLE_MULTIPLAYER_GAMEPLAY=ON`, `TH_ENABLE_NETPLAY=OFF`; rebuilt Player/resource paths and linked `th07.exe` | PASS | Build evidence only |
| 2026-08-26 | Player/resources WebSocket/rollback Web rebuild | Existing Emscripten cache with `TH_ENABLE_MULTIPLAYER_GAMEPLAY=ON`, `TH_ENABLE_NETPLAY=ON`; rebuilt and linked `th07.html` | PASS | No live browser/two-end interaction yet |
| 2026-08-26 | TH06 sister-port reuse notes | Added `docs/th06-th07-multiplayer-feature-rebase-notes.md` with reusable audit format, touch/interpolation invariants, transport/hash exclusions, and TH06 start gate | PASS, `git diff --check` clean for the source, test, ledger, and reuse notes | It is an adaptation guide only; TH06 must freeze and audit its own upstream oracle |
| 2026-08-26 | Input final hunk accounting | Disposed Controller C01-C02/H01-H03 against `d93bc94` and `22b4bcb`: player lanes ported, DirectInput diagnostic/local keyboard/reccmp wrapper excluded | PASS, 5/5 hunks disposed | Browser nickname/menu transport is outside this work class |
| 2026-08-26 | Input focused contract test | `python audit/feature-rebase/controller-feature-rebase-test.py`: frozen counts, per-slot current/previous edges, ordinary fallback, native sampler exclusion, one-shot physical sampling, Eagler sources/deadzone, full touch payload and pre-simulation lane commit | PASS, 14/14 checks | Structural evidence; two real browser endpoints still required |
| 2026-08-26 | Input ordinary desktop rebuild | Existing Release cache with multiplayer and netplay disabled | PASS, `th07` target built | Build evidence only |
| 2026-08-26 | Input multiplayer desktop rebuild | Existing Release cache with gameplay enabled and netplay disabled | PASS, `th07` target built | Build evidence only |
| 2026-08-26 | Input WebSocket/rollback Web rebuild | Existing Emscripten gameplay/netplay cache | PASS, target current (`ninja: no work to do`) | No live touch/keyboard peer run yet |
| 2026-08-26 | Audio final hunk accounting | Disposed SoundPlayer C01-C02 against `d93bc94`; native preference API excluded and per-endpoint portable preference/side-effect behavior retained | PASS, 2/2 hunks disposed | Audible behavior still needs real endpoints |
| 2026-08-26 | Audio focused contract test | `python audit/feature-rebase/sound-player-feature-rebase-test.py`: upstream count, native-shell exclusion, local preference, resimulation-only SFX suppression, logical-tick queue advancement and Web transition pairing | PASS, 9/9 checks | Structural evidence cannot prove audible transitions |
| 2026-08-26 | Audio ordinary desktop rebuild | Existing Release cache with multiplayer and netplay disabled | PASS, `th07` target built | Build evidence only |
| 2026-08-26 | Audio multiplayer desktop rebuild | Existing Release cache with gameplay enabled and netplay disabled | PASS, `th07` target built | Build evidence only |
| 2026-08-26 | Audio WebSocket/rollback Web rebuild | Existing Emscripten gameplay/netplay cache | PASS, target current (`ninja: no work to do`) | Browser gesture/BGM transition still needs real listening |
| 2026-08-26 | Diagnostics final hunk accounting | Disposed GameErrorContext C01-C03 against `d93bc94`/`be2c35a`; inseparable native hash/resync log policy excluded and portable diagnostics retained | PASS, 3/3 hunks disposed | No new long-run log recycling was introduced |
| 2026-08-26 | Diagnostics focused contract test | `python audit/feature-rebase/game-error-context-feature-rebase-test.py`: upstream count, hash/RNG/native-policy exclusion, bounded formatter, localization, fatal dialog and fixed buffer | PASS, 8/8 checks | Real missing-resource/dialog check pending |
| 2026-08-26 | Diagnostics ordinary desktop rebuild | Existing Release cache with multiplayer and netplay disabled | PASS, `th07` target built | Build evidence only |
| 2026-08-26 | Diagnostics multiplayer desktop rebuild | Existing Release cache with gameplay enabled and netplay disabled | PASS, `th07` target built | Build evidence only |
| 2026-08-26 | Diagnostics WebSocket/rollback Web rebuild | Existing Emscripten gameplay/netplay cache | PASS, target current (`ninja: no work to do`) | Browser error presentation not exercised |
| 2026-08-26 | Enemy ECL instruction final hunk accounting | Disposed the single `EnemyEclInstr.cpp` hunk from `d93bc94`; multiplayer nearest-active targeting ported and ordinary P1 path retained | PASS, 1/1 hunk ported | Real Youmu pattern run pending |
| 2026-08-26 | Enemy ECL instruction focused test | `python audit/feature-rebase/enemy-ecl-instr-feature-rebase-test.py`: count, exact helper call, ordinary guard, logical-coordinate targeting and inactive-player model | PASS, 6/6 checks | Model/source evidence only |
| 2026-08-26 | Enemy ECL instruction ordinary desktop rebuild | Existing Release cache with multiplayer and netplay disabled | PASS, `th07` target built | Build evidence only |
| 2026-08-26 | Enemy ECL instruction multiplayer desktop rebuild | Existing Release cache with gameplay enabled and netplay disabled | PASS, `th07` target built | Build evidence only |
| 2026-08-26 | Enemy ECL instruction WebSocket/rollback Web rebuild | Existing Emscripten gameplay/netplay cache | PASS, target current (`ninja: no work to do`) | No real Boss-stage peer run |
| 2026-08-26 | Assets/ANM final hunk accounting | Disposed AnmIdx I01-I04, AnmManager C01-C16 and header H01-H04 against `d93bc94`; multiplayer capacities/offsets ported, portable diagnostics/renderer retained, native layout assert excluded | PASS, 24/24 hunks disposed | Mixed-loadout and Bomb cut-in visuals pending |
| 2026-08-26 | Assets/ANM focused contract test | `python audit/feature-rebase/anm-assets-feature-rebase-test.py`: frozen counts, exact file/offset map, face distance, shared capacities/bounds, per-slot Player load/release, portable-layout and Web-cache preservation | PASS, 18/18 checks | Structural evidence only |
| 2026-08-26 | Assets/ANM ordinary desktop rebuild | Existing Release cache with multiplayer and netplay disabled | PASS, `th07` target built | Build evidence only |
| 2026-08-26 | Assets/ANM multiplayer desktop rebuild | Existing Release cache with gameplay enabled and netplay disabled | PASS, `th07` target built | Build evidence only |
| 2026-08-26 | Assets/ANM WebSocket/rollback Web rebuild | Existing Emscripten gameplay/netplay cache | PASS, target current (`ninja: no work to do`) | No real Web texture/cut-in matrix yet |
| 2026-08-26 | Effects final hunk accounting | Disposed EffectManager C01-C14/H01-H02 against `d93bc94`; all-slot attachment/capacity/alpha ported, native log/layout assert excluded, Eagler interpolation adaptation retained | PASS, 16/16 hunks disposed | Attached-effect runtime matrix pending |
| 2026-08-26 | Effects focused contract test | `python audit/feature-rebase/effect-manager-feature-rebase-test.py`: frozen counts, slots/sentinel/update range, all-player ownership, logical attachment, interpolation/presentation offset, alpha restore and rollback journal coverage | PASS, 16/16 checks | Structural evidence only |
| 2026-08-26 | Effects ordinary desktop rebuild | Existing Release cache with multiplayer and netplay disabled | PASS, `th07` target built | Build evidence only |
| 2026-08-26 | Effects multiplayer desktop rebuild | Existing Release cache with gameplay enabled and netplay disabled | PASS, `th07` target built | Build evidence only |
| 2026-08-26 | Effects WebSocket/rollback Web rebuild | Existing Emscripten gameplay/netplay cache | PASS, target current (`ninja: no work to do`) | No real touch/rollback effect run |
| 2026-08-26 | Replay final hunk accounting | Disposed ReplayManager C01-C08 against `d93bc94`; corrected replay Border threshold, retained Eagler lane timing/EAGX, excluded native shell/NoSave | PASS, 8/8 hunks disposed | Ordinary/EAGX playback and multiplayer no-save need real UI runs |
| 2026-08-26 | Replay focused contract test | `python audit/feature-rebase/replay-manager-feature-rebase-test.py`: count, native exclusion, logical edge timing, P2/P3 demo clear, EAGX ordering, Border threshold, lane reset and both save policies | PASS, 15/15 checks | Structural evidence only |
| 2026-08-26 | Replay ordinary desktop rebuild | Recompiled `ReplayManager.cpp` in Release with multiplayer and netplay disabled; linked `th07.exe` | PASS | Build evidence only |
| 2026-08-26 | Replay multiplayer desktop rebuild | Recompiled `ReplayManager.cpp` in Release with gameplay enabled and netplay disabled; linked `th07.exe` | PASS | Build evidence only |
| 2026-08-26 | Replay WebSocket/rollback Web rebuild | Recompiled `ReplayManager.cpp` in Emscripten gameplay/netplay cache; linked `th07.html` | PASS | No real browser replay file round trip |
| 2026-08-26 | GameWindow final hunk accounting | Disposed GameWindow C01-C10/H01-H02 against `d93bc94`; native display/reccmp wrappers excluded and Eagler SDL/Web pacing retained | PASS, 12/12 hunks disposed | Upstream `Netplay.cpp` rollback semantics remain a separate pending audit |
| 2026-08-26 | GameWindow focused contract test | `python audit/feature-rebase/game-window-feature-rebase-test.py`: counts, native display exclusion, SDL host, 60 Hz tick, elapsed/backlog/catchup bounds, audio commit, render alpha, ANM suppression, WebAudio and one-shot input capture | PASS, 18/18 checks | Structural evidence; jitter/high-refresh/touch needs real endpoints |
| 2026-08-26 | GameWindow ordinary desktop rebuild | Existing Release cache with multiplayer and netplay disabled | PASS, `th07` target built | Build evidence only |
| 2026-08-26 | GameWindow multiplayer desktop rebuild | Existing Release cache with gameplay enabled and netplay disabled | PASS, `th07` target built | Build evidence only |
| 2026-08-26 | GameWindow WebSocket/rollback Web rebuild | Existing Emscripten gameplay/netplay cache | PASS, target current (`ninja: no work to do`) | Live browser cadence/stall test pending |
| 2026-08-26 | Bullet final hunk accounting | Disposed BulletManager C01-C11 against `d93bc94`; closest-active targeting, per-player bullet ownership and all laser states ported | PASS, 11/11 hunks ported | Real pattern/collision matrix pending |
| 2026-08-26 | Bullet focused contract test | `python audit/feature-rebase/bullet-manager-feature-rebase-test.py`: count, three aim paths, ordinary fallback, graze/killbox ownership, three laser states, interpolation publication/draw and deterministic nearest models | PASS, 16/16 checks | Structural/model evidence only |
| 2026-08-26 | Bullet ordinary desktop rebuild | Existing Release cache with multiplayer and netplay disabled | PASS, `th07` target built | Build evidence only |
| 2026-08-26 | Bullet multiplayer desktop rebuild | Existing Release cache with gameplay enabled and netplay disabled | PASS, `th07` target built | Build evidence only |
| 2026-08-26 | Bullet WebSocket/rollback Web rebuild | Existing Emscripten gameplay/netplay cache | PASS, target current (`ninja: no work to do`) | No real touch/laser peer run |
| 2026-08-26 | Game rules final hunk accounting | Disposed GameManager C01-C20/H01-H03 against `d93bc94`, `be2c35a` and `022c533`; deterministic options/shared state/unlocks ported, native logs/encoding-only slices disposed, ordinary fallback retained | PASS, 23/23 hunks disposed | Real startup/Border/rank/unlock matrix pending |
| 2026-08-26 | Game rules focused contract/model test | `python audit/feature-rebase/game-manager-feature-rebase-test.py`: frozen counts, native/diagnostic exclusion, slow mode, startup ordering, ordinary/shared Cherry, active-count scaling, resource API, runtime-only unlocks and 2P/3P models | PASS, 20/20 checks | Structural/model evidence only; no real two-end stage/menu run |
| 2026-08-26 | Game rules ordinary desktop rebuild | Existing Release cache with multiplayer and netplay disabled | PASS, `th07` target built | Build evidence only |
| 2026-08-26 | Game rules multiplayer desktop rebuild | Existing Release cache with gameplay enabled and netplay disabled | PASS, `th07` target built | Build evidence only |
| 2026-08-26 | Game rules WebSocket/rollback Web rebuild | Existing Emscripten gameplay/netplay cache | PASS, target current (`ninja: no work to do`) | Live touch/startup/Border verification pending |
| 2026-08-26 | HUD final hunk accounting | Disposed AsciiManager A01-A11 and Gui G01-G54 against `d93bc94`, `ecd4483` and `022c533`; filled viewport, per-stage reset, compact background/scale and final Bomb-portrait gaps | PASS, 65/65 hunks disposed | Real desktop/Web/mobile 2P/3P visual matrix pending |
| 2026-08-26 | HUD focused contract tests | `python audit/feature-rebase/ascii-manager-feature-rebase-test.py` and `python audit/feature-rebase/gui-feature-rebase-test.py`: counts, menu viewport, retry, shared Border, portraits/faces, compact geometry/resources/contributions, interpolation scale, portable power bars and final draw removal | PASS, 15/15 plus 26/26 checks | Structural evidence cannot prove pixel placement or absence of flicker |
| 2026-08-26 | HUD ordinary desktop rebuild | Recompiled `AsciiManager.cpp`/`Gui.cpp` with multiplayer and netplay disabled; linked `th07.exe` | PASS | Build evidence only |
| 2026-08-26 | HUD multiplayer desktop rebuild | Recompiled HUD and dependent gameplay/session files with gameplay enabled, netplay disabled; linked `th07.exe` | PASS, pre-existing multi-character literal warnings only | No live 2P/3P visual run |
| 2026-08-26 | HUD WebSocket/rollback Web rebuild | Recompiled HUD and dependent gameplay/session files in Emscripten cache; linked `th07.html` | PASS | No mobile/high-refresh visual acceptance yet |
| 2026-08-26 | Bomb final hunk accounting | Disposed BombData C01-C27 against `d93bc94`; owner effect slot, twelve portraits and fourteen VM script sites are ported with ordinary fallbacks | PASS, 27/27 hunks ported | Real simultaneous all-loadout Bomb matrix pending |
| 2026-08-26 | Bomb focused contract test | `python audit/feature-rebase/bomb-data-feature-rebase-test.py`: count, ANM/effect fallbacks, all calc/draw pairs, owner state, logical/draw interpolation, touch exclusion, damage multiplier and shared item clear | PASS, 15/15 checks | Structural evidence cannot prove Bomb visuals or collision timing |
| 2026-08-26 | Bomb ordinary desktop rebuild | Existing Release cache with multiplayer and netplay disabled | PASS, `th07` target built | Build evidence only |
| 2026-08-26 | Bomb multiplayer desktop rebuild | Existing Release cache with gameplay enabled and netplay disabled | PASS, `th07` target built | No real simultaneous Bomb run |
| 2026-08-26 | Bomb WebSocket/rollback Web rebuild | Existing Emscripten gameplay/netplay cache | PASS, target current (`ninja: no work to do`) | No live touch-Bomb/rollback acceptance yet |
| 2026-08-26 | ECL final hunk accounting | Disposed EclManager C01-C30/H01 against `d93bc94`, `5645bee` and `022c533`; corrected Stage 4 from always-on multiplayer to host-authored/default-off | PASS, 31/31 hunks disposed | Native trace slices excluded; hosted option propagation needs a real room run |
| 2026-08-26 | ECL focused contract test | `python audit/feature-rebase/ecl-manager-feature-rebase-test.py`: counts, native-trace exclusion, all logical target forms, item/Bomb paths, Stage 4 table/default/queue/restart/reset/rollback and touch/interpolation exclusion | PASS, 19/19 checks | Structural/model evidence; no real Stage 4 peer run |
| 2026-08-26 | ECL ordinary desktop rebuild | Recompiled ECL/main paths with multiplayer and netplay disabled; linked `th07.exe` | PASS | Build evidence only |
| 2026-08-26 | ECL multiplayer desktop rebuild | Recompiled ECL/session/rollback-dependent paths with gameplay enabled and netplay disabled; linked `th07.exe` | PASS, pre-existing multi-character literal warnings only | No real closest-target/Stage 4 run |
| 2026-08-26 | ECL WebSocket/rollback Web rebuild | Initial compile exposed a missing explicit `EaglerOptions.hpp` include; added it, rebuilt and linked `th07.html` | PASS after correction | No live touch/rollback/host-option acceptance yet |
| 2026-08-26 | Enemy final hunk accounting | Disposed EnemyManager C01-C33 against `d93bc94`, `be2c35a` and `022c533`; retained Eagler session-seeded RNG adaptation and excluded native trace/session-shell slices | PASS, 33/33 hunks disposed | Native diagnostics excluded; real damage/drop/Stage 4 peer run pending |
| 2026-08-26 | Enemy focused contract test | `python audit/feature-rebase/enemy-manager-feature-rebase-test.py`: frozen count, active-slot collisions, damage/Cherry/Boss contribution, targeting, drop calls, Stage 4 reset, session-seed ordering and touch/interpolation exclusion | PASS, 16/16 checks | Structural/source evidence; no real two-end enemy run |
| 2026-08-26 | Enemy ordinary desktop rebuild | Rebuilt Enemy-dependent target with multiplayer and netplay disabled; linked `th07.exe` | PASS | Build evidence only |
| 2026-08-26 | Enemy multiplayer desktop rebuild | Rebuilt Enemy-dependent target with gameplay enabled and netplay disabled; linked `th07.exe` | PASS | Build evidence only |
| 2026-08-26 | Enemy WebSocket/rollback Web rebuild | Reused Emscripten gameplay/netplay cache after Enemy audit | PASS, `ninja: no work to do` | No live touch/rollback enemy collision run |
| 2026-08-26 | MainMenu final hunk accounting | Disposed MainMenu C01-C17 against `d93bc94`; native connection/quick-start shell excluded, portable launch/options/unlock/cursor behavior retained | PASS, 17/17 hunks disposed | Native shell excluded; real hosted menu run pending |
| 2026-08-26 | MainMenu focused contract test | `python audit/feature-rebase/main-menu-feature-rebase-test.py`: frozen count, native exclusion, launch mapping, runtime unlocks, option suppression, cursor wrap and presentation/raw-touch boundaries | PASS, 11/11 checks | Structural evidence; no real hosted menu/session run |
| 2026-08-26 | MainMenu ordinary desktop rebuild | Recompiled MainMenu with multiplayer and netplay disabled; linked `th07.exe` | PASS | Build evidence only |
| 2026-08-26 | MainMenu multiplayer desktop rebuild | Recompiled MainMenu/session paths with gameplay enabled and netplay disabled; linked `th07.exe` | PASS | Build evidence only |
| 2026-08-26 | MainMenu WebSocket/rollback Web rebuild | Recompiled MainMenu in Emscripten gameplay/netplay cache; linked `th07.html` | PASS | No live hosted menu/session run |
| 2026-08-26 | ResultScreen final hunk accounting | Disposed ResultScreen C01-C28 against `d93bc94` and `022c533`; native NoSave/network shell excluded, multiplayer persistence/prompt rules retained, existing score checksums untouched | PASS, 28/28 hunks disposed | Real ordinary replay and hosted result runs pending |
| 2026-08-26 | ResultScreen focused contract test | `python audit/feature-rebase/result-screen-feature-rebase-test.py`: frozen count, native exclusion, multiplayer score/replay suppression, ordinary persistence, display parsing, draw/raw-touch and hash-boundary checks | PASS, 8/8 checks | Structural evidence; no real result/replay run |
| 2026-08-26 | ResultScreen ordinary desktop rebuild | Existing Release cache with multiplayer and netplay disabled; linked `th07.exe` | PASS | Build evidence only |
| 2026-08-26 | ResultScreen multiplayer desktop rebuild | Existing Release cache with gameplay enabled and netplay disabled; linked `th07.exe` | PASS | Build evidence only |
| 2026-08-26 | ResultScreen WebSocket/rollback Web rebuild | Existing Emscripten gameplay/netplay cache | PASS, `ninja: no work to do` | No live result/replay run |
| 2026-08-26 | Supervisor final hunk accounting | Disposed Supervisor C01-C48/H01-H02 against `d93bc94`; native input/network/audio preference slices excluded, Eagler lane/seed/lifecycle adaptations retained | PASS, 50/50 hunks disposed | Native shell excluded; live full lifecycle run pending |
| 2026-08-26 | Supervisor focused contract test | `python audit/feature-rebase/supervisor-feature-rebase-test.py`: frozen counts, logical lane storage, local raw input, pre-simulation override, rollback capture, life/slow normalization, room seed, portable audio and native/hash exclusion | PASS, 12/12 checks | Structural evidence; no live full lifecycle run |
| 2026-08-26 | Supervisor ordinary desktop rebuild | Existing Release cache with multiplayer and netplay disabled; linked `th07.exe` | PASS | Build evidence only |
| 2026-08-26 | Supervisor multiplayer desktop rebuild | Existing Release cache with gameplay enabled and netplay disabled; linked `th07.exe` | PASS | Build evidence only |
| 2026-08-26 | Supervisor WebSocket/rollback Web rebuild | Existing Emscripten gameplay/netplay cache | PASS, `ninja: no work to do` | No live full lifecycle run |
| 2026-08-26 | Entry/main final hunk accounting | Disposed main C01-C11 against `d93bc94` and `022c533`; native startup/window/test shell excluded and browser room/session lifecycle retained | PASS, 11/11 hunks disposed | Real host-to-iframe launch and one-peer-close run pending |
| 2026-08-26 | Entry/main focused contract test | `python audit/feature-rebase/main-entry-feature-rebase-test.py`: hunk count, native exclusion, SDL callbacks, room mode/seed/slot/loadouts/Stage 4 configuration, persistence ownership, lifecycle and hash boundary | PASS, 13/13 checks | Structural evidence; no real two-end room launch |
| 2026-08-26 | Entry/main ordinary desktop rebuild | Existing Release cache with multiplayer and netplay disabled | PASS, `th07` target built | Build evidence only |
| 2026-08-26 | Entry/main multiplayer desktop rebuild | Existing Release cache with gameplay enabled and netplay disabled | PASS, `th07` target built | Build evidence only |
| 2026-08-26 | Entry/main WebSocket/rollback Web rebuild | Existing Emscripten gameplay/netplay cache | PASS, target current (`ninja: no work to do`) | Browser lifecycle not exercised live |
| 2026-08-26 | Mature rollback stalled-input retry contract | `python audit/feature-rebase/netplay-rollback-feature-rebase-test.py`: final upstream inventory, native-shell/hash exclusions, TH07 prediction, 32-frame tail, journal/capacity/side-effect/stall/close policies and current-frame retry without `CaptureLocalInput` | PASS, 16/16 checks | Structural evidence does not itself prove a browser relay recovery |
| 2026-08-26 | Mature rollback C++ focused tests | Fresh local `g++` builds of `tests/netplay-core-test.cpp`, `tests/rollback-journal-test.cpp` and `tests/netplay-input-test.cpp` with multiplayer input guards/SDL headers | PASS: core, sparse rollback journal and logical input lanes | Does not emulate browser scheduling or physical touch events |
| 2026-08-26 | Mature rollback ordinary/multiplayer/Web rebuilds after retry fix | Rebuilt ordinary desktop, gameplay multiplayer desktop, production shared-font Web and acceptance-only embedded-font Web caches | PASS, all four targets linked | The embedded-font cache exists only so a direct local server can run without the production package-font installer |
| 2026-08-26 | Live 3P WebSocket jitter/loss rollback | Three real in-app browser tabs, 1200 frames, relay 20 ms delay plus 15 ms jitter and every seventh forwarded message dropped. All peers reached confirmed frame 1199 with pause/resume 1/1; rollback counts 70/62/60, resimulated frames 164/203/187 and maximum rollback spans 6/8/7. The initially dropped frame-zero deadlock no longer occurred because the already-captured current input was retried | PASS; all three existing diagnostic final-state partitions were identical | Scripted inputs, Stage 1 only; existing runtime partition diagnostics were merely observed and were not changed, gated or repaired |
| 2026-08-26 | Live 2P physical keyboard plus jitter/loss | P0 used `netplayPhysicalInput=1` and browser canvas keyboard events; P1 used the deterministic script. Both completed 600/600 frames, confirmed 599, and agreed on the existing final diagnostic state; P0 reported `physicalInput=1`, 27 rollbacks/59 resimulated frames, P1 3/8 | PASS; local physical input was captured and synchronized without resampling during rollback | Browser keyboard evidence only; no phone touch gesture, audible or visual acceptance claim |
| 2026-08-26 | BrowserPeerTransport 2P RTC + emergency fallback | Focused Emscripten harness used the production C++/EM_JS transport. RTC path established reliable `th07-control` plus unordered `th07-input` and exchanged both binary message classes; a second run disabled `RTCPeerConnection` and exercised the WebSocket route barrier | PASS; RTC route selected host↔host UDP locally, forced fallback selected relay on both endpoints | Local same-host browser candidate path; not public NAT or TURN evidence |
| 2026-08-26 | BrowserPeerTransport 3P full mesh | Three independent browser pages established every RTC peer edge with both control/input channels before the server released route `rtc` | PASS; all three endpoints `rtc` and bidirectional harness traffic completed | Same-host Chromium only; no mixed-NAT 3P claim |
| 2026-08-26 | Full launcher room→TH07MP RTC run | Two independent Chromium contexts created/joined a room, readied, launched the production TH07MP Runtime and used BrowserPeerTransport through the real launcher/session chain | PASS; logical frames reached 124/128, transports `rtc`, selected local ICE paths `direct/direct` | Scripted/headless endpoints; not phone touch or public-NAT evidence |
| 2026-08-26 | TURN signaling credential contract | Temporary server on a test port supplied STUN plus configured TURN URLs using coturn REST-style expiry username and HMAC-SHA1 credential from a server-only shared secret | PASS; independent test recomputed TTL and HMAC | No real TURN allocation/candidate was attempted |
| 2026-08-26 | TURN security-practice review | Compared coturn TURN REST auth/quota controls with current Cloudflare/Twilio TURN credential guidance | Room-membership binding downgraded from a hard deployment gate to optional defense in depth; baseline remains server-only long-lived secret + short-lived client credentials + quota/rate/usage controls | Research/architecture decision only; no public TURN server deployed |
| 2026-08-26 | Minimal coturn deployment contract | Added `tools/netplay/render-coturn-config.cjs`, fail-closed `coturn.env.example` and `TURN.md`; renderer shares the signaling REST secret and emits REST auth, no CLI/multicast, private/reserved IPv4 peer blocks, allocation quotas, bandwidth caps and optional TLS | `node tests/coturn-config-contract-test.cjs`: PASS; signaling contract also rejects TURN URLs with no credentials | Local config-generation evidence only; `turnserver` is not installed in this Windows workspace and no real TURN allocation was exercised |
| 2026-08-26 | RTC route-release skew / frame-zero session gate | Full launcher test twice reproduced both runtimes at frame 0 after route `rtc`: one endpoint's first HELLO arrived while the other DataChannel was open but its local route event had not yet run, so the old handler discarded the packet. `BrowserPeerTransport` now retains RTC packets while route is undecided, symmetric with relay pre-route buffering | Rebuilt TH07MP Web Runtime; focused 2P/3P/fallback PASS; deterministic 300 ms one-peer route-delay regression PASS; full launcher run PASS at frames 133/136 with `rtc` / `direct` on both endpoints | Transport/session startup fix only; rollback/gameplay semantics unchanged |
