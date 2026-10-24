# TH11 Presentation Lab adapter

Status: **milestone 2 — native records, stage scenarios, stable identities and
negative control verified**. This is an isolated diagnostic profile, not a
production Runtime.

## What is covered

- Non-destructive freeze/resume of the rAF loop (no in-game pause).
- One complete fixed tick per `advanceOneTick` (update + authoritative draw).
- `drawOnly(alpha)` runs the real presentation path with no input, RNG, audio,
  Replay cursor or simulation counter advancement.
- Deterministic stage seeding through the production restart path. Open
  `http://127.0.0.1:8136/?stage=1` (optional `character`/`difficulty`) and the
  workbench enters a real stage after launch; no menu automation is involved.
- ANM submission records: owner scope (scene pass), VM identity, generation,
  in-frame occurrence, layer, script/sprite identity, authored fields,
  continuous-field mask, 16 submitted vertices and bounds. Quads, ASCII
  sprites, textured fans, ring geometry and ripples are sampled at the real
  submission boundary.
- Stable VM identity for embedded VMs that never enter `AnmManager`'s id
  space. `AnmVm` carries a reserved identity/generation pair; the audit also
  separates repeated submissions of one VM per frame (tiled stage backgrounds)
  with an occurrence index. Copies made by render-only interpolation retain
  both.
- Nine authoritative state digests: session/economy, player, bullets, items,
  lasers, enemies, ANM registry, cameras, RNG/Replay cursors.
- Negative control (`presentation_lab_fault`) perturbs an analyzer-visible
  authored field on alternating frames, so the analyzer must report
  `non-idempotent` samples.

## Verified evidence (2026-10-23)

Build: `TH11_PRESENTATION_LAB=1`, WASM
`42fc98c10a50dc6313df46211e8c7dbc1c448f11b36ad50fe8258846acdbcd7a`,
worktree `28ded7ec`, pinned `eagler-common` `5c4e96f2`.

- Boot with `?stage=1`: phase 1, 115 ANM records per reference frame.
- Two reference steps: tick 1→3, previous/current complete, 115 records each.
- Stage-1 sweep: `purity=unknown` (intentional missing groups), 115 objects,
  0 issue groups, `valid=true`, consecutive ticks.
- Negative control: 70 non-idempotent objects; the analyzer is not a
  no-op.
- Bounded scan 60 ticks / stride 6: 48 `interpolated`, 21 `responsive`,
  7 `held-review`, 10 windows, no purity stop.
- Longer scans can expose `endpoint-mismatch-previous` on looping enemy/effect
  layers (ANM layers 6/7). Those windows are real findings: the presentation
  sidecar snaps at a timer rewind rather than blending a stale loop endpoint.
  They are not waived; `purity` stays `unknown`.

## Operational note

The shared runtime shell can log a non-fatal `Startup animation draw failed`
message during headless launch; the runtime still reaches phase 1 and the lab
proceeds. It is a shell startup-branding path, not a Lab tick/draw failure.

## Explicitly unknown / not covered

- Non-ANM screen rectangles (`solid_rectangle`), HUD text lifetime, title/ending
  embedded VMs, audio queues, storage writes, the timing ring.
- Therefore a clean sweep is reported as purity `unknown`, never a pass.
- No Replay import or campaign runner yet.
- `submitted-position`/`submitted-shape` are derived submission values and are
  marked `unknown`: retail half-pixel alignment and per-pass pixel handling make
  them unsuitable as continuous authored fields. Authored `position`,
  `script-position`, `child-position`, rotation, scale, colors and UV remain
  classified.

## Commands

```powershell
# build the isolated diagnostic WASM + open the workbench
./portable/presentation-lab/start-lab.ps1 -Build

# serve an existing build
./portable/presentation-lab/start-lab.ps1

# one-shot headless verification, writes logs/lab/report.json
node ../local/test-th11-lab.mjs
```

Default URL: `http://127.0.0.1:8136/`; add `?stage=1` for a deterministic
gameplay window.

Environment:
- `TH11_LAB_FONT` — local MS Gothic TTC (default `C:/Windows/Fonts/msgothic.ttc`).
- `TH11_LAB_DATA` — clean `th11.dat` (default `<workspace>/game-data-th11/th11.dat`).
- `TH11_PRESENTATION_LAB=1` or `--presentation-lab` — build switch.

## Source gates

```text
node portable/check-high-refresh-contract.mjs
node portable/check-presentation-purity.mjs
node portable/check-presentation.mjs
node portable/check-presentation-lab.mjs
node --test third_party/eagler-common/testkit/presentation-lab/controller-core.test.mjs third_party/eagler-common/testkit/presentation-lab/release-contract.test.mjs
```
