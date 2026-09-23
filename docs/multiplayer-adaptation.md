# TH08 multiplayer adaptation

Status: design and implementation in an isolated Eagler experiment. This is not
a multiplayer-capability declaration or a playable Runtime.

## Baseline and isolation

- Upstream tracking: `main`, `fa94b0d`.
- Eagler integration base: `eagler`, `cb1bad6`.
- Experiment: `experiment/th08-multiplayer`.
- Shared runtime dependency: `8316c4f861dedb67e1e0e7be75ddcf0b90f63448`.
- Ordinary gameplay, Replay formats, storage and build outputs retain their
  existing behavior. Multiplayer requires a separately compiled Runtime.

The implementation authority is `th08_web/cpp/game/GameplayScene.*` and its
production `PlayerScene`, `EnemySystem`, `BulletSystem` and `ItemSystem` owners.
The old `GameRuntime` prototype is not an integration point.

## Agreed cooperation rules

The requested reference is TH07's cooperative game: one shared stage, enemies,
bullets and score; personal lives, power and bombs; rescue of exhausted players
and a synchronized team-wipe/retry lifecycle. TH08 retains each loadout's native
human/youkai forms, shooting, bomb and deathbomb behavior.
The current cooperative interaction copies TH07's 90-tick Focus rescue, lowest-
resource recipient, 180-tick wipe delay, and eight Shoot presses within 24
ticks to give 20 power through two big and four small targeted items. These
rules are compiled only into multiplayer variants.

TH07's implementation is evidence, not an authority for TH08-specific values.
In particular, its shared Cherry mechanics, Stage 4 chained-card exception,
P1-only final resource bonus and reported resurrection/game-over defect must
not be copied implicitly.

The user approved these TH08-specific rules:

- Personal human/youkai gauge and current form.
- Shared time-orb pool, night clock and stage progression.
- Story/route selection follows the host's selected loadout.

Simultaneous native deathbombs must observe an explicit stable seat order and the same remaining shared
balance on every endpoint. Do not silently award each seat a private copy of
the shared balance. Spell failure and capture are properties of the shared
spell; individual player actions must reach that same owner.

The completion goal remains the entire multiplayer product. The user's newer
instruction permits a commit after each substantial completed and validated
piece. Subagents must use Luna with xhigh reasoning.

Complete all multiplayer functionality and correctness before performance
work. Preserve and independently verify the ordinary single-player baseline;
commit the complete functional boundary before any later shared optimization.
Keep optimization commits and their ordinary/multiplayer regression evidence
separate, so a single-player divergence can be attributed to the correct
change rather than to mixed feature and performance work. Never replace a
golden to hide a difference.

## Implementation boundary

`eagler-common` owns protocol, session barrier, logical input history,
prediction, transport and rollback storage algorithms. This repository owns
the player roster, gameplay rules, authoritative-state inventory, state
restoration and lifecycle. No second transport or protocol is needed.

Create one shared world with explicit player contexts. Existing constructors
bind a single player and resource set by reference; adding input lanes alone
does not change that ownership. Collision, damage, targeting and pickup
selection must consume the roster explicitly. Do not duplicate the whole
`GameplayScene` per seat or rotate a hidden global current-player context.

Frame restoration must cover the actual mutable ECL/ANM owners and allocation
lifetimes, not just position, RNG or diagnostic hashes. TH08's ordinary fixed
tick includes semantically relevant Draw work; a replayed tick must retain
those effects while separating presentation and external side effects.

## Acceptance before promotion

1. Separate ordinary/MP build, storage and Replay identities; ordinary source
   and golden Replay gates remain unchanged.
2. 2P/3P loadouts, per-seat input and frame-zero barrier.
3. Collision, targeting, bomb overlap, pickup ownership, shared rewards and
   simultaneous deathbomb/resource effects.
4. Rescue, life rewards after exhaustion, full wipe, synchronized pause,
   restart generation, stage/route transition and results.
5. Late-input correction restores every declared owner and reproduces the
   uninterrupted authoritative state; report the first divergent frame/owner.
6. MP Replay, start-only spectator, transport and Launcher lifecycle checks.
7. Complete experiment diff review, then canonical integration and regression.

## Current local evidence (2026-09-23)

The newer source-matching seven-case cooperation acceptance and resumed
ordinary/MP rebuilds are recorded in [cooperation-acceptance.md](cooperation-acceptance.md).
That record supersedes the earlier four-case/hashes below, which remain as
historical provenance. In particular, production MP now preserves directed
gift affinity across homing cancellation, recipient death and pool reuse.

- The ordinary and production multiplayer WASM build independently. The latest
  ordinary WASM SHA-256 is `0d00a84ef6b214d43f2f365a6ffaa8bc6030a89c56b7d53868f3b9295b4cea3f`,
  unchanged by the multiplayer lifecycle and power-gift changes. The production
  multiplayer WASM SHA-256 is `2f2d6a8af6f7d1017d397f65d76c81e4ae7fa9c6b8888cbe3117deba9a6682bc`.
- `node portable/package-eagler.mjs --multiplayer` produced the separately
  identified `build-eagler-multiplayer` package. Package fixture tests cover
  ordinary, multiplayer and Presentation Lab identity and hash rejection.
- `node portable/check-multiplayer-cooperation.mjs` passes the pointer-free
  2P/3P rescue priority, focus-release, targeted item allocation, eight-tap
  power gift, stage-local gesture reset and 180-tick wipe rules. `python
  portable/multiplayer/check-cooperation.py` passes four real-browser/native
  fixtures: 2P final death/rescue, 3P spirit priority, targeted 3P power gift,
  and full wipe followed by the original Retry menu and whole-team resource
  reset. Its report is `artifacts/multiplayer-tests/cooperation-native.json`.
  The diagnostic fixture WASM SHA-256 is
  `9cd4e5016781df008def3486591f91feccc5410fe0694d13c96db3882a662dfe`.
- `node --test portable/multiplayer/build-isolation.test.mjs` confirms the
  fixture exports are absent from ordinary and production multiplayer WASM.
  The diagnostic fixture WASM is a separate build and is never packaged.
- Real TH08 DATA and native artwork were used to check 2P/3P HUD placement.
  Screenshots are `artifacts/multiplayer-tests/hud-th08-2p-final.png` and
  `hud-th08-3p-final.png`.

These checks prove local gameplay and package construction, not a playable
network product. The TH08 world journal/late-input correction, frame-zero
barrier, multiplayer Replay, spectator, actual transport and Launcher room
lifecycle still require implementation and acceptance. The pinned common
library's `rollback-journal-test.cpp` covers its generic algorithm only, not
restoration of TH08 owners.
