# TH10 multiplayer adaptation

Status: native multiplayer gameplay and title rollback validation in an isolated
Eagler experiment. This is not a released multiplayer Runtime or a product-
capability declaration. See [Rollback acceptance](rollback-acceptance.md) for
the narrower evidence and remaining blockers.

## Baseline and isolation

- Upstream tracking: `portable`, `0074e58`.
- Eagler integration base: `eagler`, `5da6a70`.
- Experiment: `experiment/th10-multiplayer`.
- Shared runtime dependency: `5e74214` (`experiment/th10-rollback-frontier`,
  shared session channel and transactional packet/frontier validation).
- Ordinary gameplay, Replay formats, storage and build outputs retain their
  existing behavior. Multiplayer requires a separately compiled Runtime.

The user's current instruction is to commit after a substantial item has been
completed and validated, not after each small patch. Preserve progress records
between those commits. No implicit push, deployment or canonical promotion.
Subagents must use Luna with xhigh reasoning.

### Functional completion precedes performance work

Finish the entire TH08/TH10 multiplayer functional and correctness profile
before changing performance policy or implementation. Preserve the ordinary
single-player baseline throughout this phase. Commit that complete functional
boundary, then assess optimizations in separate changes with independent
ordinary and multiplayer regression evidence. A shared optimization must not
be mixed with multiplayer bring-up: otherwise a single-player divergence loses
its independently attributable cause. Do not replace accepted goldens.

The ordinary TH10 WASM rebuilt after the network slice is still exactly
`1f5a9557f62f33c1243caa72a8649cd8335944665ceb70da763b8f0fe79f67e1`.
`artifacts/multiplayer-tests/build-sp-after-network.log` records that build.
This byte identity is baseline evidence, not multiplayer product acceptance.

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

The ordinary baseline has one `GameActors::player`, bomb and economy. The MP
source set now binds explicit `World::pilots`, per-seat input/resource owners,
and one shared stage. `GameEconomy` views bind `PilotEconomy` and `TeamEconomy`
once; no whole-world duplication or rotating global current-player context is
used. `GameActors::player` retains the seat-zero compatibility view.

Native GUI state and authored ANM remain shared and must use the same seat-zero
source on all endpoints. Selecting local-player resources in that owner caused
the P1/P2 native GUI digests to diverge on a P2 Bomb. The custom MP resource
renderer may highlight the local seat, but must not change shared GUI updates.

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

Those focused suites do not instantiate the native game world. Browser tests
and their explicitly limited owner coverage are tracked separately in
[Rollback acceptance](rollback-acceptance.md).

The common/title rule lane has 19 passing WASI entries, including header and
browser-stub compatibility, transactional packet rejection and final-ACK
generation repair. Actual native browser transport evidence now exists too;
see [Network acceptance](network-acceptance.md). It is still not full-product,
cross-device, Replay, spectator or Launcher acceptance.
