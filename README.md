# Eagler Common Runtime

Shared runtime infrastructure for Eagler Touhou ports.

License: MIT.

This directory is the single source of truth for code that is independent of a
specific Touhou title.  Game repositories keep adapters for title-owned state
and may keep thin compatibility headers during migration, but shared algorithms
must not be forked back into per-title implementations.

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

Published/active convergence slices:

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

The next shared input slice adds the already validated once-only DirectTouch
model: device displacement is captured once as `DirectTouchBegin` / `Delta`,
unapplied limited-speed movement lives in rewindable `DirectTouchState`, and
`RollbackCore::LocalFrameForCapture()` makes delayed scheduling addressable
without re-sampling the producer. Prediction is conservative by default: a
missing delta contributes zero new displacement. Equivalent-prediction
acceptance, main-thread frame budgets and performance telemetry remain separate
slices.

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

Future intended slices include DirectTouch equivalence, broader connection
health policy, browser-frame budgeting and broader performance diagnostics once
their title-facing seams are explicit.

## Convergence order

Use this order unless new evidence proves a dependency requires otherwise:

1. exact duplicates: `NetplaySession`, `WebSocketTransport`;
2. near-identical wire/core code: `NetplayProtocol`, `NetplayCore`;
3. generic storage: `RollbackJournal` plus fixed-pool/sparse snapshot helpers;
4. browser transport: `BrowserPeerTransport`;
5. generic input authority: `NetplayInput`;
6. once-only incremental DirectTouch and delayed capture mapping;
7. bounded reliable input repair;
8. DirectTouch equivalence, broader health policy and rollback driver budgets;
9. performance testkit and diagnostics;
10. only then consider broader platform/presentation helpers.

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
