# Eagler Common Runtime

Shared runtime infrastructure for Eagler Touhou ports.

License: MIT.

This directory is the single source of truth for code that is independent of a
specific Touhou title. Game repositories keep adapters for title-owned state;
once a shared authority is adopted, consumers include it directly rather than
keeping forwarding headers or forking the algorithm back into title code.

## Ownership boundary

Good common-library candidates:

- netplay protocol/session/core algorithms;
- transport and browser networking;
- rollback journals and fixed-pool snapshot primitives;
- generic input scheduling/prediction/repair;
- fixed-step/browser lifecycle helpers;
- diagnostics and performance-test primitives.

Keep title-specific:

- rollback state inventory and object layout;
- canonical hashes of title-owned state;
- Bullet/Enemy/ECL/Stage/Bomb ownership rules;
- native Replay format semantics;
- title-specific gameplay lifecycle adapters.

The rule is: **share algorithms and infrastructure, not object layouts**.

## Migration policy

Migration is intentionally incremental.  Extract exact duplicates first, prove
both games still pass their existing gates, then converge near-duplicates.  Do
not combine common-library extraction with gameplay-state or protocol changes in
the same step.

Validated convergence slices in the current local v0.13 candidate line:

- `NetplaySession`
- `WebSocketTransport`
- `NetplayProtocol`
- `NetplayCore`
- `RollbackJournal`
- generic rollback primitives: `SnapshotPolicy`, `SparsePoolCapture`,
  `PartitionedPoolJournal`
- `BrowserPeerTransport`
- `NetplayInput`
- incremental DirectTouch primitives: `DirectTouchBegin` / `DirectTouchDelta`,
  `DirectTouchState`, delayed capture mapping and conservative delta prediction
- bounded reliable input repair primitives: `InputRepairBudget` and
  `BrowserPeerTransport::SendRepairTo`
- DirectTouch equivalence proof plus explicit Core APIs for title adapters to
  confirm an already-simulated predicted input after proving state equivalence
- allocation-free per-peer frame-advantage smoothing with the shared
  64-sample trimmed-mean policy used by TH06/TH07
- browser catch-up `FrameBudget` preserving the validated six-tick cap plus an
  8 ms wall-time boundary for starting additional fixed ticks
- shared frame-pacing formula preserving the long-lived TH06/TH07 lead filter,
  deadband, ±2% clamp and smoothing constants
- confirmed-input liveness watchdog preserving the long-lived 15-second
  production peer-progress timeout while allowing title test harness overrides
- aggregate confirmed-remote frontier directly from `RollbackCore`, so room
  drivers do not duplicate sentinel/minimum semantics

`NetplaySession` and `WebSocketTransport` were byte-identical before extraction.
`NetplayProtocol` differs only through a deliberately tiny title-owned wire
capability seam (`NetplayProtocolConfig.hpp`): the game magic byte and maximum
supported analog mode. The shared
Protocol header and `NetplayCore` preserve the behavior of the current TH06 and
TH07 `eagler` branches exactly; newer uncommitted title experiments are not
folded into this authority migration.

`RollbackJournal` moved only after TH06 completed its measured optimization pass
against the TH07 playbook. The shared implementation preserves TH06's validated
allocation/index/copy/restore behavior and the TH07-only snapshot-patch API used
by its once-only DirectTouch equivalence path. Pool layout remains title-owned;
only the generic storage algorithms live here.

`BrowserPeerTransport` was byte-identical between TH06 and TH07 after replacing
only the title tag (`th06`/`th07`). The shared implementation keeps that tag as
the tiny title-owned `NetplayTransportConfig.hpp` seam so existing DataChannel
labels and legacy browser globals remain stable. Bounded reliable repair is now
part of this shared transport authority; broader performance telemetry remains
a separate concern with its own acceptance criteria.

`NetplayInput.hpp` was byte-identical between TH06 and TH07. Their implementation
also becomes byte-identical after removing TH06's legacy `Multiplayer.hpp`
player-count alias. The shared implementation therefore uses protocol
`MAX_PLAYERS` and delegates only the title-owned synchronized gameplay-lane
writeback to `NetplayInputConfig.hpp::CommitGameInputs()`.

The shared once-only DirectTouch model captures device displacement once as
`DirectTouchBegin` / `Delta`,
unapplied limited-speed movement lives in rewindable `DirectTouchState`, and
`RollbackCore::LocalFrameForCapture()` makes delayed scheduling addressable
without re-sampling the producer. Prediction is conservative by default: a
missing delta contributes zero new displacement. Equivalent-prediction
acceptance, browser catch-up budgeting and performance telemetry remain
separate authorities.

Reliable input repair is deliberately not a second input transport. Normal RTC
input remains unordered/unreliable. After the first unacknowledged input frame
has made no progress for `InputRepairBudget::StalledMs`, a consumer may send the
already-captured redundant packet through that peer's reliable control channel.
Healthy ACK progress produces zero repair traffic; control-channel backpressure
simply skips an attempt and does not fail the gameplay transport.

The shared `testkit/rtc-input-impairment.cjs` fixture can delay, jitter, drop or
black out the real RTC input DataChannel while giving reliable repair traffic
the same artificial application-send delay. Title harnesses provide their
input/control channel labels; the fixture contains no TH06/TH07 product policy.

DirectTouch equivalence is deliberately adapter-proven rather than inferred by
`RollbackCore`. The shared proof accepts only a narrow, saturated single-axis
limited-touch correction that cannot cross a movement boundary or gesture
reset. A title must provide the historical applied/remaining trace and patch
its own rollback snapshots before calling `SubmitEquivalentRemoteInput()`.
That commit API has a dedicated result type; `NotPredicted` is distinct from an
invalid player and from ordinary packet-processing results so adapters cannot
silently confuse a failed equivalence precondition with transport input state.

`FrameAdvantageWindow` is shared because the measured TH06 and TH07 algorithms
are the same: each peer owns an independent 64-sample ring, waits for 20
samples, trims up to four values from each tail and averages the remainder.
The shared form removes the per-packet temporary vector allocation. The
independent pacing formula is owned by `FramePacingPolicy`; peer membership,
sample de-duplication and telemetry remain in the consumer driver.

`FrameBudget` is not an input-delay mode. It governs browser event-loop
ownership after a callback is already behind: one due tick always runs, then
additional catch-up ticks may start only while the validated 8 ms wall-time
budget and six-tick cap allow it. Remaining accumulator debt is retained for a
later callback. This preserves the production TH07 policy and lets TH06 share
the same scheduler rule without adding local input frames.

`FramePacingPolicy` is also nondeterministic scheduling support, not gameplay
state. TH06 and TH07 have used the same policy since their original multiplayer
drivers: infer relative lead from frame-advantage exchange, reject samples past
30 frames, apply a 0.5-frame deadband, target 1.0 + lead*0.003 clamped to
0.98..1.02, smooth by 0.08 and snap within 0.0002 of unity. Peer membership,
packet de-duplication, telemetry and diagnostic disable switches stay title-owned.

`ConfirmedInputWatchdog` likewise owns no room lifecycle. Once a title decides
a remote peer is required and the session is active, it observes that peer's
confirmed input frontier and reports a timeout only after the frontier remains
unchanged for 15 seconds. Disarming, choosing a longer hidden-test timeout and
turning the timeout into a visible room failure remain title responsibilities.

`RollbackCore::ConfirmedThroughAllRemotes()` owns the aggregate frontier
because the core already owns player count, local-player identity and each
peer's confirmation state. It returns `INVALID_FRAME` until every remote has
confirmed input, then the minimum remote frontier. Replay, spectator and shared
UI policy built on that frontier remain title-owned.

## Current extraction stop line

The v0.13 candidate intentionally stops before a shared title driver.
TH06/TH07 still contain several byte-identical orchestration functions for
session retry, packet drain, spectator publication and scheduled input send.
Those functions compose many already-shared authorities plus title/session
state; extracting them now would effectively design a universal
`NetplayDriver` from only two closely related consumers.

Do **not** create v0.14 merely to remove that orchestration duplication. Reopen
the driver boundary when TH08/TH10 multiplayer or another third production
consumer exists and can validate the interface shape.

`RollbackReplayBudget` is retained as a shared **experimental primitive**
because the TH07 performance work already used the same generic 4 ms /
four-frame slicing rule. It remains opt-in incremental-reconcile infrastructure:
the Launcher does not enable it, it is not an input-delay mode, and it must not
be presented as part of the production timing profile. A second production
consumer is still required before replay-slicing becomes a shared product
policy. Broader diagnostics/testkit work may move independently when it is
title-neutral and already proven reusable.

## Convergence order

Use this order unless new evidence proves a dependency requires otherwise:

1. exact duplicates: `NetplaySession`, `WebSocketTransport`;
2. near-identical wire/core code: `NetplayProtocol`, `NetplayCore`;
3. generic storage: `RollbackJournal` plus fixed-pool/sparse snapshot helpers;
4. browser transport: `BrowserPeerTransport`;
5. generic input authority: `NetplayInput`;
6. once-only incremental DirectTouch and delayed capture mapping;
7. bounded reliable input repair;
8. DirectTouch equivalence;
9. allocation-free per-peer frame-advantage smoothing;
10. browser catch-up start budget;
11. shared frame-pacing formula;
12. confirmed-input peer liveness watchdog;
13. aggregate confirmed-remote frontier;
14. **stop and validate with a third production consumer before extracting
    driver orchestration**;
15. independently share proven title-neutral performance testkit/diagnostics;
16. only then reconsider broader platform/presentation helpers.

Every step must leave TH06 and TH07 buildable and testable independently. A
consumer may advance to a newer common implementation only after its own
title-specific rollback/canonical gates pass. Do not make a common refactor and
a gameplay-state change in the same acceptance step.

## Version/authority rule

`eagler-common` is the implementation authority. Mature shared authorities are
included directly from `<eagler/netplay/...>`; consumers must not reintroduce
title-local forwarding headers or marker `.cpp` files for those names. Keep
only genuine title-owned seams such as `NetplayProtocolConfig.hpp`,
`NetplayTransportConfig.hpp`, `NetplayInputConfig.hpp`, canonical hashes,
rollback state owners and game drivers.

The canonical published layout uses a pinned `third_party/eagler-common` Git
submodule in each game repository, pointing at `YomotsuHisami/eagler-common`.
The workspace sibling remains a development fallback and an explicit
`EAGLER_COMMON_ROOT` override remains available for controlled experiments.

Consumers link `eagler::netplay_base` for Protocol/Core and may independently
link `eagler::browser_peer_transport` and `eagler::netplay_input`. These remain
INTERFACE components so shared sources compile with the consumer's platform
toolchain and title-owned config seams. Protocol uses
`<netplay/NetplayProtocolConfig.hpp>` for wire capability; browser transport
uses `<netplay/NetplayTransportConfig.hpp>` only for its stable title tag; input
uses `<netplay/NetplayInputConfig.hpp>` only to commit synchronized logical
buttons into title-owned gameplay lanes.

Do not push a consumer commit that requires the common library until the common
repository/revision it names is available remotely. CI/release builds must pin
an exact common commit; they must never fetch an unpinned branch head.

Until a stable release policy is declared, the dependency version is the exact
Git commit recorded by the consumer submodule. Breaking shared-runtime changes
must be validated against every consumer before advancing those gitlinks.

## Standalone validation

`eagler-common` has a minimal consumer fixture so the repository can validate
its current component contract without checking out a Touhou game repository:

```text
cmake -S . -B build -DEAGLER_COMMON_BUILD_TESTS=ON
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

The TH06/TH07 workspace integration contract remains a separate gate because
it validates real consumer shims, CMake wiring and full Web builds.
