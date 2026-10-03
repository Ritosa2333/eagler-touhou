# TH08MP optimization handoff — stopped at user's request

Historical stop: user paused on 2026-09-26, then explicitly resumed on 2026-09-27.
See `mp-smoothness-measurement-2026-09-27.md` for the resumed base-Update work,
current fixture identity, accepted/rejected experiments and remaining limits.
The source/deployment descriptions below record the earlier handoff; they are
not a claim that the newer source is unbuilt. No new deployment was made.

## User context and scope

- Stage 1 clear froze with `captured input send failed`; dense bullets lag on phone.
- Phone supplied by user: RMX5080, Android WebView approximately 150. No physical-device performance run was done here. Do not infer its SoC or equate desktop CPU throttling with this phone.
- TH06MP/TH07MP are stable controls. Do not modify them casually.
- User specifically requires reuse of `eagler-touhou/docs/playbooks/rollback.md`, including its measurement methodology.
- Launcher has parallel UI/content/contracts edits. Preserve them; previous deployment deliberately excluded them.
- Active task worktree is `D:/workspace/eagler/worktrees/th08-multiplayer`; canonical `th08-eagler` supplies test DATA/fonts. Read WORKSPACE, shared ROUTES and rollback primary playbook before continuing.
- No commits were made in this optimization task. All source changes below remain in the worktree. Preexisting untracked `build-eagler-multiplayer/` was preserved.

## Production: completed and independently verified

The previously validated StageClear capture-boundary fix plus frontier snapshots was deployed on explicit request, before the later experimental work.

- URL: https://touhou.vip/?game=th08mp — TH08MP remains publicly available.
- Generation: `eef24cb58085ba70796f9ea4aa5801ec198d6aca59763ce3a416ecd14a006090`.
- WASM SHA256: `30c5e0ecafc920972958348dc70c900b7b7db5dc15a6f5eefec8bf1283e2e930`.
- Production host SSH alias `129`; site `/var/www/eagler-touhou/current`.
- Rollback directory `/var/www/eagler-touhou/rollback-20260926-194226`.
- Excludes early-send, live Bullet snapshots, checkpoint intervals and all fixture exports.
- Verified 59 public file hashes including changed files and launcher/06/07/10 controls, response headers and desktop/mobile/English entry behavior.
- Deployment evidence: `artifacts/main-stage-clear-release-20260926/`; explanatory record: `docs/mp-stage-boundary-deployment-2026-09-26.md`.
- `th08_web/artifacts/multiplayer/` still holds this production build. Do not overwrite it by rebuilding dirty source and mistake the result for the deployed version.

## Validated local improvements before this pause

1. Capture-boundary fix: after a correction shortens StageClear, reuse already captured inputs without moving the channel frontier backwards. Actual network errors/missing captured inputs still fail.
2. Frontier-only checkpoints skip confirmed-prefix snapshots while retaining all native Update/semantic Draw/audio/file/Replay behavior.
3. Send a once-only captured input before expensive reconciliation, restricted to live stable worlds; preserve loading/generation fences.
4. Live Bullet storage directly uses the existing shared `PartitionedPoolJournal` and batched bulk copies. Covers five complete ANM VM partitions plus complete tail for 1537 slots, including sentinel. Birth/reuse/clear/extra-opcode writers have hooks. No canonical fields or gameplay counts were removed.

Latest successfully built/tested fixture WASM **before unfinished interval changes**:
`39ebe48179db69af0372de78698513713e4a77e17bf93330cd4a44475264f2d5`.
The current source is newer than this binary. Do not present runs against this binary as validating current interval source.

Evidence in `artifacts/multiplayer-tests/`:

- `live-bullets-native.json`: five native writer cases pass whole-pool byte restoration.
- `live-bullets-mixed-v2.json`: 2P, 900 frames, 1000 starting bullets, delayed analog input, ordinary death, asymmetric visual settings/extra Present; cross-backend canonical agreement and 349/348 whole-byte undo audits.
- `live-bullets-mixed-3p.json`: reverse mixed backend, 3P, 600 frames, 203 audits each; live snapshot bytes about 21.4% lower.
- `live-bullets-world-loadouts.json`: 20 focused/unfocused Bomb loadout cases plus 40 later probes; complete byte and world hashes pass.
- `live-bullets-stage-3p.json`: real RTC StageClear correction reaches next stage.
- `early-input-network-v2.json`: RTC 2P/3P plus generation/restart fences. Earlier false failure came from test frame-zero route bootstrap and is retained in the older report.
- WASI runners: `portable/check-multiplayer-capture-boundary.mjs`, `portable/check-multiplayer-live-bullets.mjs`.

Native Bullet fixture strengthening still useful: mode 0 clears then emits, but does not explicitly force reuse of the original first slot. Set `next_slot` to that slot before journal Bind and assert real reuse; assert extra 0x4000 changes dormant VM templates. Do not claim current native test proves that exact same-slot scenario. The lower-level partition unit test does cover cold overwrite/reuse bytes.

## Performance evidence: optimization not yet accepted as smooth

Full methodology/results are in `docs/mp-smoothness-measurement-2026-09-26.md`.
Harness: `portable/multiplayer/measure-smoothness.py` and `performance-hook.mjs`.
Uses separate Chromium processes, native rAF, real RTC application-send impairment (~77 ms RTT / 10 ms jitter target), actual hardware GPU with `--browser-channel chromium`, P2 CPU throttle 4x, warmup, frozen inputs/build hashes, alternating order, canonical gates. Wall-clock touch uses TH07's sinusoidal stimulus through TH08 native touch APIs. No physical phone test.

Latest live versus legacy Bullet backend, both frontier+early-send, 1000 bullets:

| Round | Backend | P2 logic FPS | Present p99 ms | gaps >50 ms | capture ms/checkpoint |
|---|---|---:|---:|---:|---:|
| 1 | legacy | 45.39 | 56.5 | 7 | 5.35 |
| 1 | live | 46.53 | 52.4 | 5 | 3.94 |
| 2 | live | 36.41 | 65.3 | 9 | 4.85 |
| 2 | legacy | 40.95 | 71.9 | 19 | 5.68 |

All canonical gates pass; capture and tail latency improve, but FPS is inconsistent and below 60. Round 2 live has higher ordinary update cost; cause unresolved. Do not cherry-pick round 1 or claim mobile lag is fixed. Evidence: `smoothness-live-bullets/`.
Early send directionally improved tails/replay count in `smoothness-early-input/`, also below 60.
Existing exact-only modes are **unbuffered lockstep**, naturally around 12–14 FPS at this latency; they are not valid proof of base scene capacity. Need a buffered diagnostic or correctly labelled prerecorded workload control to separate forward game/render from snapshot/resimulation tax.

## Unfinished work added immediately before user stopped

**UNBUILT, UNTESTED interval adapter edits remain in source. Do not deploy them.**
Default `checkpoint_span` has been left at **1**. Three-frame storage is experimental and must not be enabled by default without proof.

Edited owners:

- WorldJournal: `{frame,end}` records, `CanExtend`, `CheckpointStart`, `FirstCheckpoint`, `BeginFrame(frame,extend)`. Extending skips fixed inventory and deep Replay copy, rechecks screen chain capture; retains records by interval end.
- EnemyJournal: interval end, keep first owner/existence snapshots across extension. Shared generic journal opens with `extend=true`.
- PoolsJournal: extends generic/live journals; skips fixed/live initial capture on extension; writer hooks must protect new/reused/cold state. Whole Bullet audit records retain interval end.
- ResourcesJournal: stable owner check, extend shared journal without recapture.
- TextureJournal: retain interval end and first before-images, extend without another backbuffer snapshot; discard by end.
- NetplayRuntime: `BeginCorrection(first,checkpointSpan=1)` permits at most two extra prefix ticks for span 3 while preserving the original eight-frame prediction limit. Uses existing shared core rewind.
- ReplayArchive: optional exclusive commit limit.
- RollbackDriver: restore containing checkpoint start; defer irreversible audio/file/Replay outputs until containing interval is retired; count fresh checkpoints and incremental bytes; production default remains 1.

Only basic `git diff --check` is intended at stop time; no compilation or correctness claim for these changes. Fixture API to set span, A/B wiring and dedicated interval tests are NOT implemented yet. Shared libraries were not edited.

## Resume order when user asks

1. Inspect status/diffs, preserve other work. Read rollback playbook checkpoint-span section and TH07 `src/netplay/Th07RollbackState.cpp` interval implementation (about lines 564–695). TH07 correction caller restores actual checkpoint start.
2. Review unfinished interval code for owner correctness. Especially Enemy async-owner pinning, Effect vertex allocation/capture hooks, screen-chain nodes, texture before-images, confirmation/interval retirement, audio/file append and commit cursors, Replay stamps and lifecycle shortening. Output queues have capacity 16; prediction stays 8, worst replay prefix adds 2.
3. Add meaningful tests: exact-prefix rewind with bound unchanged and no physical resampling; all native owner hashes/whole Bullet bytes at spans 1/2/3; discard within an interval; delayed corrections spanning partially confirmed intervals; no duplicate external effects or Replay output; StageClear shortening and generation changes; GPU texture restore.
4. Add fixture-only span setter and resolved-policy reporting, comparing span 1/3 in same frozen build and alternating order. Extend native world loadout probe to six ticks grouped into two intervals and restore their actual starts.
5. Rebuild fixture, restart smoke server (identity is cached at server startup), run correctness before performance. Do not compile concurrently with measurements.
6. Establish valid base/no-snapshot versus snapshot-only workload control. Measure forward Update/Draw, capture, restore, replay multiplicity, p99/gaps, receive age, allocations/copy counters. Test real authored dense stage/Bomb workload beyond stationary Bullet fixture.
7. Only after correctness and consistent useful performance gains decide production policy. Physical RMX5080/WebView acceptance remains separate. Seek new deployment instruction for later experimental bundle; earlier request was specifically to deploy the already-fixed08 artifact.

## Commands and retained environment

Workdir for commands: `D:/workspace/eagler/worktrees/th08-multiplayer`.

```powershell
$env:EMSDK='D:/workspace/eagler/toolchains/emsdk'
node portable/build.mjs --th08 --multiplayer-fixtures
node portable/check-multiplayer-netplay.mjs
node portable/check-multiplayer-capture-boundary.mjs
node portable/check-multiplayer-live-bullets.mjs
```

Fixture server (restart after every build):

```powershell
$env:TH08_MP_FIXTURES='1'
$env:TH08_MP_DATA='D:/workspace/eagler/th08-eagler/artifacts/presentation-lab/input/th08.dat'
node portable/multiplayer/serve.mjs
```

Port 8140; optional relay in launcher repo `node server/netplay-relay.mjs` with host127.0.0.1/port8141 and empty STUN URLs. Previously owned servers were stopped before this last continuation; no new server/build/benchmark was launched during interval editing.

Performance harness starts and cleans up its own servers:

```powershell
python portable/multiplayer/measure-smoothness.py --data D:/workspace/eagler/th08-eagler/artifacts/presentation-lab/input/th08.dat --output artifacts/multiplayer-tests/NEW-UNIQUE-RUN --frames 600 --cpu-rate 4 --modes frontier-legacy,frontier --rounds 2 --timeout 120 --browser-channel chromium --wall-clock-touch
```

WASI SDK auto-path: `D:/workspace/eagler/toolchains/wasi-sdk-34.0-x86_64-windows`.
Failures are retained as evidence. An initial `live-bullets-mixed.json` failed due to stale server Runtime identity; corrected rerun is `-v2`. Never erase inconvenient performance/correctness reports.
