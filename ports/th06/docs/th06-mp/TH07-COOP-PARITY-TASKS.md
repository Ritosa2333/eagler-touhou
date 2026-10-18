# TH06 Multiplayer - TH07 Cooperative Feature Parity Tasks

This ledger answers one narrow question: which multiplayer-only gameplay features
added on top of vanilla TH07 are generic cooperative-shooter features that TH06
should also have, and which are inseparable from TH07's own game systems.

The rule is not "copy TH07". TH06 vanilla mechanics remain authoritative. TH07 is
only an implementation/design reference for multiplayer behavior that also makes
sense when expressed through TH06's own lives, bombs, Power, rank, items, bosses,
HUD and replay lifecycle.

## Classification

| Area | TH07 multiplayer addition | TH06 disposition | Current TH06 state | Task |
| --- | --- | --- | --- | --- |
| Stable player slots | P1/P2/P3 lifecycle, active/away/departed identity | SHOULD HAVE | Implemented | Keep/audit |
| Independent loadouts | Per-player character/shot/SHT/ANM/Bomb ownership | SHOULD HAVE, using TH06 Reimu/Marisa A/B | Implemented | Keep/audit |
| Independent resources | Per-player lives, bombs, Power | SHOULD HAVE | Implemented sidecars | Keep/audit |
| Shared-team extend | Shared progression reward reaches every active participant | SHOULD HAVE using TH06 score extends | Implemented; one team sound/rank adjustment | T01 complete |
| Multiplayer HUD | P1/P2/P3 resources and identity visible simultaneously | SHOULD HAVE using TH06 sidebar | Implemented; Web MP build compiles, visual acceptance pending | T02 code complete |
| Boss durability | Extra players must not multiply boss DPS unchecked | SHOULD HAVE using TH06 enemy damage path | Implemented at final boss-damage point: 2P x0.75, 3P x2/3 | T03 complete |
| Rank penalty normalization | Per-player deaths/bombs must not multiply shared rank decay | SHOULD HAVE using TH06 rank | Implemented for Bomb/Miss penalties | T04 complete |
| Revivable terminal death | A zero-stock player can remain in a non-colliding state while teammates survive | SHOULD HAVE, designed on TH06 DEAD/SPAWNING lifecycle | Implemented as deterministic `REVIVABLE`; focused dual-browser runtime probe passed | T05 complete |
| Teammate revival | Nearby survivor can spend one spare life to return a terminal teammate | SHOULD HAVE | Implemented through TH06 SPAWNING lifecycle; focused dual-browser runtime probe passed | T06 complete |
| Life transfer | Nearby survivor can donate one spare life to another player | SHOULD HAVE | Implemented with atomic targeted item spawn | T07 code complete |
| Power transfer | Nearby player can donate Power via a deliberate input gesture | SHOULD HAVE using TH06 Power items | Implemented as 8 taps/24 frames and 20 Power; focused dual-browser runtime probe passed | T08 complete |
| Targeted transfer items | Donated resources visibly travel to the chosen stable slot and are rollback-safe | SHOULD HAVE using TH06 item state machine | Implemented as deterministic states 6..8 before existing slot homing | T09 code complete |
| Transfer prompts | Local feedback for life/Power transfer interaction | SHOULD HAVE | Implemented; Web MP build compiles, visual acceptance pending | T10 code complete |
| Stage player labels | Optional P1/P2/P3 labels at stage start | SHOULD HAVE | Implemented using TH06's own menu/game-frame state; visual acceptance pending | T11 code complete |
| Per-player contribution stats | Kills/damage can be attributed per stable slot and optionally shown | SHOULD HAVE as multiplayer-only telemetry/UI | Implemented with rollback/hash state, deterministic attribution, HUD and Replay stage snapshots; 2P/3P browser probes passed, visual acceptance pending | T12 complete |
| Nearest-player targeting | Enemies/aimed logic target active logical players deterministically | SHOULD HAVE | Implemented | Keep/audit |
| Multi-player collision/graze | All active ships participate; inactive terminal slots do not | SHOULD HAVE | Largely implemented | Keep/audit after new revivable state |
| Item ownership/auto-collect | Item pickup and auto-collect resolve to a stable player | SHOULD HAVE | Implemented | Extend for transfers |
| Team wipe/end + Restart | Only total team failure starts the terminal grace; TH06MP then goes directly to Result and never exposes Continue. Separately, synchronized Pause-menu `R` starts a fresh attempt/new rollback generation like TH07 Reset. | SHOULD HAVE | Implemented for REVIVABLE + terminal players; Pause-R 2P/3P browser PASS | Keep deterministic 180-frame grace and never conflate quick Restart with Continue |
| Remote readability | Remote smoothing/fade/hitbox without affecting simulation | SHOULD HAVE | Implemented | Keep draw-only |
| Multiplayer Replay lanes | Record all logical input lanes/loadouts | SHOULD HAVE | Implemented | Audit exactness |
| Per-stage Replay resources | Direct later-stage playback restores each player's lives/bombs/Power | SHOULD HAVE | Implemented in TH06; TH07 fixed 2026-08-30 | Audit exactness |
| Death/revival Replay | Replay reproduces terminal state, drift, transfer target/timer and revive frame exactly | SHOULD HAVE | Source/RNG/rollback chain audited; focused co-op state and short 2P/3P saved-Replay playback probes passed | T13 audited, human acceptance pending |
| Cherry / Cherry+ | Shared Cherry economy and thresholds | TH07-ONLY | Not applicable to TH06 | EXCLUDE |
| Border | Shared Supernatural Border activation/break/gauge | TH07-ONLY | Not applicable to TH06 | EXCLUDE |
| TH07 point-item extend thresholds | Extend based on TH07 point counter | TH07-ONLY AS-IS | TH06 uses score thresholds instead | Replace with T01 |
| Sakuya/SHT-specific rules | TH07 third character and shooter-data behaviors | TH07-ONLY AS-IS | TH06 has Reimu/Marisa A/B | EXCLUDE/REMAP only where generic |

## Implementation order

### T01 - Shared TH06 score extends

- Keep TH06's original score thresholds and single shared score.
- At a threshold, grant one spare life to every active non-departed multiplayer
  participant that is below the TH06 life cap.
- Play the extend sound and apply the original rank increase once per team award,
  not once per player.
- Ordinary TH06 keeps the vanilla P1-only path.

### T02 - TH06 compact multiplayer HUD

- Reuse the original TH06 right-side panel and existing life/bomb sprites.
- Show P1/P2/P3 rows with lives, bombs and numeric Power; keep score/graze/point
  readable below/around the rows.
- Show AWAY/terminal state text where useful; no gameplay state depends on UI.
- Ordinary TH06 draw path remains unchanged.

### T03 - Boss durability scaling

- Scale only effective damage to bosses in multiplayer; do not rewrite ECL HP.
- Initial parity target: 2P effective boss damage x0.75, 3P x2/3, matching the
  proven TH07 co-op balance rule while preserving TH06's spell-specific damage
  reductions and vanilla 70-per-tick ceiling.
- Non-boss enemies retain combined player damage so normal stages do not become
  unnecessarily spongey.

### T04 - Shared rank penalty normalization

- TH06 has one rank/subrank curve. Death/Bomb penalties caused independently by
  several players must not multiply the rate of shared rank decay.
- Divide multiplayer per-player negative rank adjustments by active player count;
  leave time/shared positive adjustments unchanged.

### T05 - TH06 REVIVABLE state

- Add a multiplayer-only state after vanilla terminal death; do not import TH07's
  `SPIRIT` enum or Border/Cherry dependencies.
- The player is non-colliding, cannot shoot/graze/collect, remains visible with a
  translucent presentation and bounded lower-field drift.
- Drift should be simulation-deterministic and rollback/replay-safe. Prefer a
  stable slot/death-state-derived direction rather than introducing a new RNG
  dependency unless a recorded deterministic RNG dependency is demonstrably needed.
- If no survivor remains, the existing deterministic team-wipe grace and Retry
  lifecycle still takes over.
- Multiplayer terminal death must not set TH06's shared `extraLives = -1` single-
  player sentinel.

### T06/T07 - Teammate revival and life transfer

- Within 20 game pixels, holding Focus without Shoot for 90 logical frames arms a
  deterministic target chosen by: REVIVABLE priority, then lowest lives, then
  stable slot id.
- Giver must own at least one spare life.
- REVIVABLE receiver: giver loses one spare life; receiver returns through TH06's
  spawn/invulnerability lifecycle with zero spare lives.
- Live receiver: donate one spare life up to TH06's life cap using a targeted life
  item so the handoff is visible and deterministic.

### T08/T09 - Power transfer and targeted item movement

- Within 20 game pixels, eight Shoot taps inside a 24-frame rolling window donate
  20 Power to the lowest-Power eligible teammate (slot id tie break).
- Express the transfer through TH06's own Power items. Two big + four small Power
  items equal 20 Power in TH06 and preserve its existing item/Power thresholds.
- Extend TH06 item states 3..5 with a short deterministic transfer-rise phase before
  homing to the fixed stable player; do not change ordinary item states 0..2.

### T10/T11/T12 - Multiplayer presentation

- Draw interaction prompts and optional stage labels from presentation-smoothed
  positions only. Never feed smoothed positions back into selection/collision.
- Add per-slot damage/kill contribution counters only if they remain rollback/hash
  state and can be shown without touching ordinary save/replay formats.

### T13 - Replay exactness audit for TH06 + TH07

Replay acceptance is stricter than "input lanes load": it must reproduce the
logical game process the local player actually saw, excluding network/render stalls.

Audit and focused-test:

1. stage-start per-player lives/bombs/Power;
2. player state transitions, terminal death and revive frame;
3. transfer target selection and 90-frame/8-tap interaction state;
4. deterministic item ownership/trajectory/collection;
5. RNG seed/generation sequence at gameplay-affecting random calls;
6. boss HP/damage and shared rank;
7. active/away/departed slot state;
8. touch/analog/button lane data;
9. rollback speculative passes never append Replay side effects;
10. draw-only smoothing/fade is deliberately excluded from Replay simulation state.

No individual test in this workstream may run for more than three minutes. Long
full-run browser smoke tests must be replaced with short deterministic probes.

### T13 audit status - 2026-08-30

- TH06 and TH07 EAGX v1 now record the recorder's stable `localPlayer` slot in
  reserved header word 120. Playback restores that slot before Player objects
  are registered. Old v1 files keep their zero-filled P1 viewpoint.
- Both TH_DEV_TOOLS Web runtime round-trips cover a P3 recorder viewpoint,
  reject an out-of-range local slot, and accept the old zero-filled header.
- Stage-start P1/P2/P3 lives, bombs, Power and cumulative contribution HUD
  counters round-trip in both games.
- `temporarilyAbsent` and `permanentlyDeparted` currently have no production
  Stage-time writer. Room and Replay setup activate every slot below
  `playerCount`; transient network loss is handled by transport recovery and
  input prediction. No Replay departure event is required until gameplay gains
  a real authoritative join/leave transition.
- TH06 restores each vanilla stage RNG seed through `Rng::Initialize`; TH07
  restores `stageRngSeed`. Both rollback journals capture the complete RNG
  object before speculative simulation, and speculative passes do not append
  Replay frames. TH06 REVIVABLE/transfer movement adds no RNG; TH07 Spirit uses
  its two recorded-sequence RNG calls only when entering Spirit.
- Multiplayer `FrameInput` stores buttons, analog mode/coordinates, direct-touch
  unlimited mode, and touch-used/touch-bomb flags for every stable slot. The
  local recorder's separate touch-event sidecar remains presentation/control
  history and is not substituted for another player's movement lane.
- The short `coop2` dual-Chromium probe passed with rollback audit enabled. It
  exercised the real Player update path through terminal death -> REVIVABLE,
  a nearby 90-frame Focus revival, and eight Shoot edges producing the six
  targeted items for a 20-Power transfer. Both peers finished frame 600 with
  the same canonical state; one peer resimulated eight rollbacks.
- The old full-Stage-6 `replay2` flow was retired as a gate after it could not
  reach save/playback inside the three-minute workstream limit. Its replacement
  stops only after every input in a short recording window is confirmed, calls
  the real `ReplayManager::SaveReplay`, and returns through the real
  Supervisor -> MainMenu -> Replay-file selection path. `replay2` passed with
  both recorders beyond playback frame 300; `replay3` passed with all three
  recorders beyond frame 60. Every runtime loaded its own MP `.rpyx`, restored
  the correct player count and observed independent stable-slot inputs.
- Remaining acceptance is human visual/operation checking. A future fully
  natural (non-test-injected) death/revival/transfer occurrence can strengthen
  the combined saved-Replay evidence, but the test hook's injected setup is
  deliberately not misrepresented as Replay-recorded state.

### T12 hash boundary

Explicit hash-boundary approval was provided on 2026-08-30. TH06 now follows
TH07's mature contribution contract: per-slot kills/damage are rollback state,
are included in the canonical authoritative hash and local restore hash, and
use saturating `u32` counters. Actual post-spell/cap/Boss-scale damage is split
proportionally from each slot's effective damage; integer remainder and kills
use the highest-damage stable slot with lower-slot exact-tie precedence.
The TH06 room gameplay ABI is now 3, so older ABI2 peers are rejected during
the room gate instead of first diverging after an enemy takes damage.

The optional HUD state and cumulative per-stage values are carried by the EAGX
v1 multiplayer extension without changing vanilla replay/save layouts. The Web
round-trip accepts both the immediately previous v1 form without contribution
snapshots and the older v1 form without either resources or contributions.

Focused runtime evidence:

- `stress2`: both peers reached frame 600 through 128/124 rollbacks and agreed
  on P1/P2 contributions `9/432` and `13/624` (kills/damage).
- scripted `relay3`: all three peers reached frame 600 through 77/52/119
  rollbacks and agreed on `15/720`, `0/0`, `16/768`. The zero middle slot is
  deliberate because that stable lane moves but does not shoot.
- native resource/contribution unit test, MP and ordinary Web builds, TH06
  cooperative/Replay/presentation/rollback contracts, ReplayExtension browser
  round-trip, and TH07's corresponding contribution contracts passed.
