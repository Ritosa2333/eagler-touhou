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

Future intended slices include browser peer transport, generic input ownership
and the shared performance testkit once their title-facing seams are explicit.

## Convergence order

Use this order unless new evidence proves a dependency requires otherwise:

1. exact duplicates: `NetplaySession`, `WebSocketTransport`;
2. near-identical wire/core code: `NetplayProtocol`, `NetplayCore`;
3. generic storage: `RollbackJournal` plus fixed-pool/sparse snapshot helpers;
4. browser transport: `BrowserPeerTransport`, input repair and health logic;
5. generic input semantics and rollback driver helpers;
6. performance testkit and diagnostics;
7. only then consider broader platform/presentation helpers.

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

Consumers link the CMake component target `eagler::netplay_base`. It remains an
INTERFACE component so the shared sources compile with the consumer's platform
toolchain and its title-owned `<netplay/NetplayProtocolConfig.hpp>` capability
seam. The config may describe wire compatibility; it must not grow into a
second title-specific Protocol/Core implementation.

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
