# Testing and Acceptance Playbook

Status: active

## Purpose

Choose the smallest meaningful regression gate and report exactly what it proves. Keep source/contract, browser automation, real-device and public-network evidence from collapsing into one generic PASS.

## Applicability and authority

- The target feature's playbook defines its semantic invariant; this document defines evidence level and acceptance boundaries.
- Current reproducible tests govern browser/runtime behavior. TH08 game semantics still follow its portable/source contract.
- Human/device claims require explicit named platform and action. An automated smoke cannot be upgraded to a human PASS.

<!-- knowledge-id: K-TEST-001 -->
## Normal design

Start with the feature playbook's semantic invariant, then choose the smallest gate that can falsify the change. Promote a result to a stronger evidence level only when the corresponding browser, device or public-network action was actually performed.

## Invariants

- Every claim names its lane, platform/origin, manual or automated method, fixture/test and evidence level.
- A lower evidence level may support a higher-level test, but never substitutes for it.
- A failure is reported at the earliest divergent owner/frame when that information is available; a final screenshot is not the diagnostic authority.
- Existing dirty work and unrelated services remain outside the test's mutation scope.

## Evidence levels

Use explicit evidence dimensions rather than one undifferentiated PASS. The
table below is the distributed project's coarse evidence grouping.

| Level | Proves | Does not prove |
| --- | --- | --- |
| Contract/source | bytes, schema, owner, build, static or focused unit behavior | browser, device, network or human experience |
| Browser automation | page lifecycle, UI/API path and captured runtime errors on the named browser | all devices, long-duration play or public deployment |
| Real device / human | the named device/browser and manual path | other devices, other origins or universal stability |
| Public network | actual origin, CDN, Relay/TURN or package route | gameplay semantics, human feel or other deployments |

Write the level beside each result. `runtimeReady`, a contract test, a screenshot or a successful room join is not a universal feature PASS.

## Minimum gates by domain

- Scheduler/interpolation: title → Stage 1, logical 60 Hz, OGG/MIDI, storage, direct-entry Replay and no runtime/page errors.
- Replay/determinism: fixed-width format/length checks, first divergent logical tick, input ownership, save/load and fresh reload; menu unlock state is not Replay proof.
- Audio: OGG stream/full, SFX, MIDI message/timer/output path, AudioContext state and real-device audio when claimed.
- Touch/mobile: focus/blur, pause/restart, portrait/landscape, menus/dialogue/StageClear/fullscreen and manual touch on the named device.
- Multiplayer: ordinary/MP isolation, 2P/3P, frame-0, prediction/rollback, restart/Result, Replay/spectator, WebRTC/WS/TURN and then human cross-device play.
- Runtime/package/deploy: clean build, manifest/MIME, import/update/reload, storage migration, empty-host trial and separate public route checks.

<!-- knowledge-id: K-TEST-002 -->
## Failure discipline

When a result differs, capture the earliest divergence: logical frame, input owner, RNG/ECL order, stage, Replay offset, network confirmation frame or draw-only state. Do not debug from the last screenshot. Check whether the observed route was later superseded or rolled back before writing a current rule.

Keep direct-entry Replay separate from menu smoke. TH08's Extra/Spell Practice unlock assumptions can block a menu route without proving Replay corruption. Keep `none`, MIDI and OGG mode selection separate; a stale persisted config or pthread `Module` copy can create a silent false positive.

<!-- knowledge-id: K-TEST-003 -->
## Structured bring-up diagnostics

For a browser-runtime freeze, black frame, disappearance or unexpected exit, collect the structured report first: guest call stack, recent API-call ring, unimplemented stubs, page faults, thread state and a bounded log snapshot. Use pixel output as supporting evidence, not as the sole diagnosis. If a harness owns the browser session, do not attach a second CDP/browser client that can invalidate the run.

## Superseded approaches

Screenshots, `runtimeReady`, a room join, a static contract or an automated smoke alone are superseded as universal PASS criteria. They remain useful evidence at their own level.

## Verification report template

For each claim record:

```text
claim:
lane/build:
browser/device/origin:
manual or automated:
test/fixture:
result:
evidence level:
not proven:
```

Run one browser/page at a time for automated lanes and close it after the test.
Preserve unrelated working-tree changes and run `git diff --check` for
documentation/source changes.

## Deliberately omitted claims

This playbook does not turn automated results into human/device/public PASS.
