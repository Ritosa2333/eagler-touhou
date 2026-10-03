# TH10 measured Adonis adaptation

Implemented locally on 2026-10-03 on the existing MP base `a064121`.
Setup v4 has 21 words, including mode, automatic/manual D, reserve and build
identity. Actual input-channel measurement uses shared ADS/2 or three-player
ADS/3 after native world construction, before simulation/capture frame zero.
Automatic B has no added frame; hybrid keeps D>=1 while saving at most two.
Manual D=0..9 and the original complete 12-frame rollback window are retained.

Pure mode uses exact all-seat input without native world snapshots or
resimulation. Hybrid reuses the original journal. New v4 input capture waits
for pending correction; absolute touch-target semantics remain unchanged.
The first Replay checkpoint is created when recording begins after startup
commit, before frame-zero activation. Replay metadata v5 carries the chosen
fixed policy/build; older formats remain readable and emit their legacy form.
EATM spectator timing precedes frame zero. Confirmed input is never delayed
again; shared upload/backpressure isolation and callback budgets are applied.

The SDL shell and Launcher expose the shared calibration/status contract.
TH10 MP defaults rollback off, with its switch left of the unchanged manual
0..8 dropdown. Guests read host settings, which lock when the run starts.

Use the sibling current common source explicitly:
`EAGLER_COMMON_ROOT=D:/workspace/eagler/worktrees/adonis/eagler-common`.
The uncommitted dependency is not pinned by the existing gitlink.
See `../../../eagler-touhou/docs/ADONIS-08-10-20261003.md` for current local
Release/component/browser evidence and retained failures, and the shared
`../../../eagler-touhou/docs/playbooks/adonis-adaptation.md` for required gates.
No commit, push, upload or deployment is part of this adaptation request.

## Historical investigation, before this adaptation

Branch: `experiment/adonis`, MP base `a064121`.
Worktree: `D:/workspace/eagler/worktrees/adonis/th10`.
The only code/dependency change is a deliberate local common gitlink update
to `5669eff`; no TH10 Adonis gameplay integration was completed in this pass.
Do not expose an Adonis selector for this Runtime yet.

The delegated investigation mapped SessionSetup/NetplayRuntime, the native
Application/World rollback paths, SDL ApplicationHost scheduling, ReplayArchive
and shell options, but stopped before a title patch. Resume from actual files,
not a claim that the common dependency makes TH10 functional.

Use the sibling TH08 and TH09 experimental integrations as references. Required
modes are baseline unchanged, pure fixed D exact-input lockstep with no world
history/prediction/rewind, and fixed D plus full rollback; experimental D=0..9.
Bind mode/D into HELLO, preserve once-only physical input and fresh touch/action
edges, confirm Replay/spectator without double delay, reset generations, and
use bounded wall-clock phase corrections without altering 60 Hz or stacking
controllers. Add title behavior and actual browser gates before enabling it.

Only edit this MP experiment tree and only directly modify sources with
`apply_patch`. No reset/clean, broad fetch, online installs, push or deployment.
No TH10-specific test or performance improvement is claimed.
