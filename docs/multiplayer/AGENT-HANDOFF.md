# TH09 rollback handoff for a new agent

> **2026-10-02 experiment/adonis override:** This worktree is now
> `D:/workspace/eagler/worktrees/adonis/th09`, branched from MP `5b9305c`.
> Read [ADONIS-EXPERIMENT.md](ADONIS-EXPERIMENT.md) first for the new timing
> modes, common pin, current builds, tests and limitations. The paths and pins
> below describe the historical rollback worktree, not this branch. No public
> push or deployment has been performed for the Adonis experiment.

Updated 2026-09-28. The user explicitly resumed optimization after `bf60dbc`:
inspect actual progress and continue toward very smooth gameplay. The resumed
work and frozen comparison identities are in `rollback-smoothness-work.md`.
Do not mistake the historical results below for validation of unbuilt changes.

The user requested another evidence-driven pass after `ef772e0`. A 7,200-frame
formal-release run reproduced severe match-complete menu stalls missing from
the shorter tests. The menu-boundary fix passed formal-release A/B/B/A, exact
world/Replay and UI lifecycle checks. See `rollback-menu-smoothness.md` and
`rollback-menu-evidence.json` for current evidence. Do not overwrite the older
frozen build directories or treat their short-run results as this pass's proof.

## First orientation

1. Workspace: `D:/workspace/eagler`. Read its `AGENTS.md` instructions and
   `WORKSPACE.md`. For implementation, debugging, or validation, read the
   relevant primary playbook **before source inspection**. For this work it is
   `D:/workspace/eagler/eagler-touhou/docs/playbooks/rollback.md`.
2. TH09 implementation and this handoff:
   `D:/workspace/eagler/worktrees/th09-multiplayer`. Branch
   `experiment/th09-multiplayer`. Original implementation was `cb3a9b7`, then
   the first handoff `bf60dbc`; resumed smoothness work is newer. Read the actual
   branch log/status and `rollback-menu-smoothness.md` before assuming a HEAD.
3. Canonical owner: `D:/workspace/eagler/th09-eagler`, branch `eagler`, HEAD
   `3b52630` when checked. It was clean and is **not** the optimization worktree.
   The experiment was created from this commit. Do not edit the upstream-tracking
   `worktrees/th09-upstream` as a Launcher Runtime owner.
4. The experiment pins `third_party/eagler-common` at `ac82fa2`. It supplies
   RollbackCore, RollbackJournal, SessionChannel and pacing/repair. The submodule
   was clean when checked. Do not silently substitute the workspace's other
   `eagler-common` checkout; it may lack SessionChannel.

All paths below are relative to the experiment worktree unless absolute.

## User decisions and boundaries

- Do **not** change the existing online DATA/OGG/resource routing or its 404
  behavior. No 404 fix was part of this work.
- The user currently prioritizes PC optimization. There is no special phone
  optimization request. Do not present desktop results as phone evidence.
- The user resumed profiling and optimization after this original handoff.
  The first measured bottleneck was snapshot copying, not collision math.
  See `rollback-menu-smoothness.md` for current source/build/test status;
  the original-game simulation and three-frame prediction policy are unchanged.
- `c`, `p`, `d` mean commit, push, deploy in the workspace instructions. The
  rollback work was committed locally; it was **not pushed or deployed** in its
  current form. Confirm new scope before publishing.
- Preserve unrelated working-tree changes. Check `git status --short` and the
  relevant diff before editing. Do not use reset/clean/restore to discard work.

## Current code and runtime status

The initial TH09 multiplayer implementation used two-player lockstep and a
six-frame input lead. The experiment replaces gameplay transport with two-peer
rollback using shared C++ primitives. Standalone and Launcher browser entries
use the same rollback session. The former lockstep interfaces remain for
compatibility; gameplay no longer sends its old lockstep input protocol.

`th09_web/cpp/multiplayer/RollbackSession.cpp` sets intentional local input
delay to **0**, direction-hold prediction to **3 missing frames**, and maximum
rollback history to **8 frames**. Shot/charge, focus and direction can be
predicted; Bomb and menu edges are not invented. Touch targets have a separate
policy; keyboard directions are discrete button bits. Eight frames are a
rollback *limit*, not eight frames of added input delay or a guarantee that a
device can afford continuous deep rollback. Missing input past the window can
stall simulation.

The driver in `th09_web/cpp/sdl/Application.cpp` captures physical input once
per logical frame; network input and ACK state stay outside game snapshots.
`WorldState` inventories rewindable game state over RollbackJournal. Intermediate
replay ticks retain Draw logic that writes game state, suppress GPU submission
and pure 2D sprite geometry, then present only the final state. Correction
starts at most eight historical ticks per callback and checks an 8 ms recovery
budget between ticks. The outer loop checks a 10 ms budget between ticks;
neither budget preempts an expensive individual tick. The resumed driver keeps
one already-due tick pending across recovery/network waits, without accumulating
stall debt, and preserves fractional phase when all due work finishes. The
ordinary no-wait cadence and input sampling policy are unchanged. Confirmed history owns
sound, music, Replay and spectator output. A gameplay hash is exchanged every
120 confirmed frames. Menu animation/navigation is rewindable; only the step
that can replace resources waits for exact reconciled input. Read-only menu
preflight and a fail-closed check before destruction protect that boundary.

The local packaged Runtime is `th09_web/build-eagler-multiplayer`. Current
release (`th09_web/artifacts/sdl-menu-release`) WASM SHA-256 is
`63b2d839e75a8dfc076b8cd5d3c27cab56eb9bd4ce3f1792e1717b5b703fc950`.
Current diagnostic (`th09_web/artifacts/sdl-menu-frontier`) WASM SHA-256 is
`a3d9f7bd769ce7233ccf96b852e7abb3ae11f47f022b2cf83be91c0f5d46d987`.
The prior `sdl-smooth-release` and `sdl-smooth-retry` directories remain frozen
historical controls. The packaged build ID is `63b2d839e75a8dfc076b8cd5`.
Default `artifacts/sdl3` and `artifacts/sdl-release` are retained older builds;
use explicit `PC_BUILD` / `TH09_OUTPUT` rather than accidentally testing them.
Generated build folders and raw test reports are ignored by Git; the evidence
JSON and Markdown under `docs/multiplayer` are committed.

The public `https://touhou.vip/runtime-manifest.json` was read on 2026-09-28.
Its `runtime/th09/multiplayer/` generation was
`0146755eb2a24aa86aa97f5af8a098e34ce02b8b4c068ad56953067339a51c4f`,
with WASM SHA-256
`868ab04843a65a3e3bbfa9ec7c0db9fe11ee36f9c22e6bf0e2a2f47d7d6aaffd`.
That is the **initial deployed rollback release**, not the later PC
optimizations. Recheck the manifest before making a future claim about what is
currently online.

Separately, upstream PR #3, "修复预警线问题", was merged on 2026-09-27 as
`b2878132c3f958cb82f49dec0901e8b5857f44de`. The experiment was branched
before that merge. Check integration explicitly before moving the experiment
into the canonical branch; do not assume this worktree includes the PR fix.

## Completed optimization work and measured evidence

Read the committed evidence in chronological order, but treat each report as
its own experiment. Do **not** add percentages across reports.

| Stage | Change | Controlled desktop evidence |
| --- | --- | --- |
| Initial migration | Sparse active Enemy/Bullet visual snapshots and first-write capture; full-state restore oracle | Dense vs sparse: capture + restore down 73.48%; capture + forward + restore down 58.92% in that fixture. See `rollback-evidence.md`. |
| PC storage | Retain owning snapshot buffers across ring reuse; copy the contiguous Bullet POD pool in one touch per field | Capture + restore + 8-tick resim over 200 blocks down 8.27%. See `rollback-pc-evidence.md`. |
| Draw | Skip pure 2D sprite geometry on unpresented historical ticks while keeping stateful Draw traversal | Same-WASM, same-checkpoint paired replay CPU down 6.60–12.11% across four runs; phase-accounted correction down 3.75–6.74%. See `rollback-draw-evidence.md`. |
| Owning pools | Sparse first-write Effect/Attack slots, fixed-slot deduplication, reuse same-type AttackState allocation, exact trivial-block copy | Hardware A/B/B/A: capture down 13.01%, restore down 22.78%, capture + restore + 8-tick resim down 11.11%. See `rollback-owning-evidence.md`. |
| Resumed smoothness | Sparse whole Bullet POD runs, actual per-VM first writes, preserve completed callback phase and retry one pending tick | Exact-input dual-process RTC A/B/B/A, 77 +/- 10 ms each direction and P2 2x CPU throttle: P2 submitted-RAF gap p99 33.33 -> 16.67 ms, >25 ms gaps 135 -> 17, maximum 66.67 -> 33.33 ms. See `rollback-smoothness-work.md` and its portable evidence JSON. |
| Long-run menu boundary | Keep menu animation/navigation in rollback; wait only before world resource replacement | Formal-release 7,200-frame A/B/B/A, 39 +/- 5 ms each direction, P2 2x slowdown: measured whole-interval logic 55.950 -> 59.958 Hz; 145-frame result menu 10.83 -> 2.45-2.47 seconds. All sixty confirmed points and wire tapes match. See `rollback-menu-evidence.json`. |

The earlier owning-pool 200-block A/B/B/A fixture used Intel UHD / ANGLE D3D11, same
inputs, full-state and pixel checks outside timers. Correction total was
371.910 ms before and 330.573 ms after. Its no-snapshot 320-tick control was
**7.86% slower** after the change; worst correction outliers also did not
consistently improve. Therefore this is evidence for the measured correction
workload, **not** an ordinary-frame speedup or guaranteed worst-case result.
Initial accounted checkpoint bytes fell from 2,741,824 to 2,452,192, but the
counter excludes subsequent first writes, allocations and metadata; it is not
total memory usage.

The earlier owning-pool correctness checks included sparse/dense state restore before and
after resimulation, exact final pixels, Effect/Attack clear/reuse/overflow,
all nine polymorphic AttackState types, round reset, 900-frame independent
two-peer plus exact-input reference, equal 1,312-byte Replay, RTC and WebSocket
relay with spectator and audio. The three 600-frame transport worlds reached
hash `1357358377`. Deliberate network outages still caused visible
presentation gaps. Release RTC passed periodic confirmed checks, but endpoints
ended on different frames, so there was no same-frame final-hash assertion.

The older live Draw A/B/B/A RTC p95 result was **mixed**. Do not extrapolate
its controlled CPU result. The resumed work separately verified improved
submitted-RAF tails and sustained per-frame four/eight-frame corrections through
1,800 frames with 383 peak active bullets and exact reference-world/Replay
equivalence. That still is not arbitrary-stage/maximum-density, physical phone,
public Internet/TURN, subjective dodging or acoustic acceptance. Deliberate
outages still visibly stall. See the new evidence for scope and outliers.

## Keyboard prediction result

`th09_web/tests/multiplayer/keyboard-prediction.*` sweeps fixed direction
prediction horizons against four synthetic discrete-keyboard traces. Extending
the horizon globally to six frames improves long holds but doubles errors on
short taps. `keyboard-adaptive.*` tests a candidate that extends after six
consecutive **received actual** same-direction inputs: it reduces total
mismatches on long holds and move/release, but increases wrong-direction and
release over-hold errors. These probes have no game-world or human trace.
Production remains at a three-frame direction horizon. Do not enable the
candidate based only on total mismatch counts; TH09's useful metric is whether
correcting a dangerous bullet, hit or death changes a player's dodging decision.

## Reproduction map

Set these in PowerShell from the experiment worktree:

```powershell
$env:TH09_EMSDK='D:/workspace/eagler/th08-eagler/tools/emsdk'
$env:WASI_SDK_BIN='D:/workspace/eagler/toolchains/wasi-sdk-34.0-x86_64-windows/bin'
$env:TH09_OUTPUT='artifacts/sdl-your-next-unique-build'
node th09_web/tests/multiplayer/run-tests.mjs
node th09_web/scripts/build-sdl.mjs
$env:PC_BUILD='th09_web/artifacts/sdl-your-next-unique-build'
node th09_web/tests/multiplayer/world-browser.mjs
node th09_web/tests/multiplayer/world-browser.mjs --dense
node th09_web/tests/multiplayer/world-browser.mjs --round-reset
node th09_web/tests/multiplayer/peer-browser.mjs
$env:TH09_PROFILE='0'
$env:TH09_OUTPUT='artifacts/sdl-your-next-release'
node th09_web/scripts/build-sdl.mjs --release
node th09_web/scripts/build-eagler.mjs --multiplayer
```

For real local transports, set `$env:NATIVE_GPU='1'` before running
`node th09_web/tests/multiplayer/transport-browser.mjs` and its `--relay` or
`--release` forms. The harness checks the renderer and rejects software GPU
fallback. `PC_BUILD` selects a frozen artifact directory; `RUN_LABEL` selects
the report name. Run performance comparisons serially with no compilation or
other heavy work in parallel. `node
th09_web/tests/multiplayer/export-menu-evidence.mjs` checks the current retained
reports, source hashes and packaged release. The older `export-smoothness` and
owning/Draw/PC stage-specific summarizers reference **historical binaries** and
are not current-build gates. For the current 7,200-frame formal-release recipe,
use `rollback-menu-smoothness.md`, not the old default artifact directories.

Local retail assets, fonts/music fixtures and Node dependencies were prepared
for the earlier tests. Verify they still exist if a new agent or host cannot
start the browser. Raw reports are in ignored
`th09_web/artifacts/multiplayer-tests/`; portable records are in
`docs/multiplayer/rollback-*-evidence.json`.

## Where to look next

The measured sustained result-menu slowdown is resolved. Actual remote-PC
control feel and acoustic/device acceptance remain open; do not keep changing
prediction/delay or guessing new compute hotspots merely because those human
checks cannot be automated. Reproduce a remaining symptom before more changes.
The formal release still showed isolated long submissions, which remain in the
evidence; no zero-stall or public Internet/TURN guarantee has been established.

Read `eagler-touhou/docs/playbooks/rollback.md` first. Then measure the
**remaining must-replay** cost in a dense real TH09 battle: Bullet update and collision,
Enemy ECL, target selection, stateful Draw, snapshot capture and restoration.
`AttackAreas` already has an active-pointer list and Enemy targeting already
shares the active-Enemy traversal; do not transplant another game's cache
without finding an actual TH09 bottleneck. Preserve RNG, ordering, lifecycle
and full-state/Replay equivalence. Extend the existing sustained repeated
four/eight-frame gates to stronger loads and check presentation tails, not only
one isolated restore every eight ticks. Compare dangerous visible corrections, hit/death changes, local response
and stall rate under identical network traces before changing input delay or
prediction policy.

Some older milestone Markdown (especially `rollback-migration.md`) says "no
commit" or describes only the initial snapshot layout. Those statements were
historically true at that stage. The current committed source and this handoff
supersede them for branch state; the stage-specific measurements remain valid
within their named binary comparisons.
