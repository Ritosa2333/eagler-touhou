# Multiplayer Playbook

Status: active

## Purpose

Route multiplayer, rollback, spectator, MP Replay and MP Launcher work without reviving the withdrawn early multiplayer path or mixing network transport, deterministic simulation and product packaging.

For rollback **optimization** rather than multiplayer architecture, use
`rollback.md`. It preserves the complete TH07 journal/input/snapshot/mobile
optimization exploration, including failed experiments and zero-delay evidence.

## Applicability and authority

Multiplayer implementation and rollback/netplay experiments must use a topic
worktree separate from upstream tracking and the canonical Eagler integration
worktree. Promotion follows
[Adaptation Worktree Isolation](adaptation-worktrees.md).

- TH06MP and TH07MP provide the reusable architecture and test evidence for the
  products that currently declare Multiplayer.
- Titles that do not declare `multiplayerRuntime + multiplayer` are outside
  this profile; do not describe that absence as unfinished adapter work.
- Multiplayer itself is an **optional product capability**. Once a product
  declares `multiplayerRuntime + multiplayer` in the shared product catalog,
  the profile-required Multiplayer behaviors are mandatory; do not add a
  second per-title feature switch for them.
- Browser transport claims come from reproducible transport and Launcher tests. A room connection or focused browser PASS is not human cross-device acceptance.

<!-- knowledge-id: K-MP-001 -->
## Normal design

Treat ordinary and MP variants as separately identified products whose
relationship comes from `product-catalog`, not from title-number branches.
Use separate runtime/build identity, URL or package identity, room/session state
and Replay semantics where required. Shared UI/service helpers are the default;
product facts such as loadout/difficulty bounds stay in the catalog.

The TH07MP reference layering is:

1. protocol: version, HELLO/READY, frame numbering, input, confirmation and targeted relay envelopes;
2. synchronization: deterministic simulation, input prediction, bounded rollback and Stage/Result lifecycle;
3. transport: WebRTC full mesh with WebSocket fallback and separate control/input semantics;
4. browser: signaling, ICE/TURN, reconnect and route state;
5. game adapter: the owners for GameManager, Enemy, Bullet, Item, Effect, ANM, ECL, Replay and input.

Use a frame-0 barrier, bounded direction prediction, confirmed frames, limited catch-up and a sparse/fixed-arena journal. Full-world snapshots were measured at roughly 19 MB each and are not a per-tick transport strategy. Relay forwards targeted envelopes; it does not simulate the game.

For the current TH06/TH07 production profile, there is one player-facing timing policy: zero added local input frames with full rollback on every endpoint. The generic netplay core may retain input-delay primitives for tests and future evidence-backed designs, but the shared Launcher does not expose a title-specific buffered/stability mode. Do not reintroduce the retired TH07 mobile/desktop asymmetric policy merely for backward compatibility.

Spectator is a start-only, read-only product path. It consumes confirmed history/checkpoints and cannot change seat, ready, pause, result, input, touch, THPrac or the simulation. Runtime exit must not accidentally destroy an active room that Launcher still owns.

<!-- knowledge-id: K-MP-002 -->
## Invariants and pitfalls

- One logical frame samples input once. Retransmit, retry and prediction never resample a device.
- Delivery, prediction, confirmation and rollback each have explicit owners.
- Restart creates a new generation/session identity; a visible canvas is not proof that transport is healthy.
- Reconnection waits are bounded; an ICE restart requires missing health or confirmation progress, not just a transient `disconnected` state.
- MP Replay uses the documented local-player offset and isolated MP storage; do not infer ownership from visual seat position.
- Keep rollback/presentation separate. Remote smoothing and alpha are draw-side; rollback changes logical state only.
- Do not restore the withdrawn `TH_ENABLE_MULTIPLAYER`, `MultiplayerRuntime.*`, `g_Player2` or old LCConnect path.
- Do not add or expand hash-based gating to hide a semantic mismatch. Compare the divergent owner/state and preserve existing correctness checks.
- Relay/TURN is shared service infrastructure. New configuration uses the
  `EAGLER_NETPLAY_*` namespace. Historical `TH07_*` relay/TURN variables are
  compatibility aliases only and must not be copied into a new title-specific
  namespace.
- A diagnostics/fallback room is product-neutral. Do not encode `th07mp` (or
  another title) merely to reach shared signaling/ICE health checks.

## Superseded approaches

The early TH06 LCConnect/`MultiplayerRuntime.*`/`g_Player2` route is withdrawn. Full-world per-tick snapshots and decorative rollback state are also superseded by bounded sparse ownership and logic-only rollback.

## Code and system anchors

- sibling `eagler-common/`: shared cross-title netplay/runtime authority.
  Generic session, transport, core, input ownership, journal/rollback storage,
  browser catch-up/time-sync pacing, confirmed-frontier liveness and reusable
  fault-injection primitives converge here. Title repositories retain small
  protocol/input/transport config seams, rollback-state inventories, canonical
  hashes, gameplay lifecycle and title acceptance fixtures. Do not build a
  universal title driver merely to eliminate a few lines of adapter glue.
- `src/contracts/product-catalog.mts`: optional Multiplayer
  declarations and per-product room/loadout bounds.
- `src/contracts/adapter-capabilities.mts`: profile-required
  Multiplayer obligations and legacy compatibility classification.
- `server/netplay-relay.mjs`: shared relay policy; derives room
  product policy from the declared product prefix and uses generic service
  configuration.
- `src/launcher/multiplayer-relay-url.mts` and
  `network-diagnostics.mts`: shared transport URL/diagnostics ownership.
- `tests/test-netplay-relay-product-policy.mjs` and
  `test-netplay-service-config.mjs`: product-policy and deployment-namespace
  gates.
- `tests/test-multiplayer-spectator-launcher-contract.mjs` and
  `tests/test-multiplayer-replay-launcher-contract.mjs`: Launcher product
  contracts.
- TH06MP/TH07MP title repositories: title-specific rollback inventories,
  gameplay semantics and transport acceptance evidence.

## Verification

Check in this order:

1. ordinary/MP build and storage identity are isolated;
2. protocol/version, frame-0 barrier, input ownership and prediction bounds;
3. 2P and 3P core, restart/death/Result, Replay and spectator backlog;
4. WebRTC, WebSocket fallback, Relay/TURN candidate and route state;
5. Launcher card/room/exit behavior;
6. browser-focused evidence, then clearly labeled human/device evidence.

Record first divergent logical frame and owner state. Do not substitute a screenshot, a room join or a source-only contract for end-to-end acceptance.

When the failure is performance rather than protocol/lifecycle, follow the
measurement order and exact-byte/canonical gates in `rollback.md`; do not infer
rollback cost from average FPS alone.

## Deliberately omitted claims

This playbook does not make multiplayer mandatory for a title and does not
promote focused protocol/browser tests to cross-device human acceptance.
