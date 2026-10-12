# TH15 multiplayer implementation evidence

Implementation and dependency topology: [README](README.md).
This is a local experiment; canonical branches have not been promoted.

## Production artifacts

Multiplayer WASM: `8341e5a2610929f5345a0a0343e9a60182e2d0d7d8954a83aa4ac230d540ba64`.
Ordinary THPrac WASM: `cac254973e61eda535b1db8be98036be332f53da2e9c43522c373e6b0650a256`.
Both release builds and closed packages pass. The ordinary module has no MP
exports. Neither production module exposes development probes.
Common netplay pin: `dd93c944ce2f2830e99be9c8dba7ff2ac645f548`.

## Passing gates

- Production C++ gameplay under Node/WASM: 3,600 retail ticks, four characters,
  independent local-seat selection and resources, damage pooling, ECL aim,
  item ownership, remote collisions, single team chapter rewards, next-stage
  resources and all-player game-over.
- Shared economy revision (2026-10-13): one score owner across seats; pooled
  score/Graze/point value/point-item and collection statistics; one real bullet
  credits three grazing pilots once and retains each pilot's cooldown.
  Remote shot score gains accumulate once. Bomb stock/fragments, Power and
  life loss remain individual. One chapter reward contributes one team life
  fragment; pooled fragments grant each pilot one Extend, preserving the
  eight-spare-life cap, Extra's five-fragment threshold and five-Extend limit.
  Capped pilots still contribute; all-capped fragments convert only to the
  recipient's Bomb fragment. Extends do not revive ghosts.
- Protocol: 2P/3P, manual D=0/2/9, challenge on/off, reordered holes,
  capture-once ownership, session generation, build/ABI disagreement,
  all-seat replay including recorded P3, no second replay delay, malformed data,
  and rejection of gameplay version 1 (the previous per-seat economy).
- Common startup: calibration, immutable timing choice, lost-control repair,
  disconnect handling, and TH08/10/11/15 spectator timing identity regression.
- Real browser, production shell and Japanese 1.00b DATA, shared relay:
  2P reached frame 237 with a start-admitted spectator;
  3P reached frame 235. Both held frame zero until peers arrived,
  stopped advancing when a producer stopped, resumed when it returned, saved
  valid recordings, rejected a mismatched replay executable, and played through
  the real offline menu to the hosted exit event. The 3P recording is from P3;
  replay score metadata uses the shared team score; seat identity stays individual.
- Launcher production build, TH15 room/loadout/timing/relay policy, catalog,
  runtime profiles/options, product surface and 43 shared multiplayer service
  tests. Ordinary THPrac typed-owner gameplay regression passes.
- Git diff whitespace checks with `cr-at-eol`, respecting the repository's
  byte-preserving `* -text` policy and existing mixed/CRLF delivery sources.

Reports, replays and screenshots are ignored under
`th15_web/artifacts/sdl-multiplayer`; retail DATA and measured font binaries
are private inputs, not source deliverables.

## Resolved failures and limits

The initial browser attempt timed out before frame zero. Physical sampling
now occurs only when the local frame has not already been captured; a blocked
tick only pumps the network. A subsequent 3P offline replay reached EOF without
another simulation or presentation frame, suppressing its hosted exit event.
The loop now reports terminal and error states even without a new frame.
The final browser run passes both previously failing paths.

This is Legacy co-op, without Pointdevice checkpoint retry, THPrac in MP,
ghost rescue, Power transfer, multiplied drops or teammate-opacity controls.
Full-stage/Extra clear, long result/ending spectator tails, internet/TURN,
physical touch/gamepad devices and human cross-device acceptance are not run.
Browser tests use local software rendering; they establish functionality,
not high-refresh performance or handset acceptance.

Integration is local commits on the title, launcher and common experiment
branches. Publish the common dependency before distributing the title pin.
