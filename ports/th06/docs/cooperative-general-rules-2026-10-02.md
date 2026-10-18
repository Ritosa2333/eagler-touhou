# TH06 / TH07 cooperative general rules: bounded implementation

Status: local experimental changes; no push, merge, deployment or assets. Donation-package and dynamic boss decisions remain open. This document does not claim the complete requested ruleset is implemented.

## Audited bases

- TH06 `eagler`: `b3df2df27dd6b31fde2ec96f8fa881742563decc`
- TH07 `eagler`: `77af369be0b9282338111cb6e4007439d6497ebc`
- Separate topic worktrees; original checkouts and dependency pins unchanged

## Current / requested / decision matrix

| Rule | TH06 base | TH07 base | Candidate / decision |
|---|---|---|---|
| Fresh-life bombs | Config/default 3 | Shooter-specific initial bombs | 1 for first life, ordinary paid-stock respawn, new run/reset; stage survivor inventory retained |
| Ordinary-death power | 1 big + 5 small; loses up to16 power | Same | Preserved, including quantities |
| Final-death power | 5 Full Power items | Same | Preserved by explicit direction; no suppression or doubling |
| Final-death free life | Nearest survivor receives1 reserve without source debit | Same | Removed multiplayer-only free award |
| Enemy/ECL life/bomb drops | One copy per active player | Same | Original single-drop quantity; no power doubling |
| Donation gesture | 90 focused logical ticks in20-unit radius, no shooting; donor spends1 reserve | Same | Unchanged; one continuous hold permits one donation |
| Paid ghost rescue | Existing terminal bomb stock, zero lost power, 120 invulnerable ticks, 60 global-clear grace ticks | Same except shooter respawn timer | Fixed power and2–3 bombs awaiting decision; existing behavior preserved in this bounded candidate |
| Stage-transition ghost rescue | Re-registers ghost as playable, retaining terminal bombs | Same | New playable life gets1 bomb; living survivors keep earned stock |
| Ordinary enemy durability | Original HP; no boss factor | Original HP; 3P bomb-hitbox modifier exists | Unchanged |
| Boss durability | Team damage factor0.75 at2P;2/3 at3P | Same, except Stage4 chained cards | Current magnitudes preserved; dynamic permanent-loss adjustment pending |
| Power-gift pool exhaustion | Six-slot atomic capacity precheck | Deduct20 before up to6 spawns, allowing partial loss | TH07 now uses the same precheck; quantity/gesture unchanged |

## Implemented changes

1. Base bomb inventory1 for fresh cooperative lives and ordinary respawns. Stage reinitialization preserves survivors' earned stock. A former ghost receiving the existing free next-stage return gets1 bomb before its state is cleared. Stage replay resource snapshots remain authoritative.
2. Remove the multiplayer-only terminal-death life award. Death no longer creates a reserve life in a surviving teammate's bank. Vanilla ordinary/final-death item quantities remain intact.
3. Enemy/ECL life and bomb drops use exactly one `SpawnItem`, preserving the original spawn quantity. All direct item callers and power quantities remain unchanged.
4. TH07 refuses a20-power donation when fewer than six item slots are free. No debit or partial item batch occurs on capacity failure.
5. Deterministic compatibility labels advance through the existing mechanism: TH06 gameplay/live9→10; TH07 gameplay5→6 and live6→7. Old multiplayer peers/replays must be rejected rather than silently interpreted under changed rules. Single-player formats/build identity are unchanged.

## Exact boss behavior and pending options

No literal boss HP multiplication exists here. TH06 combines player damage, retains the team70-damage-per-tick ceiling, then applies boss-only factor0.75 for2 active roster slots or2/3 for3. Relative effective durability is therefore4/3 or1.5 before other game damage rules and rounding, not1.5/2.

TH07 applies its boss factor after native spell/invincibility handling and skips that factor for existing Stage4 chained-card phases. Separately,3P bomb-hitbox damage is multiplied by2/3 before the boss calculation; ordinary shot damage does not receive this extra factor. Neither exception was edited.

The current count includes roster ghosts and ordinary temporary death/spawning. Recommended pending option: count viable roster slots, excluding only terminal REVIVABLE/SPIRIT/ELIMINATED; retain temporary DEAD/SPAWNING and temporarily disconnected roster slots. On donation revival, restore the count. This changes effective remaining boss durability immediately without editing current/max HP or adding rollback state. An alternative literal HP rescale requires an explicitly chosen current/max/proportional and re-revival policy. Neither option is implemented in this bounded candidate.

## Donation decisions still required

- Proposed numeric package:2 bombs and64/128 power. Not applied without approval
- One donor reserve currently buys one playable recipient life without adding a spare reserve. Independently earned shared extends may already have accumulated in a ghost's bank; forcing stock to0 would erase these. Recommended pending behavior preserves earned reserves
- Existing120-tick rescue invulnerability is suitable for reuse. Existing60-tick bullet grace repeatedly clears the entire field, so it is not a local clear
- Neither title has an existing rescue radius. TH07's generic radius-clear routine converts bullets to point items, which would introduce an unwanted rescue reward. A local no-reward clear needs an approved radius and an explicit laser policy
- Ordinary spawn grace/invulnerability is unchanged. Terminal bomb stock remains unchanged until a separate paid-rescue package is approved, avoiding a partial zero-bomb rescue regression

## HIGH RISK inference / policy ledger

- **HIGH RISK — policy inference, implemented:** removing the old terminal-death survivor life grant treats it as extra multiplayer resource generation. This closes the direct donate/die life-reimbursement loop but changes legacy cooperative behavior
- **HIGH RISK — scope interpretation, implemented:** the base1-per-life instruction covers initial life, fresh resets and the existing free next-stage ghost return, while living stage survivors retain earned inventory. Paid donation rescue is a separate exception, pending approval
- **HIGH RISK — retained legacy balance:** ordinary-death low-power drops can exceed the power actually lost; five Full Power terminal drops can refill teammates. These vanilla quantities were explicitly retained. The candidate does not claim all death-item resource incentives are eliminated
- **HIGH RISK — unimplemented proposal:** two rescue bombs,64 power, one playable life with preserved earned reserves, and a local no-reward clear require final semantic/numeric agreement
- **HIGH RISK — unimplemented proposal:** dynamically count only nonterminal roster slots for the existing boss factor, restoring the factor after revival. No current/max HP rewrite is proposed
- **HIGH RISK — retained exception:** TH07's Stage4 chained-card exemption and3P bomb-hitbox factor remain because per-spell/per-title rebalance was outside scope, not because the candidate establishes their balance quality
- **HIGH RISK — compatibility consequence:** existing MP ABI gates intentionally reject older multiplayer peers and replay files. No automatic migration or historical-rule playback was added

**HIGH RISK — retained near-cap transfer behavior:** a receiver at127/128 is eligible for the full20-power gift, without prorating or refund. The recipient may gain only1 actual power. TH06 subsequent capped pickups award native score; TH07 reaching the cap converts other active small/big power items, including transferred items, to Cherry. The fixtures cover debit/emission, full-target exclusion and capacity-failure atomicity; they do not execute this native near-cap collection/conversion path. This patch does not claim lossless transfer.

The TH07 six-free-slot check is a concrete transactional correctness fix matching TH06, not a new balance rule. Original item quantities and unmodified ordinary-enemy HP are explicit requirements, not inferred numeric tuning.

## Verification

Passed:

- Existing native core suites for both titles, including common netplay, journal and input-lane behavior
- Actual shared `SessionGate` and packet codec with the title ABI constants:2P/3P old-ABI HELLO rejected; current HELLO/READY accepted; repeated HELLO idempotent
- Executable fixtures extract exact production function bodies at run time: ordinary/final death, terminal free-life removal, original power-drop quantities,2P/3P paid donation and held-input debounce, spectator-slot exclusion, zero donor reserve, target cap selection, no donor debit on item-pool capacity failure,20-unit debit/six-item emission, and item-pool0/5/6-free boundaries
- Exact production stage-revival helper fixture: former ghost gets1 bomb, living survivor retains4
- Exact production reset helper fixtures: fresh MP bomb1; TH06 recorded stage stock preserved
- Copied pre-frame restore/re-execution of the production donation functions yields identical tested player state
- Final Emscripten wasm32 syntax compilation of Player, ItemManager, MultiplayerResources, GameManager and AsciiManager translation units in both titles, MP+netplay enabled
- Optimized ordinary-build object identity against each audited base for Player, ItemManager, GameManager, AsciiManager and BulletManager: byte-for-byte identical, using the same compile path/toolchain
- `git diff --check`

Limits:

- Focused fixtures replace rendering/audio, `SpawnItem`, resource setters, timers and RNG with minimal stubs. They test selected production decision branches, not the real allocation/collection/checksum/RNG implementations. The copied-player re-execution test is not title rollback or replay proof
- Registration-to-stage-helper ordering is source-reviewed and the actual translation units are compiled; the whole RegisterChain lifecycle is not executed by the helper fixture
- The separate TH06 `multiplayer-resources-test.cpp` was compiled with actual `MultiplayerResources.cpp` to wasm and passed under Node, including fresh-start bomb1 and recorded stock preservation
- No full title link/build, real2P/3P+spectator match, local-bullet rescue integration, all-midboss/spell-state playthrough or full stage-replay validation was run. No assets were supplied/used
- Old-MP-replay rejection and spectator ABI wiring are source-reviewed only; the executable ABI test proves the actual shared live-peer gate, not replay/spectator integration
- Boss/safe-clear behavior is pending, so the current candidate does not certify those requested changes
- Toolchain headers come from the installed Emscripten SDL3 cache for syntax/object checks; full pinned-vendor linking was not performed

Reproduce focused behavior with `python3 tests/cooperative-rules-test.py` or the ordinary `scripts/run-core-tests.sh` entry point. Reproduce ordinary object identity with `EMXX=/path/to/em++ python3 tests/verify-coop-singleplayer-isolation.py`. Generated logs and binaries are kept outside source patches.
