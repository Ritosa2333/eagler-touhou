# Presentation Lab Controller Core

This directory owns title-neutral orchestration for presentation diagnostics:

- stop the display loop without invoking an in-game pause;
- advance the real fixed-step simulation without skipping ticks;
- issue draw-only alpha samples without advancing simulation;
- repeat samples for idempotence and compare state fingerprints;
- run bounded scans and retain compact reports.

The consumer adapter owns every title-specific concern: record wire layout,
owner identities, state fingerprints, scene metadata, continuity policy,
classification and report schemas. The common controller must not know Touhou
object layouts, Replay menus or stage numbering.

Production runtimes must not expose the diagnostic adapter. Consumers enable
it through a compile-time profile and pin the exact `eagler-common` revision.

Run the standalone contract with:

```text
node --test testkit/presentation-lab/controller-core.test.mjs
```
