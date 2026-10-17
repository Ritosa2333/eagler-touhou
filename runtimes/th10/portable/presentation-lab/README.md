# TH10 Presentation Lab adapter

This directory contains only TH10 runtime and observation semantics. The
controller, normalized analyzer, report format and eventual UI/server are
owned by the pinned `third_party/eagler-common` revision.

Current implementation milestone: isolated diagnostic build/package profile,
non-destructive freeze/resume, complete fixed-tick driver, original reference
frames and normalized ANM submission records. The owner registry, draw-only
complete state-purity evidence, inactive untextured mesh paths, non-ANM screen
rectangles, projected model pixels and
automatic Replay navigation are explicitly incomplete and must not be
reported as a finished title adaptation.  The ANM observer covers regular
quads, prebuilt quads, textured fans, projected strips and model-buffer
submissions; model-buffer records remain in local coordinates.
State evidence currently detects changes in lifecycle, economy/input/RNG,
authored ANM fields, interpolation sidecars, Replay cursors and host cadence.
Because gameplay actor pools, non-pool/embedded ANM VMs, audio internals and
non-ANM effects remain uncovered, a clean sweep is intentionally reported as
purity `unknown`.

The common workbench and TH10 loopback server are wired, but require a fresh
diagnostic build/package before launch.  `start-lab.ps1 -Build` performs that
isolated build; it never opens a browser window.

Normal source gate:

```powershell
node portable/check-high-refresh-contract.mjs
node portable/check-presentation-purity.mjs
node --test third_party/eagler-common/testkit/presentation-lab/controller-core.test.mjs third_party/eagler-common/testkit/presentation-lab/release-contract.test.mjs
```
