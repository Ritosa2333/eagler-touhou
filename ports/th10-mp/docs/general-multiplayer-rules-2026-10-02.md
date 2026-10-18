# TH10 general multiplayer rules audit and evidence

## Scope and identity

- New isolated worktree: `rules-20261002/th10`, branch `experiment/rules-20261002`.
- Verified starting commit: `a0641219bae3c444da29824f5eedc404f07c075b`.
- Shared dependency remains `c239e13730eafc57f2cd309f77f449782bd05bbd`.
- General cooperative lifecycle/resource rules only. No difficulty tiers, per-spell tuning, performance-policy changes, merge, publication or deployment.
- **Partial implementation:** the numeric and dynamic-boss decisions below remain unapproved. No pending policy has been silently selected.

## Current / requested / gap / unclear matrix

| Area | Current verified implementation | Requested intent | Gap or decision |
| --- | --- | --- | --- |
| Ordinary enemies | Native health, full ordinary shot damage | Avoid tanky ordinary enemies | Already true for shots. Three-player spirit-strike damage is globally reduced to 2/3; removing that penalty would be a balance change requiring a decision |
| Bosses and midbosses | Boss flag `0x8000`; incoming damage ×0.75 for 2 seats, ×2/3 for 3 | Preserve existing overall scaling; ease remaining fight after elimination | Count is fixed roster; viable-player/donation behavior pending |
| Scripted item drops | Original kind/count loops, no roster multiplier | Single-player quantities | Preserved; no drop scaling introduced |
| Native death | Native life decrement, power −64 units with zero floor, seven authored power items | Keep native title behavior | Preserved |
| Bomb1 per life | TH10 has no discrete bomb stock; spirit strike costs 20 native power units = 1.00 | Adapt without fabricating bomb inventory | Whether to guarantee minimum 1.00 power at start/respawn is unapproved; unchanged |
| Donation resources | Existing power inherited; negative life promoted to zero, banked spirit lives retained | Fixed usable power and one playable life | Proposed 3.00 power = 60 units and zero reserves await approval |
| Donation safety | Existing 280-tick invulnerability; native state timer reset to zero | Local bullet clear plus safe reentry | Timer zero accidentally invokes 30 ticks of native screen-wide clearing; local radius and timer fix pending chosen policy |
| Final death reward | Previously gave nearest surviving partner a free life | Avoid life duplication / donation farming | Removed MP-only free life; native drops retained |
| Compatibility | Live/spectator ABI v5; old v4 MP Replay accepted | Deterministic rule compatibility | Raised gameplay ABI to v6; old v4/v5 MP Replay rejected |

## Implemented changes

1. Removed the extra survivor life from `Frame::game_over` in `WorldPlayerMultiplayer.cpp`. The native death penalty and seven authored power drops still execute once before the Spirit state. A later rescue spends an existing donor life.
2. Bumped `GameplayContract` from version `0x10000005` to `0x10000006`. The existing live and spectator gates now distinguish old and new deterministic semantics.
3. `ReplayArchive` accepts the current gameplay contract only. It no longer silently replays v4/v5 inputs using v6 resource rules. Old MP recordings require their matching older runtime; ordinary single-player Replay code is unchanged. Replay description/checkpoint format versions are separate and remain supported when their gameplay ABI matches.
4. Added behavioral live/spectator/replay compatibility tests, including 2/3-player spectator read-only input ownership and nonmutation on rejected archives.
5. Corrected the diagnostic-only fixture timer initializer. `Timer::initialize(value)` sets the previous sample, not the current timer; the fixture now sets current/fractional/previous explicitly. This restores the intended 60-tick active-state timer and 10,000-tick protection for controlled initial conditions. No fixture control enters ordinary or production multiplayer builds.

## Dynamic boss HP analysis (decision pending)

TH10 does not multiply `health` or `maximum_health` by the number of players. `EnemyState::update` applies the existing boss damage multiplier after native spell damage rules. ECL writes native maximum health and interrupt thresholds, and `EnemyPhase` compares those raw values.

Recommended option: derive viable player count from the rollback-owned native pilot state, exclude final-death/Spirit players, retain normally respawning players, and apply the existing multiplier for that count. Restore the count on donation revival. Keep raw current/max HP and all phase thresholds unchanged.

- With raw remaining HP H, effective ordinary-shot durability is approximately H/(2/3), H/0.75 or H/1, subject to the existing per-hit truncation. Going 3→2→1 lowers that remaining work without an HP-bar jump or accidental phase transition.
- This also leaves native spell resistance, timeout rules, midboss/boss flag classification and all authored ECL thresholds untouched.
- Alternative: rewrite maximum/current HP proportionally and adjust every interrupt threshold. This has higher ECL and health-bar risk and would need substantially wider native phase testing.
- Reversal on revival versus retaining the lower multiplier through the phase is a gameplay choice. Neither was selected here.

## HIGH RISK inference and policy ledger

Every inferred, adapted or previously invented rule is explicitly marked HIGH RISK; the label is about design authority, not a claim of measured code failure.

| Risk | Rule / assumption | Status |
| --- | --- | --- |
| **HIGH RISK** | Removing final-death survivor +1 prevents donate/death life repayment; this changes an existing cooperative rule | Implemented under the requested anti-farm direction; no retail TH10 equivalent |
| **HIGH RISK** | Donation fixes power to 3.00, including resetting a previously full-power spirit | Pending numeric approval; not implemented |
| **HIGH RISK** | “Life+1” means one playable ship at TH10 reserve count zero, discarding banked ghost reserves | Pending approval; not implemented |
| **HIGH RISK** | Mapping Bomb1 to at least 1.00 retained power would affect starting power, ordinary respawns and/or stage recovery | Unresolved; native behavior retained |
| **HIGH RISK** | A 64-pixel donation clear would reuse the first native respawn radius | Reported precedent only; radius not approved or applied |
| **HIGH RISK** | Existing donation invulnerability lasts 280 ticks | Existing adaptation retained; no new duration invented |
| **HIGH RISK** | Boss scaling follows currently viable pilots and reverses on revival while keeping raw HP fixed | Pending approval; not implemented |
| **HIGH RISK** | Existing global 3-player spirit-strike damage ×2/3 affects ordinary enemies too | Existing behavior retained pending explicit keep/remove decision |
| **HIGH RISK** | Existing Spirit drift ±0.2 pixels/tick and stage-boundary revival of every seat | Existing inherited adaptations retained; outside this patch’s selected changes |
| **HIGH RISK** | Existing 20-pixel / 90-tick / one-spare-life / focus-release donation and 180-tick wipe rules come from cooperative adaptation | Existing behavior retained; no new values |

## Verification

### Passed

- All 33 official WASI SDK 34 resource-free rule entries pass, including new live/spectator compatibility, MP Replay rejection and original enemy-drop count/RNG tests in both ordinary and multiplayer builds.
- Actual production Application/World/ECL lifecycle on a CPU-only WASI host for 2 and 3 players: final death, donor resource conservation through two donation/death cycles, genuine revived-player re-death, and a wrongly predicted donation cancelled by a six-tick-late release.
- Same-build exact/delayed CPU runs match **all 44 canonical words**, native status and Replay status at **199 confirmed checkpoints per roster** (398 total). Each delayed run performs one real rollback and six resimulated ticks, followed by a successful real donation.
- All three full Emscripten 6.0.9 Web builds pass:
  - Ordinary: `80ad72a9ac34e0ebf3351cc897850eabe0f60f7108b55a1b7a6804e31423a01e`, 2,906,942 bytes
  - Production multiplayer: `36a5f27c6380db717e396333f73358ceafc03d332c9b0809e437308671d56af8`, 3,092,779 bytes
  - Diagnostic fixtures: `63372725d5da950afdee2c2f8861e227a196b2fdcf7364afca4eb3eae936318c`, 3,102,967 bytes
- Linked-binary isolation verifies current source inventories, loader/WASM hashes, zero multiplayer exports in ordinary and zero fixture exports in production. All five source/build-plan isolation tests also pass. Every one of the ordinary build’s 473 inventoried source inputs also matches the exact starting commit’s Git blob (zero changed or unavailable entries); this is source isolation, not a fresh retail Replay-golden rerun.

### Evidence locations

Generated evidence stays in ignored `artifacts/general-rules-cpu/`: build identities, 2P/3P exact/delayed JSONL, `report.json`, resource-free logs and full-build logs. The CPU adapter was copied read-only into this new worktree from the prior private test checkpoint; no existing worktree was edited. It is diagnostic evidence, not shipped game code.

### Reproduction and prerequisites

The **source patch alone** reproduces the 33 resource-free rule entries and five source/build-plan tests. It also reproduces full Web builds with official Emscripten 6.0.9 and its installed SDL ports. From the repository root:

```sh
WASI_SDK_PATH=/path/to/wasi-sdk-34 node portable/check-multiplayer-rules.mjs
node --test tests/multiplayer-build-isolation.test.mjs portable/multiplayer/build-isolation.test.mjs
EMSDK=/path/to/emsdk EM_CONFIG=/path/to/emsdk/.emscripten node portable/build.mjs --th10
EMSDK=/path/to/emsdk EM_CONFIG=/path/to/emsdk/.emscripten node portable/build.mjs --th10 --multiplayer
EMSDK=/path/to/emsdk EM_CONFIG=/path/to/emsdk/.emscripten node portable/build.mjs --th10 --multiplayer --multiplayer-fixtures
node portable/multiplayer/check-built-isolation.mjs
```

The 398 full-owner CPU comparisons are **not reproducible from the source patch alone**. They additionally require the untracked diagnostic adapter and driver retained in this worktree: `artifacts/general-rules-cpu/{host-prefix.hpp,host.cpp,build.mjs,run.mjs}`. The adapter is derived from the earlier private CPU-host checkpoint; it is not part of the deliverable patch and no private logs or retail assets are added to source control. Rerun in this retained worktree:

```sh
WASI_SDK_PATH=/path/to/wasi-sdk-34 node artifacts/general-rules-cpu/build.mjs
TH10_DATA_PATH=/path/to/owned/th10.dat node artifacts/general-rules-cpu/run.mjs
```

The native data used for the reported comparisons has SHA-256 `1fb1d0ffe34115f563f5feb43755c0feee2315b0ac2b32e2f9e84c81e9433bea`. The runner verifies the compiled source inventory and WASM identity, records the supplied data and runner hashes, and retains both exact/delayed traces. Different retail resources constitute a separate rerun, not this exact evidence identity. A recipient with only the patch must obtain an authorized copy of the diagnostic adapter or supply an equivalent audited CPU host before claiming to reproduce these comparisons.

### Limits and unfinished acceptance

- Donation fixed resources, local clear and dynamic boss downgrade remain pending decisions, so this is not completion of the requested general rule set.
- The CPU host executes original authored updates/Draw and collision/resource owners but mocks GPU rasterization and glyph pixels. It does not establish browser transport, visible bullet-clear rendering, physical audio, mobile pacing or cross-device acceptance.
- Native browser fixture runs are blocked by absent licensed font assets. No font or retail asset was invented or uploaded.
- No full native midboss/spell/health-bar acceptance has been claimed; current boss findings are source audit plus the existing arithmetic behavior tests.
- The full-engine sanitizer and independent retail Replay gates were not rerun for this patch. Prior performance work’s sanitizer limitations are not reclassified as passing.
