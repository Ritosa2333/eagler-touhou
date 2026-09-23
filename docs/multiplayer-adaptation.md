# TH10 multiplayer adaptation

Status: implementation in an isolated Eagler experiment. The cooperation-rule
owner is a preparatory component; it is not a playable multiplayer Runtime or
a product-capability declaration.

## Baseline and isolation

- Upstream tracking: `portable`, `0074e58`.
- Eagler integration base: `eagler`, `5da6a70`.
- Experiment: `experiment/th10-multiplayer`.
- Shared runtime dependency: `8316c4f861dedb67e1e0e7be75ddcf0b90f63448`.
- Ordinary gameplay, Replay formats, storage and build outputs retain their
  existing behavior. Multiplayer requires a separately compiled Runtime.

The user requested a persistent completion goal and no commits before the
complete adaptation is finished. Subagents must use Luna with xhigh reasoning.

## Agreed cooperation rules

Use TH07's cooperative model: one shared stage, enemies, bullets and score;
personal lives and power; spirit rescue and synchronized team-wipe/retry.
TH10 has no separate stock of bombs: a native bomb consumes 20 units of the
acting player's power. Its six character/shot combinations remain distinct.

TH10's native deathbomb window, power loss, drops and spell-specific bomb
behavior remain title-owned. The cooperative lifecycle receives a completed
native death; it must not apply the native death penalties a second time.

TH07's cooperation evidence is in `Player.cpp`'s `UpdateLifeTransfer`,
`SelectLifeTransferReceiver` and `UpdateTeamWipeRetryCountdown`. Rescue uses a
20-unit radius, 90 continuous eligible logical ticks and one donor spare life.
Holding Fire prevents the transfer; a successful transfer requires Focus to
be released before another one. Wipe grace is 180 gameplay ticks and freezes
with synchronized pause/time-stop. Recovery must reset wipe state, including
when a life reward arrives after exhaustion.

TH07's Cherry formula, Stage 4 chained-card damage exception, P1-only final
resource bonus and reported resurrection/game-over defect are not TH10 rules.

## State ownership and integration

`World` currently has one `GameActors::player`, bomb, input lane and economy.
The second device profile in `Input` is not an implemented second pilot.
`GameEconomy` mixes personal character/shot/power/lives with shared score,
faith timer, point value, rank, difficulty and stage progression. Do not copy
the entire economy per seat and accidentally create several shared worlds.

Reuse the native Player environment seams with explicit per-pilot contexts.
Collision, enemy targeting, shots, bomb damage and item attraction must choose
or visit those contexts explicitly. Shared world systems run once per logical
tick. Multiplayer state and integration code belong outside the ordinary
build's source set.

`eagler-common` owns protocol, barrier, input history, prediction, transport
and rollback storage algorithms. This title owns gameplay semantics and the
complete save/restore inventory: ECL/RNG, player/options/shots, enemies,
bullets/lasers/items, ANM allocation state, spell, economy, session and results.
World presentation caches are not authoritative game state.

## Acceptance before promotion

1. Separate ordinary/MP build, storage and Replay identities; retain ordinary
   quick/daily golden Replay gates.
2. Rule-owner tests for 2P/3P rescue, concurrent recipients, life rewards after
   exhaustion, pause and exact wipe timing.
3. Runtime tests for all six loadouts, per-seat power/bomb/deathbomb, concurrent
   damage, pickup ownership, shared faith/score/extends and final bonuses.
4. Frame-zero barrier, captured input, late correction and complete owner
   restoration compared with an uninterrupted run.
5. Restart generations, stages/results, MP Replay, spectator and Launcher
   room lifecycle, followed by actual transport/browser acceptance.
6. Review the entire experiment diff before canonical Eagler integration.

The focused rule gate is `node portable/check-multiplayer-rules.mjs`; it uses
the existing WASI SDK test lane and requires no retail resources. Both suites
pass:

- Cooperation: rescue timing/range, Fire cancellation, Focus-release latch,
  3P recipient priority, synchronous item allocation and life debit ordering,
  failed allocation, explicit life-award recovery, native life bounds and
  repeated wipe/recovery transitions.
- Shared-core integration: a Focus release on the final rescue tick arrives
  eight frames late. The real common `RollbackCore` and `RollbackJournal`
  restore the wrongly predicted rescue, donor life, recipient state and
  rescue progress, then reproduce every corrected rule-state boundary.

These suites do not instantiate the native game world or constitute a
multiplayer Runtime test. Native players, item allocation and all other world
owners still need integration and their own restoration proof.

Current infrastructure evidence: the pinned common library's
`netplay-base-test.cpp` passes as Emscripten/WASM under Node, covering protocol,
core input/confirmation and session barriers. It does not exercise an actual
browser transport. Multiplayer gameplay, title rollback, browser and
cross-device acceptance remain pending.
