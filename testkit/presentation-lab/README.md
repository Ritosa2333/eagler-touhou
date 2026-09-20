# Presentation Lab

This directory owns title-neutral contracts and orchestration for presentation
diagnostics:

- stop the display loop without invoking an in-game pause;
- advance the real fixed-step simulation without skipping ticks;
- issue draw-only alpha samples without advancing simulation;
- repeat samples for idempotence and compare state fingerprints;
- run bounded scans and retain compact reports.

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
node --test testkit/presentation-lab/controller-core.test.mjs
```
