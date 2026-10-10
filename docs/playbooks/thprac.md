# THPrac and Portable Practice Playbook

Status: active

## Purpose

Guide real semantic THPrac integration and portable practice behavior. Prevent a launch popup, host bypass or UI-only harness from being mistaken for a working Practice lifecycle.

## Applicability and authority

THPrac integration must use a topic worktree separate from upstream tracking
and the canonical Eagler integration worktree. Promotion follows
[Adaptation Worktree Isolation](adaptation-worktrees.md).

- TH06/TH07 reallyportable and source/contract evidence provides the reusable owner, lifecycle and proof method.
- A title without proven THPrac integration must not infer support from another
  title's UI or hook layout.
- Game semantics belong to the target portable/source contract. Shared names such as `State` do not prove shared lifecycle semantics.

<!-- knowledge-id: K-THPRAC-001 -->
## Normal design

The observable path is:

```text
Practice → character/shot → original stage-select position → ImGui/overlay
```

Integration must preserve real ECL hook, section, warp, replay, pause, restart and target semantics. The portable approach replaces DLL injection/D3D/Win32 dependencies with source hooks, a render-independent menu and an adapter; it does not replace game semantics with a host-only mock.

The first hook boundary is after ECL decompression and before pointer tables. Ordinary builds stay unhooked. Use an expected-byte/CRC boundary for the instruction site where the source contract requires it; translated ECL may have a different whole-file CRC policy, but the site check remains.

Track owners separately: live config, original menu memory, adapter/context, host session, Replay candidate, THPrac mode/target and each game's globals. Fresh Practice clears stale owners; `Mode=Original`, `fakeType`, Direct Frame, Chapter, Pause and Replay each retain their own semantics.

Important recorded differences include TH06 Pause-R versus Continue, TH06 time-stop gameplay state, TH07 `mRepStatus`, `THSectionPatch`, `THStageWarp`, reset `State(1)` and navigation states. Do not merge same-named fields across games.

<!-- knowledge-id: K-THPRAC-003 -->
## Purple THPrac shared library

Purple THPrac's title-independent tools belong to `eagler-common`, following
the source-level netplay component model. The optional C++17 target is
`eagler::thprac`; its types live in `eagler::thprac`. The implementation and
source-extraction authority are `eagler-common/docs/thprac.md` and
`eagler-common/tools/generate-purple-thprac.mjs`, not copied title generators.

### Shared and title-owned boundaries

All component paths below are relative to
`eagler-common/include/eagler/thprac/`.

| Shared component | Responsibility / consumer seam |
| --- | --- |
| `PracticeInput.hpp` | Auto-shoot latch, disable-key ordering and fast-retry input tools. The title owns input capture, logical-key conversion and session reset. |
| `PracticeKeyMonitor.hpp` | Native title key masks, 60-frame APS accounting, key recording and CSV. The title owns the monitor instance and authoritative recording cadence. |
| `PracticeSpeed.hpp` | Logical simulation period and Replay speed selection; not Launcher high-refresh presentation. |
| `PracticeKeyHud.inc` | Original purple key-display geometry, colors and drawing order. Bind `key_monitor` to the current session. |
| `PracticeReaction.inc` | Purple reaction test. Supply `PracticeCounter::QuadPart`, `practice_counter_frequency`, `practice_counter_now` and `practice_random_generator` from the platform adapter. |
| `PracticeSpeed.inc` | Purple speed widgets. TH08 supplies its existing reset flag as the optional third argument; preserve other titles' existing UI lifetime. |

Do not move game-specific F12 membership, cheat effects, ECL patches, section /
warp data, completion, Replay ownership, graphics hooks or resources into this
component. `PracticeCadence` remains title-owned: TH08/TH10 and TH11/TH15 have
different catch-up caps. Shared extraction must not erase those differences.
Sharing a tool does not enable it in a title whose original purple F12 menu
does not contain it, and does not imply blue THPrac has been migrated.

### Consumer integration

Include shared headers directly, for example:

```cpp
#include <eagler/thprac/PracticeInput.hpp>
#include <eagler/thprac/PracticeKeyMonitor.hpp>
#include <eagler/thprac/PracticeSpeed.hpp>
```

CMake consumers link `eagler::thprac` (or use
`eagler_common_link_thprac(target)` after including the common CMake helper).
Script-based builds add the common `include/` directory to the compiler search
path. Hash shared `.hpp` and `.inc` inputs into both the object-cache key and
build report; changing common code must invalidate cached title objects.
Do not keep title-local forwarding headers or regenerate private tool copies.

Include UI `.inc` components once in the title's private UI namespace, after
ImGui and required standard-library headers and adapter bindings. Ordinary
builds must not acquire UI/SDL dependencies merely by using the shared header
types. Preserve each title's existing THPrac enable/disable policy.

Localized widgets retain the title's `label(practice_...)` binding. Select its
locale through the existing Launcher language / Runtime THPrac locale contract;
do not add a competing language preference to the shared library. Touch input,
F12 / Tab / Backspace ownership and mobile function buttons also remain on the
existing Launcher-to-Runtime input path; this extraction adds no new protocol.
Retain `THPRAC-LICENSE.txt` (MIT, Ack 2022) when redistributing shared tools.

### Verification and publication

Run these commands from the common repository, with the actual purple source
checkout substituted for `<purple-source>`:

```sh
node tools/generate-purple-thprac.mjs <purple-source> --check
cmake -S . -B build -DEAGLER_COMMON_BUILD_TESTS=ON
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

Only the common generator's `--write` updates shared extraction. Title
generators check that authority and regenerate title-specific hooks/resources.
Also run each consumer's source checks, full THPrac build, supported ordinary
OFF build and the lifecycle/device gates below. Source equality and successful
compilation do not prove mobile visual or gameplay equivalence.

For unpublished local validation, explicitly set `EAGLER_COMMON_ROOT` to the
common checkout. Never silently replace an initialized, stale pinned dependency
with a sibling checkout. Publish the common commit first, then register/update
each consumer's `third_party/eagler-common` submodule to that exact published
revision, rerun canonical gates, and publish consumer changes. Do not push a
gitlink to an unpublished commit or ship a release requiring a local override.

Extraction status recorded on 2026-10-10: the shared implementation is
based on common `cedb710`. TH08/TH10/TH11/TH15 consumer changes
are isolated in `experiment/common-purple-thprac` topic worktrees, not promoted
to their daily integration branches. All four full THPrac builds, TH10/TH11/TH15
OFF builds, shared/source checks and 34 common tests passed. TH08 retains its
existing always-built practice adapter. Published experiment consumers pin
common `25d678c` through the Goan114 fork, including TH11's new submodule.
Device validation and promotion to daily integration branches remain pending.
This status is extraction evidence, not a Product Catalog support declaration.

<!-- knowledge-id: K-THPRAC-002 -->
## Invariants and pitfalls

- UI visibility is not semantic integration proof.
- Vanilla Pause remains the vanilla owner; THPrac must not create a competing Pause layer.
- Retry/restart replaces the old owner and generation; stale `isInReplay` or candidate state must not poison fresh Practice.
- Preserve ECL transaction/range checks and source proof. Do not weaken correctness checks or use hash gates to hide a mismatch.
- Keep proof debt visible. A historical `closed` state can be reopened by a later user report.

## Superseded approaches

Launch popups, UI-only harnesses, host bypasses and fake character state are not Practice integration. A previous proof-closed label is not a permanent guarantee after a later user report.

## Code anchors

- `integrations/RUNTIME_CONTRACT.md`: shared Host/Runtime integration boundary.
- The target title's Practice lifecycle, ECL/resource owners and THPrac adapter:
  title-specific semantic authority.
- the sibling TH06/TH07 Runtime practice owners and, when available in a full
  maintainer workspace, the external `thprac-reallyportable` reference project.
  These are reference sources, not files shipped inside this repository.

## Verification

1. Verify the hook/source boundary and ordinary-build OFF behavior.
2. Enter fresh Practice, select character/shot, reach original stage-select and open the real overlay.
3. Exercise section/warp, Pause, Replay, restart/retry, target completion and exit/re-entry.
4. Check owner/reset/mutation closure and compare logic-first evidence, not only UI screenshots.
5. Label source contract, automated lifecycle, browser and human/device evidence separately.

## Deliberately omitted claims

This playbook does not promote a visible Practice menu to proof of real ECL,
Replay, pause/restart or target semantics.
