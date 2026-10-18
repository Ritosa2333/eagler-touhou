# TH08 general multiplayer rules audit

Base: `experiment/th08-multiplayer`, `c23c67d9e3d46f67dabbf4b8063de7d91930eb39`.
Work is confined to the new `experiment/rules-20261002` worktree. No push, merge,
deployment, asset changes, difficulty tiers or per-spell balance changes.

## Compatibility warning

The existing gameplay contract revision is advanced from 6/7 to 8/9
(untimed/timed session envelopes). Previous MP replays, live peers and
spectator streams are rejected because their input was authored under
different resource rules. Checksums and transport formats are unchanged.
Ordinary single-player replay handling and formats are unchanged.

## Current/requested/gap/decision matrix

| Rule | Upstream behavior | Requested/result | Decision status |
| --- | --- | --- | --- |
| Ordinary life bombs | Character's shot-profile starting bombs on initial life, ordinary respawn and team reset | One base bomb in MP, including fresh runs, ordinary respawns, retry reset and a Spirit's next-stage base life | Implemented from explicit every-life instruction; SP and practice overrides preserved |
| Ordinary drop quantity | Native drop routines, no multiplayer drop multiplier | Keep authored/native quantities | Already compliant; unchanged |
| Ordinary enemy HP | Native ECL HP; no boss multiplier for ordinary enemies | No HP buff | Already compliant; unchanged |
| Elimination reward | Native five Full Power drops plus MP-only nearest-survivor +1 life | Remove only the MP-only extra life; native Full Power drops remain | **HIGH RISK — inferred anti-farm policy**, directed for this candidate; requires playtesting |
| Donated revive resources | Debits donor one spare life; restores Spirit to active state with retained power/bombs | Independent fixed package, one playable recipient life | Exact resource package and playable-life interpretation awaiting approval |
| Revive safety | 120-tick invulnerability; 60 frames of whole-screen 768×896 rectangle clear | Local circle clear and safe re-entry | Radius awaiting approval; no radius invented |
| Boss effective HP | Boss damage ×0.75 (2P), ×2/3 (3P), fixed configured count | Downscale when a teammate is eliminated | Active viable count is proposed; approval pending |

## Native resources and representation

TH08's `lives` field counts spare lives. An active pilot with `lives == 0`
still has one playable life; an eliminated Spirit also retains zero spares.
Therefore a donated revival from Spirit to active with zero spares increases
playable team lives by one while debiting the donor one spare. Setting the
receiver to one spare would grant two playable lives.

Native ordinary death drops one large Power item (8 Power), five small Power
items (1 each), and a character-specific bomb item for characters 2/8/9 when
they had bombs remaining. Final death drops five Full Power items. These
quantities and native random item trajectories are untouched. Ordinary power
pickups remain +1/+8; multiplayer does not double their amounts.

The retained final-death Full Power drops are a remaining anti-farming/balance
concern, explicitly preserved rather than silently redefining native drops.
The MP-only elimination life gift was a separate resource faucet, not a
native drop. Its removal means elimination cannot refund the donor's cost.

The current Spirit entry retains its upstream shot-profile bomb stock while
the donated-rescue package awaits approval. The free next-stage return resets
only a former Spirit to one base bomb; survivors retain their current stock.

Fresh-session initialization and team reset share the production
`begin_base_life` helper. Run/stage statistics remain under their existing
owners. No external input, display state or spectator seat enters this rule.

## Boss HP and damage semantics

`EclVm::life`, `initial_life`, `remaining_life` and phase thresholds remain in
native ECL units. ECL opcode 131 initializes all three life fields; phase
thresholds and GUI segments refer to that representation. The boss HUD reads
`life / initial_life`. Neither current nor maximum HP is multiplied today.

The multiplayer damage path aggregates eligible pilots' damage and applies
the native cap of 70 damage per enemy per frame. Native Last Spell and damage
protection handling runs before the generic boss multiplier:

- 2P: multiply final boss damage by 0.75, truncating to integer
- 3P: multiply final boss damage by 2/3, truncating to integer
- Ordinary enemies skip this multiplier

Separately, 3P bomb-origin damage regions are multiplied by 2/3 inside
`PlayerShots::damage`, before aggregation. This applies to ordinary enemies as
well. Ordinary option-laser regions carry an origin marker and are excluded.
This existing bomb special is retained, not silently rebalanced.

Proposed dynamic option: count viable roster seats for the final generic boss
multiplier. Spirits are unavailable; ordinary dying/respawning pilots remain
viable; spectators never get roster seats. This changes remaining effective
HP without healing, dealing instant damage, rescaling max HP or rewriting
phase thresholds. Reviving a Spirit would restore the appropriate multiplier.
**HIGH RISK — proposed interpretation**, not literal HP resizing; approval
pending before adoption.

## Revival safety audit

The upstream Spirit revive uses 120 fixed ticks of invulnerability (2 seconds
at 60 Hz), plus `clear_frames = 60`. The latter emits a 768×896 rectangle each
frame and is effectively whole-screen protection, not a local clear.

Existing bomb-only cancellation circles use 32, 64, 96 and 128 pixel radii.
None is an existing revival radius. The rescue interaction's 20 pixel reach
is not a suitable inferred bullet-clear specification. Reusing a circle API
is safe implementation reuse; selecting its radius remains a balance choice.
The existing invulnerability duration can be retained without inventing one.

## Determinism and ownership

The candidate changes native fixed-tick owners. No new rollback state, live
input source, difficulty selector or per-spell table is introduced. Existing
WorldJournal inventory already captures pilot life state, personal resource
banks, roster availability and cooperation gesture state. A source audit of
that inventory is not a complete gameplay rollback test.

The existing policy processes donors in stable seat order, requires focus
release after a successful donation, never lets a zero-spare donor spend the
last playable life, and gives a Spirit priority over a live recipient. Added
production-policy tests cover simultaneous donors, range bounds, recipient
life caps and deterministic restore/re-execution at the 90-tick threshold.

## Verification evidence

Toolchains: official Emscripten 6.0.9 and WASI SDK 34, using the pinned
`eagler-common` gitlink `c239e13730eafc57f2cd309f77f449782bd05bbd`.

- New `portable/check-multiplayer-rules.mjs`: production `PlayerLife` compiled
  separately as SP and MP; 144 ordinary-death combinations and final-death
  quantities pass in each. MP fresh-session/reset production helper also
  tested for 2P/3P and life/power boundary values
- `check-multiplayer-cooperation.mjs`: existing tests plus simultaneous
  donors, zero-spare donor, life cap, 20-pixel boundary and snapshot/resimulation
  pass
- `check-multiplayer-resources.mjs`: existing personal resource tests pass
- `check-multiplayer-enemy-damage.mjs`: existing damage tests pass
- `check-multiplayer-netplay.mjs`: 2P/3P old-ABI live/spectator rejection and
  existing exact-input, correction, duplicate, bounded-history tests pass
- `check-multiplayer-replay.mjs`: 2P/3P, every recorded seat, both timing
  envelopes round-trip; valid old-ABI archives are rejected before mutation
- `check-multiplayer-live-bullets.mjs`: complete-byte Bullet restore test passes
- `node --test tests/multiplayer-build-isolation.test.mjs portable/multiplayer/build-isolation.test.mjs`:
  8/8 pass, including actual WASM export isolation for ordinary, production MP
  and diagnostic MP binaries
- README presentation analyzer, high-refresh contract and presentation-purity
  gates pass
- Full ordinary, production MP and diagnostic MP Emscripten builds pass with
  source-to-manifest validation
- `git diff --check` passes

### Source-matched build identities

| Variant | Bytes | WASM SHA-256 |
| --- | ---: | --- |
| Ordinary | 3,337,138 | `e1336e89387d2b4b0c8f28708e30f75116301e3351ac46ad18a63ebc40c1d64b` |
| Production MP | 3,557,052 | `dcc61ff707c81f4edb04a29f06f5606bd1f9634ec730671ca5c8bfc7010b1513` |
| Diagnostic MP | 3,744,391 | `2179e6babf442a370d28ebb130fb21d9d9b44af05acb05fbcede834d9e4b9b72` |

The ordinary binary is identical across the initial and final candidate builds;
this is not a claim of comparison with a separately built upstream baseline.
Generated binaries/manifests and final gate output are under the worktree's
ignored `th08_web/artifacts/` and `artifacts/multiplayer-tests/` directories.
The focused reproducible command is:

```sh
WASI_SDK_PATH=/path/to/wasi-sdk node portable/check-multiplayer-rules.mjs
```

For full builds, set `EMSDK` to the official installed toolchain and run
`node portable/build.mjs --th08`, then add `--multiplayer` or
`--multiplayer-fixtures` for the other two isolated variants.

Initial test-harness failures (corrected): the death test omitted advancing
the respawn animation timer, and the old-replay fixture attempted to encode
an empty tape. Neither was a game regression; the corrected tests execute the
production code and valid nonempty replay envelopes. A concurrent scope
correction invalidated one intermediate MP build, which the existing source
identity guard rejected; the clean final rebuild passed. The initial binary
isolation attempt lacked the diagnostic build; after producing that variant,
all eight checks passed. Those intermediate logs remain separate from final
results.

## Not established

No full real-stage browser/gameplay, human balance, cross-device, asset-based
replay-oracle or whole-title rollback run is claimed. Retail assets and private
logs were not inspected or copied. Existing WASI transport/replay tests prove
component contracts, not end-to-end gameplay or visual acceptance.


## Completion boundary

This is a locally verified **partial candidate**, not completion of the entire
requested rule set. It implements the unambiguous every-life Bomb1 rule,
preserves native drops/ordinary HP, removes the directed MP-only elimination
life gift, and fences changed MP semantics. Donation package, exact local
clear radius and dynamic boss damage-count policy remain explicitly pending.
The current paid revive still uses upstream profile bomb stock, zero Power
after a normal final death, 120-tick invulnerability and the old wide clear.
Those are known requested gaps, not endorsed final balance.
