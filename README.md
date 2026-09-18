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

`NetplaySession` and `WebSocketTransport` were byte-identical before extraction.
`NetplayProtocol` differs only through a deliberately tiny title-owned wire
capability seam (`NetplayProtocolConfig.hpp`): the game magic byte. The shared
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
labels and legacy browser globals remain stable. This authority migration does
not include the later experimental input-repair path or performance telemetry;
those remain separate changes with their own acceptance criteria.

`NetplayInput.hpp` was byte-identical between TH06 and TH07. Their implementation
also becomes byte-identical after removing TH06's legacy `Multiplayer.hpp`
player-count alias. The shared implementation therefore uses protocol
`MAX_PLAYERS` and delegates only the title-owned synchronized gameplay-lane
writeback to `NetplayInputConfig.hpp::CommitGameInputs()`. The extraction keeps
the existing capture/replay/override semantics only; incremental DirectTouch
remainder, reliable input repair and performance telemetry are not part of this
slice.

Future intended slices include incremental DirectTouch ownership, input
repair/health logic and the shared performance testkit once their title-facing
seams are explicit.

## Convergence order

Use this order unless new evidence proves a dependency requires otherwise:

1. exact duplicates: `NetplaySession`, `WebSocketTransport`;
2. near-identical wire/core code: `NetplayProtocol`, `NetplayCore`;
3. generic storage: `RollbackJournal` plus fixed-pool/sparse snapshot helpers;
4. browser transport: `BrowserPeerTransport`;
5. generic input authority: `NetplayInput`;
6. incremental DirectTouch, input repair/health and rollback driver helpers;
7. performance testkit and diagnostics;
8. only then consider broader platform/presentation helpers.

Every step must leave TH06 and TH07 buildable and testable independently. A
consumer may advance to a newer common implementation only after its own
title-specific rollback/canonical gates pass. Do not make a common refactor and
a gameplay-state change in the same acceptance step.

## Version/authority rule

`eagler-common` is the implementation authority. Per-game files with the same
old names may exist only as forwarding/compatibility shims while migration is in
progress. They must not contain a second implementation.

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
