# TH15 multiplayer implementation evidence

Implementation and dependency topology: [README](README.md).
This is a local experiment; canonical branches have not been promoted.

## Production artifacts

Multiplayer WASM: `6ade33af16e8e6b43f0ce76572c0cf62e16a6044f4a36e71b7afc1d8c4c2a350`.
Ordinary THPrac WASM: `fd1add818b94c75aa0809aba64ea010f1fe134ac26d11c14354c353544ee5315`.
Both release builds and closed packages pass. The ordinary module has no MP
exports. Neither production module exposes development probes.
Common netplay pin: `dd93c944ce2f2830e99be9c8dba7ff2ac645f548`.

## Passing gates

- Production C++ gameplay under Node/WASM: 3,600 retail ticks, four characters,
  independent local-seat selection and resources, damage pooling, ECL aim,
  item ownership, remote collisions, single team chapter rewards, next-stage
  resources and all-player game-over.
- Shared statistics and independent fragments (2026-10-13): one score owner across seats; pooled
  score/Graze/point value/point-item and collection statistics; one real bullet
  credits three grazing pilots once and retains each pilot's cooldown.
  Remote shot score gains accumulate once. Bomb stock/fragments, Power and
  life loss remain individual. Life fragments and Extend tiers are also
  individual. One chapter score reward grants one fragment per eligible pilot,
  preserving the eight-spare-life cap, Extra's five-fragment threshold and
  five-Extend limit separately for each pilot. A full-life recipient converts
  its fragment to its own Bomb fragment even while teammates have spare room;
  full Bomb stock respects its own cap. Tests cover actual pickups in 2P/3P,
  mixed-stock chapter rewards, independent Extra tiers and next-stage retention
  of life/Bomb fragments and Extend progress. Chapter fragments skip ghosts
  and exhausted pilots awaiting their ghost transition.
- Independent Power and HUD (2026-10-13): real small/large P and Full Power
  pickups select an eligible underpowered collector before a full teammate.
  A farther pilot above its own POC line can attract an item which the nearest
  pilot cannot collect. Homing ownership changes after a ghost transition.
  Normal P converts on spawn and in the field only when all non-exhausted,
  non-ghost pilots are full, including temporary Miss/respawn states; the last
  Power pickup converts remaining field P and its appearance in the same tick.
  ECL Power reads the minimum of that team. Full Power fills one pilot or
  awards the shared full-Power point-value bonus once, respecting its cap.
  Cancel point-value items never use Power priority. Converted P scores once
  using the shared point value and native recipient height calculation.
  HUD fixtures cover 2P/3P personal Power, life/Bomb labels, independent Extra
  thresholds, local/ghost identity, shared values rendered once, side-panel
  bounds, suppression of the old personal widgets, and unchanged RNG state.
  Stock markers/counts and numbers use the original native glyph queue.
  Final 2P/3P screenshots were visually checked: all seat groups fit, the
  local highlight changes by seat, shared totals appear once, and dimmed
  decorative panel art keeps the P3 resources readable.
- Protocol: 2P/3P, manual D=0/2/9, challenge on/off, reordered holes,
  capture-once ownership, session generation, build/ABI disagreement,
  all-seat replay including recorded P3, no second replay delay, malformed data,
  and rejection of gameplay versions 1 through 3 (previous resource/item rules).
- Common startup: calibration, immutable timing choice, lost-control repair,
  disconnect handling, and TH08/10/11/15 spectator timing identity regression.
- Real browser, production shell and Japanese 1.00b DATA, shared relay:
  2P reached frame 244 with a start-admitted spectator;
  3P reached frame 243. Both held frame zero until peers arrived,
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
