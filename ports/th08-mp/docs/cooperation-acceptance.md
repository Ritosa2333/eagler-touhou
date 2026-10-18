# TH08 native cooperation acceptance

Status: completed local cooperation slice; not full multiplayer acceptance.

This slice adds native exhausted-player Spirit state, a 90-tick Focus rescue,
180-tick team-wipe grace, full-team native Continue reset, separated spawns,
and eight-press 20-power gifts. It preserves the original deathbomb/death
sequence before entering Spirit and uses the existing Player/Item/menu owners.
No alternate gameplay simulator or ordinary single-player behavior is added.

## Directed item ownership

Gift affinity is independent of transient homing. Cancel-homing and collect-all
may change an item's movement but cannot redirect a promised gift to a closer
third pilot. A recipient becoming unavailable releases the affinity; removal,
reset and slot reuse clear it. A full pool cannot charge the giver for an
incomplete six-item gift. The stable seat order determines otherwise tied
recipients, with Spirits preferred over live recipients for life rescue.

## Native browser evidence

The recovered source-matching diagnostic result is:

`artifacts/multiplayer-tests/cooperation-5539642e-f999-4aab-8c39-7e37d162a8aa/`

Both `suite.json` and `cases.json` pass all seven cases:

1. Native final death and 90-tick rescue, including release-before-repeat.
2. Three-player rescue with a genuinely closer live competitor and a Spirit.
3. Three-player targeted power gift.
4. Team wipe and original Retry/Continue menu, resources and spawn positions.
5. Gift affinity across native homing changes.
6. Pool exhaustion: no gift charge, RNG change or cursor change.
7. Recipient becoming Spirit releases gifts; ordinary slot reuse has no affinity.

Fixtures set explicit initial conditions and then run native logic. The
fixture-only exports are excluded from ordinary and production MP binaries.
This is neither network delivery nor late-input world-restoration evidence.

## Resumed build and regression results

After the gift-affinity fix, both ordinary and production MP were rebuilt.
Actual binaries and every recorded source file were checked against manifests:

| Build | WASM SHA-256 |
| --- | --- |
| Ordinary | `0d00a84ef6b214d43f2f365a6ffaa8bc6030a89c56b7d53868f3b9295b4cea3f` |
| Production MP | `1e6a6315b852f137a10cd1ad97eb3df608b6ca67ca90171e60e9ff0e387e65b3` |
| Diagnostic MP | `66d45b0ce9519da804def5616721611c8cd3da51d9cd10375952cf7243870232` |

The ordinary hash is identical to the pre-cooperation single-player baseline.
Build logs are `artifacts/multiplayer-tests/build-cooperation-resume-mp.log`
and `build-cooperation-resume-sp.log`. Production MP packaging also passed.

The cooperation, personal-resource, effect-slot, storage-isolation and enemy-
damage rule checks all passed. `portable/multiplayer/build-isolation.test.mjs`
plus `portable/package-eagler.test.mjs` passed five tests, including actual WASM
export isolation. These checks are separate from the seven native browser cases.

## Remaining product work and phase boundary

TH08 still requires full title-world rollback, frame-zero/session generations,
actual transport, multiplayer Replay, start-only read-only spectator, touch
and Launcher lifecycle integration, plus the final cross-title regressions.
No performance optimization, canonical promotion, push or deployment belongs
to this slice. Complete multiplayer functionality first, then assess performance
against a separately recorded ordinary/multiplayer functional baseline.
