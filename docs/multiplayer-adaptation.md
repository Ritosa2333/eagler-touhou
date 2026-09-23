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

The user requested a persistent completion goal and no commits before the
complete adaptation is finished. Subagents must use Luna with xhigh reasoning.

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

Current evidence: source investigation and the pinned common library's
`rollback-journal-test.cpp` passing as Emscripten/WASM under Node. This tests
the journal algorithm, not restoration of TH08 owners. No multiplayer Runtime,
browser, cross-device or title rollback acceptance has been claimed.
