# TH15 multiplayer implementation evidence

Implementation and dependency topology: [README](README.md).
This is a local experiment; canonical branches have not been promoted.

## Production artifacts

Multiplayer WASM: `1c7047f4fa8ce6dac9567ca78a6e1fae52461888ec544d9b41fbc01635894935`.
Ordinary THPrac WASM: `805c45624dc50a1d7896f2bd6421c49a2b5841419a472f52b7907d95bf20cc5d`.
Both release builds and closed packages pass. The ordinary module has no MP
exports. Neither production module exposes development probes.
Common netplay pin: `dd93c944ce2f2830e99be9c8dba7ff2ac645f548`.

## Passing gates

- Co-op balance/state ownership (2026-10-13): two personal base Bombs at start
  and respawn, personal fragments retained, Bomb debit isolated; teammate
  protection extends short timers without shortening longer respawn grace.
  Every seat can die without reducing teammates' damage. Real EnemyDamage
  traversal applies native protection before Boss scaling; ordinary enemies
  retain pooled native damage. Per-enemy fractional carry preserves tiny hits;
  temporary respawn retains participant count, exhaustion/ghosts reduce it.
  Production ECL spell services retain remote Bomb startup protection with
  either Reimu or Reisen as primary; it ends after the final damaging Bomb,
  while Reisen-only shields keep native spell behavior. Shields activated
  during the first 60 spell frames retain protection until their own end;
  pre-existing shields do not prolong a teammate's startup Bomb reduction. Remote committed misses
  update chapter, stage and ECL counters once. Overlapping remote pilots apply
  the native special-bullet velocity factor only once per bullet.


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
  Small/large P retain their types and appearance even with all pilots full.
  Retail full-Power score rewards are credited once without increasing blue
  point counts or shared point value; a retained field P can refill a pilot
  after later Power loss. ECL Power reads the minimum of the viable team.
  Full Power fills one pilot or awards the shared full-Power point-value bonus
  once, respecting its cap. Cancel point-value items never use Power priority.
  HUD fixtures compare each seat's actual ANM sprite selection, fragment fill,
  dimensions and position with the original single-player life/Bomb widgets,
  including independent Extra thresholds. They cover native split Power
  typography, shared values rendered once, panel bounds, unchanged background
  colour and RNG state, and absence of textual stock/label substitutes.
  Production 2P/3P browser screenshots verify original labels, hearts/stars,
  fragment bars, separators, fonts and background, with grouped seat numbers.
  Three-player seat numbers occupy the gap beside the life label to avoid
  overlapping the preceding player's original Power row.
- Protocol: 2P/3P, manual D=0/2/9, challenge on/off, reordered holes,
  capture-once ownership, session generation, build/ABI disagreement,
  all-seat replay including recorded P3, no second replay delay, malformed data,
  and rejection of gameplay versions 1 through 6 (previous resource/item rules).
- Common startup: calibration, immutable timing choice, lost-control repair,
  disconnect handling, and TH08/10/11/15 spectator timing identity regression.
- Real browser, production shell and Japanese 1.00b DATA, shared relay:
  2P reached frame 242 with a start-admitted spectator;
  3P reached frame 246. Both held frame zero until peers arrived,
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

- Ghost state (2026-10-13): actual final-death transition, TH11-style bounded
  drift, input/resource/RNG isolation, local-seat independence, and original
  character sprite submission at translucent alpha despite native death hiding.
  Real bullet and moving/stationary/curved laser fixtures exclude ghosts and
  retain positive live-player Graze controls. Gameplay ABI is now version 7.
