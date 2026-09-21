# Presentation Lab

## Release status

The title-neutral Presentation Lab contract is public at v1. TH08 and TH10
are the first two consumers; their exact feature and observation coverage
remains title-owned and is not implied by this common release. TH06 and TH07
must not advertise Lab support until their draw chains stop mutating
authoritative or rollback-visible state.

Consumers pin an exact `eagler-common` commit as a Git submodule and include
these modules directly. There is no copied or bundled title-local edition of
the common workbench:

```js
import {PresentationLabControllerCore} from
  './third_party/eagler-common/testkit/presentation-lab/controller-core.mjs';
import {analyzeNormalizedWindow} from
  './third_party/eagler-common/testkit/presentation-lab/analyzer.mjs';
```

`DRIVER_API_VERSION` and `OBSERVATION_API_VERSION` are compatibility majors.
Breaking method, receipt, record or purity semantics require a new major;
additive report fields may remain within v1. Every report still records the
exact common revision, adapter version and native ABI because a major alone is
not a reproducible build identity.

This directory owns title-neutral contracts and orchestration for presentation
diagnostics:

- stop the display loop without invoking an in-game pause;
- advance the real fixed-step simulation without skipping ticks;
- issue draw-only alpha samples without advancing simulation;
- repeat samples for idempotence and compare state fingerprints;
- run bounded scans and retain compact reports.

`analyzer.mjs` accepts normalized records and treats missing state coverage as
`unknown`, not a purity pass. Title adapters may retain a legacy analyzer while
they migrate their native ABI, but new consumers should emit the normalized
frame and field model described by the contract.

`workbench.html`, `workbench.mjs` and `workbench.css` own the shared browser
workflow after it was exercised by a second title.  A consumer serves a
title-owned `/lab-config.mjs`; DATA mounting, runtime configuration, Replay
paths, owner semantics and native ABI remain outside the common workbench.

The consumer implements the `RuntimeDriverV1` and `ObservationAdapterV1`
boundaries in [CONTRACT.md](CONTRACT.md). It owns every title-specific concern:
runtime ABI, complete tick transaction, record wire layout, owner identities,
state fingerprints, scene metadata, continuity policy and Replay navigation.
The common controller must not know Touhou object layouts, WASM export names,
Replay menus or stage numbering.

Production runtimes must not expose the diagnostic adapter. Consumers enable
it through a compile-time profile and pin the exact `eagler-common` revision.

Run the standalone contract with:

```text
node --test testkit/presentation-lab/controller-core.test.mjs testkit/presentation-lab/release-contract.test.mjs
```

Release readiness requires the contract suite, two pinned consumers, isolated
diagnostic/production profiles, and an explicit `unknown` result for every
uncovered state or observation group. A title's user-played acceptance is
reported separately from common component publication.
