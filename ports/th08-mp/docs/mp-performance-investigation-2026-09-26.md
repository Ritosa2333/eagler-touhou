# TH08MP / TH10MP stutter investigation — 2026-09-26

User report: TH08MP stutters throughout a remote session at approximately
77 ms RTT / 10 ms jitter; TH10MP stutters near startup and its HUD sometimes
jumps from 120 to about 300 FPS. TH06MP/TH07MP do not exhibit the symptom.
The user explicitly confirmed TH08 resource tracing was **off**.

## Confirmed findings and local changes

### TH08: synchronous framebuffer readback on every rollback checkpoint

`TextureJournal::BeginFrame` eagerly protects the 640×480 backbuffer by calling
`Touch(back, true)`. Previously this called `graphics_device().read`, which
reached `glReadPixels`, converted every pixel to CPU format, and copied the
result into the checkpoint. This happened on forward and resimulated frames.
Disabling resource tracing did not disable this path.

The candidate retains the backbuffer before-image in reusable GPU textures.
It restores by exact nearest-neighbor color blit, marks the CPU copy stale,
and retains explicit readback for actual CPU consumers/screenshots. Other
textures retain their existing CPU byte snapshots. Confirmation, rollback
history, gameplay, authored Draw and input capture are unchanged. The title's
eight-frame history bounds simultaneously live backbuffer images; released
slots are reused and renderer destruction releases their GL objects.

Controlled 600-frame, two-world test, 3-frame late input, variable direct touch,
resource trace disabled, desktop Chromium with SwiftShader:

| Metric (each endpoint) | Baseline | Candidate |
| --- | ---: | ---: |
| readPixels calls (including startup) | 999 / 999 | 18 / 18 |
| time inside readPixels | 8163 / 7477 ms | 326 / 1428 ms |
| canonical divergence | none | none |

This isolates removal of readback work, **not** real-device smoothness. Tail
times did not establish an overall presentation improvement: the stopped-loop
test bursts GPU commands, and candidate testing overlapped build activity.
Do not describe these numbers as physical phone FPS or a full stutter fix.

### TH10: HUD exposes resimulation FPS

`Application::multiplayer_resimulate_draw` executes the authored Draw chain,
including `FrameStatistics::draw`, during corrections. That legacy counter
therefore includes repeated historical draws. `AppStatistics::draw_rate`
left-aligns longer strings by shifting X away from 590 when the number reaches
three digits. The high-refresh replacement searched for X=590 and consequently
missed exactly these inflated values.

The MP HUD now formats `Application::presentation_fps`, counted only after
actual Presents, directly at its text producer. Native timing/Replay accounting
and simulation remain unchanged. This repairs the display; it is not itself a
fix for the startup stutter.

### Remaining TH10 pacing work

TH06/TH07 call the common `FrameAdvantageWindow` / `FramePacingPolicy` machinery
to regulate relative endpoint progress. The current TH08/TH10 adapters do not
consume that recommendation in their SDL cadence. Their normal callbacks skip
expired deadlines and service at most one forward tick.

An 18-second real-rAF, manually delayed packet experiment (38.5 ±5 ms one-way,
two same-page Chromium worlds, SwiftShader) observed a persistent TH10 startup
lead of roughly 9–12 frames, with corrections averaging about 11 historical
frames on the leading endpoint. Callback CPU costs were only a few ms while
presentation gaps were much larger. Thus a stopped-loop, lockstep fixture
understates the rollback depth caused by startup skew. The shared event loop
and software GPU prevent attributing all visible gaps to the user's real
network or device. No pacing-policy change is included in this candidate.

## Verification and boundaries

- Exact GPU image restoration: PASS for 2P and 3P. Sixteen repeated two-level
  restores compared the entire backbuffer byte-for-byte; confirmation/reuse,
  CPU texture mutation, release/replacement and screenshot ownership checked.
- 900 frames, P2 ordinary death, four-frame bidirectional delay, variable touch,
  two extra presentation draws, different local visual settings: PASS; no
  canonical divergence. Endpoints performed 349/348 corrections and
  1395/1392 resimulated frames. Both ended at one spare life and Power 68.
- TH10 existing rollback browser regression: PASS after the FPS-only change.
- The older `check-correction.py` fails at its own session handshake (step 3).
  A fresh run calling its probe without either texture test produces the same
  failure. This probe is not counted as passing evidence.
- Changed-source whitespace checks: PASS.
- No real Windows/Android remote-session acceptance, hardware-GPU frame-time
  acceptance, or 120 Hz visual acceptance has been claimed.
- Changes remain in existing MP experiment worktrees; no commit, push,
  integration, upload or deployment was performed.

## Reproduction and artifacts

TH08 source root: `D:/workspace/eagler/worktrees/th08-multiplayer`.
TH10 source root: `D:/workspace/eagler/worktrees/th10-multiplayer`.

TH08 commands:

```powershell
node portable/build.mjs --th08 --multiplayer-fixtures
python portable/multiplayer/check-texture-history.py --url <fixture-origin> --output artifacts/multiplayer-tests/gpu-history.json
python portable/multiplayer/check-desync.py --url <fixture-origin> --frames 900 --delay 4 --ordinary-death 1 --presentations 2 --guest-config visual --analog --output artifacts/multiplayer-tests/gpu-history-death.json
node portable/build.mjs --th08 --multiplayer --resource-trace
```

Use the title's existing `serve.mjs` with the requested variant and local DATA.
The production profile preserves opt-in trace availability; it is not enabled
by this performance change.

Detailed local evidence lives in each worktree's `artifacts/multiplayer-tests/`:
TH08 `perf-before.json`, `perf-after.json`, `gpu-history.json`,
`gpu-history-death.json`, `perf-raf-after.json`; TH10 `perf-before.json`,
`perf-raf.json`, `fps-rollback.json`. The profiling scripts in that artifact
directory are investigation tools, not shipped runtime code.
