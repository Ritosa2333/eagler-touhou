# Replay branch coverage audit

## Notification baseline and scope

The first explicit unified branch/build notification that can be confirmed in
this chat is the user's message relayed on **2026-10-17 21:03 Asia/Singapore**.
No earlier notification time is invented. TH10/TH11 already had separate
ordinary/MP owners in the original task. This audit retrospectively checks all
Replay changes owned by this chat, including the earlier TH06/TH07 seek work.

The original branch-coverage conclusion was incomplete: ordinary TH08/TH15
artifacts built from an MP checkout do not prove that the independent `eagler`
branch contains the feature. Native artifact checks remain valid for their
named bytes; their branch-coverage interpretation is corrected here.

## Findings and repairs

| Title | Ordinary coverage | MP coverage | Audit action |
| --- | --- | --- | --- |
| TH06 | Shared Replay adapter present on `eagler`; separate normal artifact and native checks already passed | Separate multiplayer artifact and real 2P/3P checks already passed | No missing Replay source. Rechecked OFF/ON CMake profiles and existing reports |
| TH07 | Adapter present on `eagler`; original ordinary checks passed | Independent `eagler-mp` branch lacked the adapter | Added the same adapter, render-swap guard, SDL loop ownership and shell commands to MP. Rebuilt and tested both CMake variants |
| TH08 | `eagler` lacked local Replay entry and shell commands | `eagler-mp` already contains the feature and has prior real 2P/3P artifact checks | Added native ordinary entry, metadata/controls, one-folder listing/import and shared shell controller to the actual ordinary branch |
| TH10 | `eagler` contains the native adapter and common controller; prior ordinary build/checks passed | `eagler-mp` contains its native adapter and controller; prior real MP/stage-offset build/checks passed | No missing Replay synchronization |
| TH11 | `eagler` contains the native adapter and controller; prior ordinary build/checks passed | `eagler-mp` contains its native adapter and controller; prior 2P/3P format-corpus build/checks passed | No missing Replay synchronization |
| TH15 | `eagler` lacked local Replay entry and shell commands | `eagler-mp` already contains its native entry, MP reopening and controller, with prior format-corpus checks | Added the native adapter and ordinary-only shell wiring to `eagler`; retained the MP-owned reopening path on MP |

The same local Replay controller block is present on all **11 inspected refs**:
TH06 `eagler`, plus `eagler`/`eagler-mp` for TH07/08/10/11/15. The repaired
TH07/TH08/TH15 native adapter files match their counterpart branch byte-for-byte
after newline normalization. Entry wiring remains title/mode specific.
Commit ancestry alone was not used to detect a missing feature: several refs
contain the code through copied/rebased history without the original commit id.

TH09 retains its existing VS/round player and received no native modification
from this chat. TH20 remains outside this task's native-adapter claims.

## New build and native evidence

| Actual build owner | Build | Observed check | WASM SHA-256 |
| --- | --- | --- | --- |
| `th07-eagler`, checked out on `eagler-mp` | `build-web-release-normal`, `TH_ENABLE_MULTIPLAYER_GAMEPLAY=OFF` | Original Stage 3 Demo, metadata, pause, backward seek 20 and rate | `9366a408949cad6a70929268a66cdb5f8c48516723fcbe8db95e9907cc8d5e97` |
| Same TH07 source checkout | `build-web-release-multiplayer`, flag `ON` | Real native 2P/3P recordings, restored seat counts and independent input lanes, pause, backward seek 20, rate and recorded EOF | `4a9dbd1e2c506598f19c6bfef8908d3d4d3f172747a55404466336f87def44b9` |
| `th08-eagler` on `eagler` | `node portable/build.mjs --th08`, ordinary `sdl3` | All four original Demos, selected native entries, pause/restart and rate | `07fc8cef099ecc142bca0cb70b8fc28fd2a24a6a70bf174284727c0c06821504` |
| Same actual TH08 ordinary branch | Same binary | Ten native-format entries, branches/Extra/Spell Practice, hot import/overwrite | Same hash |
| `th15-eagler` on `eagler` | `node th15_web/scripts/build-sdl-application.mjs --release` | All three original Demos, selected native entries, pause, backward reconstruction and rate | `31c14c884dd60d2307680750d036ab0b42d7dcaf9f859e53d95d514ee81952b0` |

TH07 is deliberately described by its two actual compile variants, not by
pretending its OFF build came from the separately checked `eagler` ref. The
ordinary ref already contains the identical native adapter. TH08/TH15 ordinary
validation above uses their actual ordinary branches.

Reports/logs live under ignored `artifacts/replay-workspace/branch-audit-2103/`:
`th07-normal.json`, `th07-mp.json`, `th08-normal.json`, `th08-special.json`,
`th15-normal.json`, and `source-coverage.json`. Runtime build logs are
`artifacts/replay-branch-normal-build.log` in each affected game; TH07 also has
`artifacts/replay-branch-multiplayer-build.log`.

TH08's original `.dat` was selected explicitly from the existing private input
because the old automatic resource lookup no longer matched the ordinary
checkout. Its format corpus is encoded by the real native codec using retained
Demo inputs. It establishes format/entry behavior, not authentic performances
of each branch/spell. Ordinary TH08 still has unknown duration and disables
arbitrary position seek. No duration was fabricated.

The build identities for the repaired TH08/TH15 native entry sources match the
actual source files. All four new build processes exited successfully. The
common bridge's eight tests pass. No retail golden or physical-device claim is
added by these control checks.

## Prior evidence and freshness boundaries

TH06, TH10, TH11 and the unchanged TH08/TH15 MP counterparts retain the separate
named-artifact evidence recorded in the earlier Replay session. The current
source refs were rechecked for the required Replay adapter/controller. This
audit did **not** rebuild those unchanged counterparts at their latest HEAD,
and does not claim a new latest-HEAD or full-game regression PASS for them.
TH11/TH15 MP's older input corpora are native-codec software fixtures, not
naturally recorded multi-stage performances.

## Test ownership and commits

The directory native checker now requires `eagler` for ordinary and `eagler-mp`
for MP, records repository/branch/HEAD/dirty state, checks the WASM build hash,
and checks the declared hashes of its Replay entry sources. MP recording checks
support `--repo=PATH` for TH08/TH10 and fail before starting a service if the
selected checkout is on the wrong branch. Both wrong-owner rejection paths
were exercised. The TH06/TH07 shell checker independently reads the CMake
multiplayer compile flag and records it beside the distinct WASM hashes. A
cache configured for the wrong variant fails before playback. Known native
2P/3P fixtures also check the observed seat count and independent input lanes.

| Repair | Commit |
| --- | --- |
| TH07 MP | `9d8af17` |
| TH08 ordinary | `f81d08c` |
| TH15 ordinary | `a9e4432` |

The existing shared baseline `2452cb8` did not contain the Replay helpers.
Compatibility ref `replay-workspace-2452cb8` adds exactly the six Replay files
as `ccd66ce`, preserving that baseline's audio work. TH07/TH08 now pin this
commit. The analogous `08974f6` compatibility ref `30d2fcd` is retained locally
but is not required by these final parent pins. Shared main already contains
the same Replay implementation.

Other chats' staged audio/build edits were preserved and had been committed by
their owners before the final Replay repair commits. No worktrees were created,
no branches were switched or merged, and nothing was pushed, published or
deployed by this audit.
