# TH08 Adonis experiment

Date: 2026-10-02. Branch `experiment/adonis`, MP base `c23c67d`.
Worktree: `D:/workspace/eagler/worktrees/adonis/th08`.
Common dependency: `5669eff91e88a4391e144652836bcb0804b1482b`.
No ordinary/eagler tree changes, push or deployment.

## Implemented policy

`netplayAdonisMode` selects 0 (existing baseline), 1 (pure fixed-delay actual-input
lockstep) or 2 (fixed delay plus the existing complete rollback owner).
Experimental `netplayInputDelay` is fixed for the run in 0..9; baseline retains
its existing 0..8 envelope and prediction-budget behavior. Defaults remain 0.
The Launcher experiment exposes this only for matching TH08/TH09 MP Runtimes.

Physical capture N is stored once for execution N+D. Bootstrap inputs are
neutral, retries reuse the same stored sample, and later capture attempts cannot
advance the physical input frontier while the current simulation is stalled.
Direct-touch deltas and one-shot actions use the existing input representation.
There is no dynamic D change, blocking busy-wait, logical-dt change or gameplay
reduction. D=0 still waits for remote actual input in pure mode.

Pure mode sets common `allowPrediction=false`, rejects forged predicted commits
and correction/world-ready requests, and bypasses the actual native world and
texture rollback binding/capture path. Confirmed audio, file writes, Replay and
spectator publication remain enabled; these are not speculative world snapshots.
Hybrid retains the existing world capture, restore and resimulation owners.

The common SessionChannel supplies Adonis due/first-arrival statistics, advisory
messages and bounded phase corrections. Experimental input resend is 16 ms.
The outer SDL scheduler consumes correction as wall-clock delay, retains any
unspent debt across short display callbacks, and leaves pending advice untouched
when a tick/retry is already due. It does not stack the old proportional pacer
or change the fixed 60 Hz deterministic simulation.

## Contracts and ownership

* Native setup v5 has 20 words: the existing v4 fields plus nonzero mode 1/2.
  Legacy v2/v3/v4 decoding remains. Invalid mode/D/length is rejected atomically.
  HELLO ABI includes the new mode and D, preventing mixed-policy peers.
* Netplay status v3 extends the old prefix with mode at word 14. Existing status
  consumers reading 12/14-word prefixes continue to work.
* Replay metadata v4 adds mode to the existing timing description (104 bytes).
  Legacy metadata v2/v3 remains readable. Playback and spectators consume
  already-applied confirmed inputs and never queue them for D a second time.
* Restart generation preserves the declared timing policy, resets capture/phase
  state and rejects stale-generation input. Live changes remain forbidden.

Main source owners: `SessionSetup`, `NetplayRuntime`, `NetworkConnection`,
`RollbackDriver`, `ReplayArchive`, `ReplayRuntime`, `BrowserExports`,
`sdl/GameHost.cpp` and `sdl-runtime/multiplayer-host.mjs`.
The branch's common submodule was initialized from local objects, not fetched.

## Passing component gates

```powershell
$env:WASI_SDK_PATH='D:/workspace/eagler/toolchains/wasi-sdk-34.0-x86_64-windows'
node portable/check-multiplayer-netplay.mjs
node portable/check-multiplayer-replay.mjs
node --test portable/multiplayer/host-options.test.mjs
git diff --check
```

The first two compile and execute actual title components as WASI tests. The
host-options suite passed 8/8. Added coverage includes pure D=0/1/9, both 2P/3P,
48 exact input frames per case, idempotent sample reuse, fresh touch/bomb edges,
no prediction or world-ready state, gap handling, incompatible mode/D HELLO,
atomic invalid setup rejection, generation restart and stale packet rejection.
Hybrid correction replays late inputs against a deterministic reference fold;
this component test is not a full-world equivalence proof on its own.
Replay archive cases cover every recorded seat in 2P/3P, baseline and both
experimental modes, export/load and confirmed playback without double delay.

## Actual Emscripten build

```powershell
$env:EMSDK='D:/workspace/eagler/th08-eagler/tools/emsdk'
$env:EM_FROZEN_CACHE='1'
$env:TH_BUILD_JOBS='1'
node portable/build.mjs --th08 --multiplayer
```

PASS, actual multiplayer variant, 223 translation units. WASM 3,563,917 bytes:
`766eb581d13840d2f6aa936be03e6bc286ed10b940f1f57efcd528cdbfb8e469`.
Output is `th08_web/artifacts/multiplayer/th08-sdl.mjs/.wasm` and `build.json`.
The first two-job build hit a Clang 24 frontend crash. A one-job incremental
retry with the same source completed; no claim is made that the compiler crash
was fixed. Retained log: `th08_web/artifacts/multiplayer/adonis-build-retry.log`.
The existing local SDK/cache was reused with downloads disabled.

## Actual browser game gates

```powershell
$env:EAGLER_WORKSPACE='D:/workspace/eagler'
$env:EAGLER_LAUNCHER_ROOT='D:/workspace/eagler/worktrees/adonis/eagler-touhou'
$env:TH08_MP_DATA='D:/workspace/eagler/th08-eagler/artifacts/presentation-lab/input/th08.dat'
node portable/multiplayer/check-network.mjs --only=host --players=3 --adonis-mode=1 --input-delay=9
node portable/multiplayer/check-network.mjs --only=host --players=2 --adonis-mode=2 --input-delay=2
```

Both final commands PASS against the real compiled C++/WASM, retail local DATA,
isolated browser contexts, managed runtime shell and host-authoritative room
admission. Actual player transport reported RTC, not forced relay. The room's
accepted mode/D/prediction fields are asserted and become every peer's options;
they are not independently injected after a mode-zero room start.

The harness advances through frame 349, checks equal canonical state across all
peers at each requested checkpoint, and checks keyboard and touch movement begin
at exactly the declared D. It also verifies stalled callbacks do not build an
extra capture queue, local focus/fire/bomb controls, no P1 movement from a P2
drag, MP save-directory isolation, exit/sync/import restrictions, retained parent
lobby ownership and persistent storage after reopening the iframe.
Pure mode additionally asserts zero prediction/correction/resimulation, zero
world-ready flag and zero rollback maximum bytes at every checkpoint.

Retained final reports under `artifacts/multiplayer-tests/`:

| Case | Report directory | Result |
| --- | --- | --- |
| Pure D=9, 3P, authoritative room | `network-47a889ed-7545-48d6-8d74-de0e360525fe` | PASS |
| Hybrid D=2, 2P, authoritative room | `network-2d6cdaa6-b439-43ac-a62b-932eed77572e` | PASS |
| Pure D=9, 2P, earlier direct-option harness | `network-36ccf595-d46a-4de7-9fda-9680532d537e` | PASS |
| Pure D=0, 3P, earlier direct-option harness | `network-8053fc62-84f8-4380-9a91-4ffec059df1a` | PASS |
| Hybrid D=2, 3P, earlier direct-option harness | `network-ded2c164-70f5-4639-b12c-f214829c3939` | PASS |

Each contains `suite.json` and `host.json`, with build/harness identity. The
earlier three do NOT prove the newly extended room's authoritative timing;
the final two do. All are short desktop browser correctness gates, not phone,
WAN, whole-stage or quantitative performance acceptance.

Two failed setup attempts are also retained. `network-16bd70fc-...` had a missing
default DATA path; the existing local fixture path above resolved it.
`network-a5ed3fec-...` timed out before the shell's ready event because the test
server omitted `directory-keyboard.mjs`. The server mapping is fixed and the
host harness now records failed requests and HTTP errors as well as page errors.
Neither failure reached an Adonis gameplay session.

## Remaining evidence

No TH08 performance benefit is claimed from the short host tests. Run sequential
matched-input A/B cases before reporting CPU, frame-time or latency gains.
Add forced-relay fault injection, longer stage/boss/transition runs, actual
Replay viewer and live spectator integration for v5, and real remote phones.
Native archive/read-only tests do not replace those product-level gates.
Do not publish to the main site or mix this MP build into ordinary TH08.
